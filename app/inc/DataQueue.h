#ifndef _DATA_QUEUE_H_
#define _DATA_QUEUE_H_

#include "RGPProtocol.h"
#include "RemoteDataServer.h"
#include "Type.h"

typedef enum _DataType
{
	INVALID_TYPE = -1,
	RDS_TYPE = 0,
	UDS_TYPE = INVALID_TYPE
}DataType;

typedef union _Data
{
	RdsPack 	m_RdsData;
#if INVALID_TYPE != UDS_TYPE
	RGPPInfo	m_RGPData;
#endif
}Data;

/***********************************************************************
**Function Name	: DQ_InsertData
**Description	: insert data to DataQueue.
**Parameters	: data - in.
				: type - in.
**Return		: 0 - ok , -1 - failed.
***********************************************************************/
extern int 			DQ_InsertData(Data data, DataType type);
/***********************************************************************
**Function Name	: DQ_GetData
**Description	: get data from DataQueue.
**Parameters	: type - in.
**Return		: Data.
***********************************************************************/
extern Data			DQ_GetData(DataType type);

#endif //_DATA_QUEUE_H_

