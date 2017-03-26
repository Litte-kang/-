#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "Type.h"

#define HOSTENT			struct hostent
#define SOCKADDR_IN		struct sockaddr_in
#define IN_ADDR			struct in_addr
#define SOCKADDR		struct sockaddr

#define INVALID_SOCKET	-1	

typedef struct _SocketParam
{
	int 	m_Fd;
	int 	m_Port;
	uchar 	m_Ip[17];	//ip use 16byte the last byte is 0
}SocketParam;

/***********************************************************************
**Function Name	: Socket_GetNetConifgInfo
**Description	: get net paramter from sys.config.
**Parameters	: pParam - save net paramter.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
extern int		Socket_GetNetConifgInfo(SocketParam *pParam);
/***********************************************************************
**Function Name	: Socket_ConnectServer
**Description	: connect server.
**Parameters	: times - connect times.
				: param - in.
**Return		: -1 - failed, socket - ok.
***********************************************************************/
extern int 		Socket_ConnectServer(int times, SocketParam param);
/***********************************************************************
**Function Name	: Socket_RecvData
**Description	: recieve data from server.
**Parameters	: fd - in.
				: pBuff - store data recieved.
				: len - expectations.
				: timeout - wait time (0:unlimit time).
**Return		: 0 - ok -1 - failed .
***********************************************************************/
extern int 		Socket_RecvData(int fd, uchar *pBuff, int len, int timeout);
/***********************************************************************
**Function Name	: Socket_SendData
**Description	: send data to server.
**Parameters	: fd.
				: pBuff - store data recieved.
				: len - expectations.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
extern int 		Socket_SendData(int fd, const uchar *pBuff, int len);
/***********************************************************************
**Function Name	: Socket_Close
**Description	: close client to free source.
**Parameters	: param - in.
**Return		: none.
***********************************************************************/
extern void 	Socket_Close(SocketParam param);

#endif //_SOCKET_H_