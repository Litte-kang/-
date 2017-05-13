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
#define RDS_DATA_TYPE_GET_USER_CMD			0
#define RDS_DATA_TYPE_CLOSE_CONN			1
#define RDS_DATA_TYPE_INVALID_USER_CMD		2

//for RdsPack Data Buffer size
#define RDS_DATA_SIZE_128		128
#define RDS_DATA_SIZE_256		256
#define RDS_DATA_SIZE_512		512
#define RDS_DATA_SIZE 			RDS_DATA_SIZE_128

//for RdsPack Port bit
#define RDS_PORT_NO 		0x01
#define RDS_PORT_NO_01		RDS_PORT_NO
#define RDS_PORT_NO_02		(RDS_PORT_NO << 1)
#define RDS_PORT_NO_03		(RDS_PORT_NO << 2)
#define RDS_PORT_NO_04		(RDS_PORT_NO << 3)
#define RDS_PORT_NO_EMPTY	0x00	

#define RDS_BUFF_SIZE	1024

typedef struct _RdsPack
{
	uchar 	m_MiddleNo[11];						//middle id use 10byte,the last byte is 0
	uchar	m_Ports;							//a port map 1bit(port 1 - 1th bit, port 2 - 2th bit, port 3 - 3th bit, port 4 - 4th bit)
	uchar 	m_TermialSize;						
	ushort 	m_TermialNo[RDS_TERMIAL_SIZE];
	uchar 	m_DataType;						
	ushort 	m_DataLen;
	uchar 	m_Data[RDS_DATA_SIZE];
}RdsPack;

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