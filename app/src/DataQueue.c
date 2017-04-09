#include "DataQueue.h"

//-----------------uds data queue-----------------//
static Data g_UdsDataQueue[20] = {0};
static uchar g_UdsDQWritePos = 0;
static uchar g_UdsDQReadPos = 0;
static uchar g_UdsDQCurCount = 0;

//-----------------rds data queue-----------------//
static Data g_RdsDataQueue[5] = {0};
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
			l_debug(NULL, "data type is %d", type);
			if (g_RdsDQCurCount < 5)
			{
				g_RdsDataQueue[g_RdsDQWritePos] = data;
				g_RdsDQWritePos++;
				g_RdsDQCurCount++;

				if (5 <= g_RdsDQWritePos)
				{
					g_RdsDQWritePos = 0;
				}

				return 0;
			}
			break;
		case UDS_TYPE:
			l_debug(NULL, "data type is %d", type);
			if (g_UdsDQCurCount < 20)
			{
				g_UdsDataQueue[g_UdsDQWritePos] = data;
				g_UdsDQWritePos++;
				g_UdsDQCurCount++;

				if (20 <= g_UdsDQWritePos)
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
			l_debug(NULL, "data type is %d", type);
			if (0 < g_RdsDQCurCount)
			{
				d = g_RdsDataQueue[g_RdsDQReadPos];
				g_RdsDQReadPos++;
				g_RdsDQCurCount--;

				if (5 <= g_RdsDQReadPos)
				{
					g_RdsDQReadPos = 0;
				}
			}
			break;
		case UDS_TYPE:
			l_debug(NULL, "data type is %d", type);
			if (0 < g_UdsDQCurCount)
			{
				d = g_UdsDataQueue[g_UdsDQReadPos];
				g_UdsDQReadPos++;
				g_UdsDQCurCount--;

				if (20 <= g_UdsDQReadPos)
				{
					g_UdsDQReadPos = 0;
				}
			}
			break;
	}

	return d;
}
