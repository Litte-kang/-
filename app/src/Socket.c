#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include "Socket.h"
#include "Common.h"
#include "Config.h"
#include "json.h"

static void CatchSig(int SigNum);

//---------------------------end-----------------------//

/***********************************************************************
**Function Name	: CacthSig
**Description	: this is callback,when a specified signal come, call it.
**Parameters	: SigNum - signal type.
**Return		: none.
***********************************************************************/
static void CatchSig(int SigNum)
{
	switch (SigNum)
	{
		case SIGPIPE:
			l_debug(ERR_LOG_PATH, "%s:catch SIGPIPE!\n", __FUNCTION__);
			break;
		default:
			break;
	}
}

/***********************************************************************
**Function Name	: Socket_GetNetConifgInfo
**Description	: get net paramter from sys.config.
**Parameters	: pParam - save net paramter.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
int Socket_GetNetConifgInfo(SocketParam *pParam)
{
	uchar sys_config_info[SYS_CONFIG_BUFF_SIZE] = {0};
	struct json_object *p_sys_conf = NULL;
	struct json_object *p_obj = NULL;
	struct json_object *p_param = NULL;
	FILE *fp = NULL;

	fp = fopen(SYS_CONFIG_PATH, "r");

	if (NULL == fp)
	{
		l_debug(ERR_LOG_PATH, "open %s error\n", SYS_CONFIG_PATH);
		return -1;
	}

	fscanf(fp, "%s", sys_config_info);
	fclose(fp);

	memset(pParam, 0, sizeof(SocketParam));

	p_sys_conf = json_tokener_parse(sys_config_info);
	p_obj = json_object_array_get_idx(p_sys_conf, SYS_CONFIG_SERVER_INFO);
	p_param = json_object_object_get(p_obj, "serverIp");

	memset(pParam->m_Ip, 0, 16);
	memcpy(pParam->m_Ip, json_object_get_string(p_param), strlen(json_object_get_string(p_param)));

	p_param = json_object_object_get(p_obj, "serverPort");

	pParam->m_Port = json_object_get_int(p_param);

	///*
	printf("%s:%d\n",pParam->m_Ip,pParam->m_Port);
	//*/

	json_object_put(p_sys_conf);

	return 0;
}
/***********************************************************************
**Function Name	: Socket_ConnectServer
**Description	: connect server.
**Parameters	: times - connect times.
				: param - in.
**Return		: INVALID_SOCKET - failed, socket - ok.
***********************************************************************/
int Socket_ConnectServer(int times, SocketParam param)
{
	SOCKADDR_IN serv_addr = {0};
	int tmp = 0;
	int socket_fd = -1;
	struct sigaction action;
	struct sigaction sa;

	if (0 >= times)
	{		
		l_debug(ERR_LOG_PATH, "%s:param error\n", __FUNCTION__);
		return -1;
	}
	
	//create socket	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (-1 == socket_fd)
	{
		l_debug(ERR_LOG_PATH, "%s:create socket failed!\n",__FUNCTION__);
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(param.m_Port);
	serv_addr.sin_addr.s_addr = inet_addr(param.m_Ip);

	bzero(&(serv_addr.sin_zero), 8);
	
	do
	{
		tmp = connect(socket_fd, (SOCKADDR*)&serv_addr, sizeof(SOCKADDR));
		
		if (0 == tmp)
		{			
			L_DEBUG("connect %s:%d sucessful!\n", param.m_Ip, param.m_Port);
		
			sa.sa_handler = SIG_IGN;
			action.sa_handler = CatchSig;

			sigemptyset(&action.sa_mask);

			action.sa_flags = 0;
			sigaction(SIGPIPE, &sa, 0);
			sigaction(SIGPIPE, &action, 0);

			return socket_fd;
		}
		
		times--;

		if (0 == times)
		{
			break;
		}

		sleep(1);
		continue;
	}while(0);
	
	close(socket_fd);
	
	return INVALID_SOCKET;	
}
/***********************************************************************
**Function Name	: Socket_RecvData
**Description	: recieve data from server.
**Parameters	: fd - in.
				: pBuff - store data recieved.
				: len - expectations.
				: timeout - wait time (0:unlimit time).
**Return		: 0 - ok -1 - failed .
***********************************************************************/
int Socket_RecvData(int fd, uchar *pBuff, int len, int timeout)
{
	int rec_len = 0;
	int max_fd = 0;
	fd_set inset;
	struct timeval tv;

	if (NULL == pBuff)
	{
		l_debug(ERR_LOG_PATH, "%s:pBuff is null!\n",__FUNCTION__);
		return -1;
	}

	if (0 > fd)
	{
		l_debug(ERR_LOG_PATH, "%s:socket fd error!\n",__FUNCTION__);
		return -3;
	}
	
	FD_ZERO(&inset);
	FD_SET(fd, &inset);	
	max_fd = fd + 1;
	
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	
	//--- wait data from server ---//
	select(max_fd,  &inset, NULL, NULL, (timeout ? &tv : NULL));	

	if (!FD_ISSET(fd, &inset))
	{
		l_debug(ERR_LOG_PATH, "%s:recv error!\n",__FUNCTION__);
		return -1;
	}

	FD_CLR(fd, &inset);	
	
	memset(pBuff, 0, sizeof(pBuff));

	rec_len = recv(fd, pBuff, len, 0);	
	if (-1 == rec_len)
	{
		l_debug(ERR_LOG_PATH, "%s:recieve data from server failed!\n",__FUNCTION__);
		return -1;
	}
	else if (0 == rec_len)
	{
		l_debug(ERR_LOG_PATH, "%s:connection break!\n",__FUNCTION__);
		return -1;
	}	

	return 0;	
}

/***********************************************************************
**Function Name	: Socket_SendData
**Description	: send data to server.
**Parameters	: fd.
				: pBuff - store data recieved.
				: len - expectations.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
int Socket_SendData(int fd, const uchar *pBuff, int len)
{
	int send_len = 0;	

	if (NULL == pBuff)
	{
		l_debug(ERR_LOG_PATH, "%s:pBuff is null!\n",__FUNCTION__);
		return -1;
	}

	if (0 <= fd)
	{
		send_len = send(fd, pBuff, len, 0);
		
		if (-1 == send_len || send_len != len)
		{
			l_debug(ERR_LOG_PATH, "%s:send data to server failed!\n",__FUNCTION__);
			return -1;
		}		
	}
	else
	{
		l_debug(ERR_LOG_PATH, "%s:socket fd error!\n",__FUNCTION__);
		return -1;
	}

	return 0;
}

/***********************************************************************
**Function Name	: Socket_Close
**Description	: close client to free source.
**Parameters	: param - in.
**Return		: none.
***********************************************************************/
void  Socket_Close(SocketParam param)
{
	if (0 <= param.m_Fd)
	{
		close(param.m_Fd);	
	}
	else
	{
		l_debug(ERR_LOG_PATH, "%s:logout client(%d) %s:%d failed!\n", 
			__FUNCTION__, param.m_Fd, param.m_Ip, param.m_Port);	
	}
}