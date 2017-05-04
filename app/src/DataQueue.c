#include "DataQueue.h"

//-----------------uds data queue-----------------//
static Data g_UdsDataQueue[UDS_TYPE_QUEUE_LEN] = {0};
static uchar g_UdsDQWritePos = 0;
static uchar g_UdsDQReadPos = 0;
static uchar g_UdsDQCurCount = 0;

//-----------------rds data queue-----------------//
static Data g_RdsDataQueue[RDS_TYPE_QUEUE_LEN] = {0};
static uchar g_RdsDQWritePos = 0;
static uchar g_RdsDQReadPos = 0;
static uchar g_RdsDQCurCount = 0;

/***********************************************************************
**Function Name	: DQ_InsertData
**Description	: insert data to DataQueue.
**Parameters	: data - in.
				: type - in.
**Return		: 0 - ok , -1 - failed.
***********************************************************************/
int DQ_InsertData(Data data, DataType type)
{
	switch (type)
	{
		case RDS_TYPE:
			if (g_RdsDQCurCount < RDS_TYPE_QUEUE_LEN)
			{
				g_RdsDataQueue[g_RdsDQWritePos] = data;
				g_RdsDQWritePos++;
				g_RdsDQCurCount++;

				if (RDS_TYPE_QUEUE_LEN <= g_RdsDQWritePos)
				{
					g_RdsDQWritePos = 0;
				}

				return 0;
			}
			break;
		case UDS_TYPE:
			if (g_UdsDQCurCount < UDS_TYPE_QUEUE_LEN)
			{
				g_UdsDataQueue[g_UdsDQWritePos] = data;
				g_UdsDQWritePos++;
				g_UdsDQCurCount++;

				if (UDS_TYPE_QUEUE_LEN <= g_UdsDQWritePos)
				{
					g_UdsDQWritePos = 0;
				}

				return 0;
			}
			break;
	}

	return -1;
}
/***********************************************************************
**Function Name	: DQ_GetData
**Description	: get data from DataQueue.
**Parameters	: type - in.
**Return		: Data.
***********************************************************************/
Data DQ_GetData(DataType type)
{
	Data d = {0};

	switch (type)
	{
		case RDS_TYPE:
			if (0 < g_RdsDQCurCount)
			{
				d = g_RdsDataQueue[g_RdsDQReadPos];
				g_RdsDQReadPos++;
				g_RdsDQCurCount--;

				if (RDS_TYPE_QUEUE_LEN <= g_RdsDQReadPos)
				{
					g_RdsDQReadPos = 0;
				}
			}
			break;
		case UDS_TYPE:
			if (0 < g_UdsDQCurCount)
			{
				d = g_UdsDataQueue[g_UdsDQReadPos];
				g_UdsDQReadPos++;
				g_UdsDQCurCount--;

				if (UDS_TYPE_QUEUE_LEN <= g_UdsDQReadPos)
				{
					g_UdsDQReadPos = 0;
				}
			}
			break;
	}



	return d;
}
