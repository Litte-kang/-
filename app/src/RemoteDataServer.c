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
#include "sqlite3.h"

/*
Description			: mutex
Default value		: PTHREAD_MUTEX_INITIALIZER.
The scope of value	: /.
First used			: /
*/
pthread_mutex_t g_CounterMutex = PTHREAD_MUTEX_INITIALIZER;
/*
Description			: record socket use status(uart thread will use it)
Default value		: SCOKET_NULL_STATUS.
The scope of value	: /.
First used			: /
*/
uchar g_SocketUseStatus = SCOKET_NULL_STATUS;
/*
Description			: scoket paramter
Default value		: {INVALID_SOCKET, 0, 0}.
The scope of value	: /.
First used			: /
*/
SocketParam g_SocketParam = {INVALID_SOCKET, 0, 0};

static void		RdsThrd(void *pArg);

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

	Socket_GetNetConifgInfo(&g_SocketParam);

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
	pPack->m_PortNo = RDS_PORT_NO_EMPTY;

	for (i = 0; i < array_len; ++i)
	{
		p_ele = json_object_array_get_idx(p_obj, i);

		pPack->m_PortNo |= (RDS_PORT_NO_01 << (json_object_get_int(p_ele) - 1));
		l_debug(NULL, "portNo:%d", pPack->m_PortNo);
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

	if (RDS_PORT_NO_EMPTY != pack.m_PortNo)
	{
		if ((pack.m_PortNo & RDS_PORT_NO_01))
		{
			json_object_array_add(p_array, json_object_new_int(1));
		}

		if ((pack.m_PortNo & RDS_PORT_NO_02))
		{
			json_object_array_add(p_array, json_object_new_int(2));
		}

		if ((pack.m_PortNo & RDS_PORT_NO_03))
		{
			json_object_array_add(p_array, json_object_new_int(3));
		}

		if ((pack.m_PortNo & RDS_PORT_NO_04))
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
	RdsPack rpack = {0};
	uchar rbuff[1024] = {0};
	int i = 0;

	l_debug(RUN_LOG_PATH, "-------start remote data station thread-------\n");

	while (1)
	{
		//connect server
		if (INVALID_SOCKET == g_SocketParam.m_Fd)
		{
			if (EBUSY != pthread_mutex_trylock(&g_CounterMutex))
			{
				g_SocketParam.m_Fd = Socket_ConnectServer(3, g_SocketParam);

				pthread_mutex_unlock(&g_CounterMutex);
			}
		}

		if (INVALID_SOCKET != g_SocketParam.m_Fd)
		{
			if (0 == Socket_RecvData(g_SocketParam.m_Fd, rbuff, 1024, 1))
			{
				l_debug(NULL, "%s\n", rbuff);

				memset(&rpack, 0, sizeof(RdsPack));

				Rds_JsonParse(&rpack, rbuff);
			}
		}

		if (SCOKET_NULL_STATUS == g_SocketUseStatus
			&& INVALID_SOCKET != g_SocketParam.m_Fd)
		{
			Socket_Close(g_SocketParam);
			g_SocketParam.m_Fd = INVALID_SOCKET;
		}

		DelayMS(50);
	}
}