#ifndef _REMOTE_DATA_SERVER_H_
#define _REMOTE_DATA_SERVER_H_

#include "Socket.h"
#include "pthread.h"
#include "Type.h"

//for RdsPack TermialNo
#define RDS_TERMIAL_SIZE_10	10
#define RDS_TERMIAL_SIZE_15	15
#define RDS_TERMIAL_SIZE_20	20
#define RDS_TERMIAL_SIZE 	RDS_TERMIAL_SIZE_10

//for RdsPack DataType
#define RDS_DATA_TYPE_X		0

//for RdsPack Data Buffer
#define RDS_DATA_SIZE_256		256
#define RDS_DATA_SIZE_512		512
#define RDS_DATA_SIZE_1024		1024
#define RDS_DATA_SIZE 			RDS_DATA_SIZE_256

//for RdsPack PortNO
#define RDS_PORT_NO 		0x01
#define RDS_PORT_NO_01		RDS_PORT_NO
#define RDS_PORT_NO_02		(RDS_PORT_NO << 1)
#define RDS_PORT_NO_03		(RDS_PORT_NO << 2)
#define RDS_PORT_NO_04		(RDS_PORT_NO << 3)
#define RDS_PORT_NO_EMPTY	0x00	

//for socket use status
#define SOCKET_INIT_STATUS		0x01
#define SOCKET_USE_STATUS1		SOCKET_INIT_STATUS
#define SOCKET_USE_STATUS2		(SOCKET_INIT_STATUS << 1)
#define SOCKET_USE_STATUS3		(SOCKET_INIT_STATUS << 2)
#define SOCKET_USE_STATUS4		(SOCKET_INIT_STATUS << 3)
#define SCOKET_NULL_STATUS		0x00

typedef struct _RdsPack
{
	uchar 	m_MiddleNo[11];						//middle no use 10byte,the last byte is 0
	uchar	m_PortNo;							//a port no use 1bit
	uchar 	m_TermialSize;						
	ushort 	m_TermialNo[RDS_TERMIAL_SIZE];
	uchar 	m_DataType;						
	ushort 	m_DataLen;
	uchar 	m_Data[RDS_DATA_SIZE];
}RdsPack;

/*
Description			: mutex
Default value		: PTHREAD_MUTEX_INITIALIZER.
The scope of value	: /.
First used			: /
*/
extern pthread_mutex_t g_CounterMutex;
/*
Description			: record socket use status(uart thread will use it)
Default value		: SCOKET_NULL_STATUS.
The scope of value	: /.
First used			: /
*/
extern uchar g_SocketUseStatus;
/*
Description			: scoket paramter
Default value		: {INVALID_SOCKET, 0, 0}.
The scope of value	: /.
First used			: /
*/
extern SocketParam g_SocketParam;

/***********************************************************************
**Function Name	: Rds_Start
**Description	: start remote data server.
**Parameters	: none.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern int		Rds_Start();
/***********************************************************************
**Function Name	: Rds_JsonParse
**Description	: parse a json str.
**Parameters	: pPack - save result.
				: pJsonStr - json string.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern int		Rds_JsonParse(RdsPack *pPack, const uchar *pJsonStr);
/***********************************************************************
**Function Name	: Rds_JsonString
**Description	: convert RdsPack to a json string.
**Parameters	: pack - in.
				: pJsonStr - out.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern int 		Rds_JsonString(RdsPack pPack, uchar *pJsonStr);

#endif //_REMOTE_DATA_SERVER_H_