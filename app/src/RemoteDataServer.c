#include "pthread.h"
#include "stdio.h"
#include "sys/types.h"
#include "string.h"
#include "errno.h"
#include "json.h"
#include "RemoteDataServer.h"
#include "Socket.h"
#include "Common.h"
#include "UartApi.h"
#include "Config.h"
#include "DataQueue.h"

static void		RdsThrd(void *pArg);
static void 	GetRemoteUserCmd(SocketParam SocketParam, uchar *pBuff, int len, RdsPack txPack);
static void		SendDevData(SocketParam socketParam, uchar *pBuff, int len, RdsPack txPack);

/***********************************************************************
**Function Name	: Rds_Start
**Description	: start remote data server.
**Parameters	: none.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
int	Rds_Start()
{
	pthread_t thread = {0};
	pthread_attr_t thread_attr = {0};
	int res = 0;

	//init attr
	res = pthread_attr_init(&thread_attr);

	if (-1 == res)
	{
		l_debug(ERR_LOG_PATH, "%s: thread attr init failed", __FUNCTION__);
		return -1;
	}

	//set cpu againt scope(process)
	res = pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);

	if (-1 == res)
	{
		l_debug(ERR_LOG_PATH, "%s: set cpu againt scope failed", __FUNCTION__);
		return -1;
	}

	//set thread detach 
	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	if (-1 == res)
	{
		l_debug(ERR_LOG_PATH, "%s: set thread detach failed", __FUNCTION__);
		return -1;
	}

	//create thread
	res = pthread_create(&thread, &thread_attr, (void*)RdsThrd, NULL);

	if (-1 == res)
	{
		l_debug(ERR_LOG_PATH, "%s: create thread failed", __FUNCTION__);
		return -1;
	}

	pthread_attr_destroy(&thread_attr);

	return 0;
}
/***********************************************************************
**Function Name	: Rds_JsonParse
**Description	: parse a rds json str.
**Parameters	: pPack - save result.
				: pJsonStr - json string.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
int	Rds_JsonParse(RdsPack *pPack, const uchar *pJsonStr)
{
	int array_len = 0;
	int i = 0;
	struct json_object *p_rds_pack = NULL;
	struct json_object *p_obj = NULL;
	struct json_object *p_ele = NULL;

	if (NULL == pPack || NULL == pJsonStr)
	{
		return -1;
	}

	p_rds_pack = json_tokener_parse(pJsonStr);

	//middle no
	p_obj = json_object_object_get(p_rds_pack, "middleNo");
	memcpy(pPack->m_MiddleNo, json_object_get_string(p_obj), 10);
	l_debug(NULL, "middleNo:%s", pPack->m_MiddleNo);

	//port no
	p_obj = json_object_object_get(p_rds_pack, "portNo");
	array_len = json_object_array_length(p_obj);
	pPack->m_Ports = RDS_PORT_NO_EMPTY;

	for (i = 0; i < array_len; ++i)
	{
		p_ele = json_object_array_get_idx(p_obj, i);

		pPack->m_Ports |= (RDS_PORT_NO_01 << (json_object_get_int(p_ele) - 1));
		l_debug(NULL, "portNo:%d", pPack->m_Ports);
	}

	//termial no
	p_obj = json_object_object_get(p_rds_pack, "termialNo");
	array_len = json_object_array_length(p_obj);

	//array_len <= RDS_TERMIAL_SIZE;
	array_len = (array_len > RDS_TERMIAL_SIZE ? RDS_TERMIAL_SIZE : array_len);
	pPack->m_TermialSize = array_len;

	for (i = 0; i < array_len; ++i)
	{
		p_ele = json_object_array_get_idx(p_obj, i);

		pPack->m_TermialNo[i] = (ushort)json_object_get_int(p_ele);
		l_debug(NULL, "termialNo:%.5d", pPack->m_TermialNo[i]);
	}

	//data type
	p_obj = json_object_object_get(p_rds_pack, "dataType");
	pPack->m_DataType = json_object_get_int(p_obj);
	l_debug(NULL, "dataType:%d", pPack->m_DataType);

	//data
	p_obj = json_object_object_get(p_rds_pack, "data");
	array_len = json_object_array_length(p_obj);
	array_len = (array_len > RDS_DATA_SIZE ? RDS_DATA_SIZE : array_len);
	pPack->m_DataLen = array_len;

	for (i = 0; i < array_len; ++i)
	{
		p_ele = json_object_array_get_idx(p_obj, i);

		pPack->m_Data[i] = json_object_get_int(p_ele);
		l_debug(NULL, "data:%d", pPack->m_Data[i]);
	}

	json_object_put(p_rds_pack);

	return 0;
}
/***********************************************************************
**Function Name	: Rds_JsonString
**Description	: convert RdsPack to a json string.
**Parameters	: pack - in.
				: pJsonStr - out.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
int Rds_JsonString(RdsPack pack, uchar *pJsonStr)
{
	int i = 0;
	struct json_object *p_rds_pack = NULL;
	struct json_object *p_array = NULL;
	uchar tmp[10] = {0};

	if (NULL == pJsonStr)
	{
		return -1;
	}

	memset(pJsonStr, 0, sizeof(pJsonStr));

	p_rds_pack = json_object_new_object();

	//middle no
	json_object_object_add(p_rds_pack, "middleNo", json_object_new_string(pack.m_MiddleNo));

	//port no
	p_array = json_object_new_array();

	if (RDS_PORT_NO_EMPTY != pack.m_Ports)
	{
		if ((pack.m_Ports & RDS_PORT_NO_01))
		{
			json_object_array_add(p_array, json_object_new_int(1));
		}

		if ((pack.m_Ports & RDS_PORT_NO_02))
		{
			json_object_array_add(p_array, json_object_new_int(2));
		}

		if ((pack.m_Ports & RDS_PORT_NO_03))
		{
			json_object_array_add(p_array, json_object_new_int(3));
		}

		if ((pack.m_Ports & RDS_PORT_NO_04))
		{
			json_object_array_add(p_array, json_object_new_int(4));
		}
	}

	json_object_object_add(p_rds_pack, "portNo", p_array);

	//termial no
	p_array = json_object_new_array();

	for (i = 0; i < pack.m_TermialSize; ++i)
	{
		json_object_array_add(p_array, json_object_new_int(pack.m_TermialNo[i]));
	}

	json_object_object_add(p_rds_pack, "termialNo", p_array);

	//dataType
	json_object_object_add(p_rds_pack, "dataType", json_object_new_int(pack.m_DataType));
	
	//data
	p_array = json_object_new_array();

	for (i = 0; i < pack.m_DataLen; ++i)
	{
		json_object_array_add(p_array, json_object_new_int(pack.m_Data[i]));
	}

	json_object_object_add(p_rds_pack, "data", p_array);

	memcpy(pJsonStr, 
		json_object_to_json_string(p_rds_pack), 
		strlen(json_object_to_json_string(p_rds_pack)));

	//l_debug(NULL, "%s", pJsonStr);

	json_object_put(p_rds_pack);
	json_object_put(p_array);

	return 0;
}
/***********************************************************************
**Function Name	: RdsThrd
**Description	: this is a thread to recv or send to remote sever.
**Parameters	: none.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
static void	RdsThrd(void *pArg)
{
	RdsPack tx_pack = {0};
	SocketParam socket_param = {INVALID_SOCKET, 0, 0};
	uchar buff[RDS_BUFF_SIZE] = {0};

	l_debug(RUN_LOG_PATH, "-------start remote data server thread-------\n");

	if (0 != Socket_GetNetConifgInfo(&socket_param))
	{
		l_debug(ERR_LOG_PATH, "%s:get net param failed", __FUNCTION__);
		return;
	}

	memcpy(tx_pack.m_MiddleNo, "CS00000001", 10);
	tx_pack.m_Ports = RDS_PORT_NO_EMPTY;
	tx_pack.m_TermialSize = 0;
	memset(tx_pack.m_TermialNo, 0, RDS_TERMIAL_SIZE);
	tx_pack.m_DataLen = 0;
	memset(tx_pack.m_Data, 0, RDS_DATA_SIZE);

	while (1)
	{
		//connect server
		socket_param.m_Fd = Socket_ConnectServer(3, socket_param);

		if (INVALID_SOCKET != socket_param.m_Fd)
		{
			//get user cmd
			GetRemoteUserCmd(socket_param, buff, RDS_BUFF_SIZE, tx_pack);

			//send Device data
			SendDevData(socket_param, buff, RDS_BUFF_SIZE, tx_pack);

			//notify server close connection
			tx_pack.m_DataType = RDS_DATA_TYPE_CLOSE_CONN;
			memset(buff, 0, RDS_BUFF_SIZE);
			Rds_JsonString(tx_pack, buff);
			Socket_SendData(socket_param.m_Fd, buff, strlen(buff));
		}

		if (INVALID_SOCKET != socket_param.m_Fd)
		{
			Socket_Close(socket_param);
			socket_param.m_Fd = INVALID_SOCKET;
		}

		DelayMS(200);
	}
}

static void GetRemoteUserCmd(SocketParam SocketParam, uchar *pBuff, int len, RdsPack txPack)
{
	RdsPack rx_pack = {0};
	Data dat = {0};

	memset(pBuff, 0, len);

	//notify server send user cmd
	txPack.m_DataType = RDS_DATA_TYPE_GET_USER_CMD;
	Rds_JsonString(txPack, pBuff);
	Socket_SendData(SocketParam.m_Fd, pBuff, strlen(pBuff));

	//receive user cmd within 1s
	memset(pBuff, 0, RDS_BUFF_SIZE);

	if (0 == Socket_RecvData(SocketParam.m_Fd, pBuff, RDS_BUFF_SIZE, 800))
	{
		l_debug(NULL, "receive:%s\n", pBuff);

		memset(&rx_pack, 0, sizeof(RdsPack));

		Rds_JsonParse(&rx_pack, pBuff);

		dat.m_RdsData = rx_pack;

		//user cmd
		DQ_InsertData(dat, RDS_TYPE);
	}
}

static void	SendDevData(SocketParam socketParam, uchar *pBuff, int len, RdsPack txPack)
{
	Data dat = {0};

	memset(pBuff, 0, len);

	//get data
	dat = DQ_GetData(UDS_TYPE);

	//l_debug(NULL, "send:%d\n", dat.m_UdsData.m_Data.m_DataType);

#if 1
	if (RGP_NULL != dat.m_UdsData.m_Data.m_RGPType)
	{
		//RGPInfo to RdsPack
		memcpy(txPack.m_MiddleNo, "CS00000001", 10);
		txPack.m_Ports = (RDS_PORT_NO << (dat.m_UdsData.m_PortNo - 1));
		txPack.m_TermialSize = 1;
		txPack.m_TermialNo[0] = dat.m_UdsData.m_Data.m_Addr;
		txPack.m_DataType = dat.m_UdsData.m_Data.m_DataType;
		txPack.m_DataLen = dat.m_UdsData.m_Data.m_Content.m_DataInfo.m_Len;
		memcpy(txPack.m_Data, 
			dat.m_UdsData.m_Data.m_Content.m_DataInfo.m_Data, 
			txPack.m_DataLen);

		Rds_JsonString(txPack, pBuff);
		l_debug(NULL, "send:(%d)%s\n", (RDS_PORT_NO << (dat.m_UdsData.m_PortNo - 1)), pBuff);
		
		//send data
		Socket_SendData(socketParam.m_Fd, pBuff, strlen(pBuff));
	}
#endif	
}