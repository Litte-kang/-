#include "string.h"
#include "stdlib.h"
#include "pthread.h"
#include "stdio.h"
#include "errno.h"
#include "UartDataServer.h"
#include "Common.h"
#include "RGPProtocol.h"
#include "RemoteDataServer.h"
#include "DataQueue.h"
#include "UartApi.h"
#include "Config.h"

static void		UdsDataProcess(int portNo, uchar *pBuff, int len);
static void 	ProcessPostMethod(int portNo, RGPPInfo rgpInfo);
static void	 	UdsRecvThrd(void *pArg);	
static void 	UdsSendThrd(void *pArg);

/***********************************************************************
**Function Name	: Uds_Start
**Description	: start uart data server.
**Parameters	: ports - in 
				: bit 1 - port 1
				: bit 2 - port 2
				: bit 3 - port 3
				: bit 4 - port 4
				: set 1 use port, clear - not use port
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
int Uds_Start(uchar ports)
{
	pthread_t thread = {0};
	pthread_attr_t thread_attr = {0};
	int res = 0;
	int i = 0;

	//init attr
	res = pthread_attr_init(&thread_attr);

	if (0 != res)
	{
		l_debug(ERR_LOG_PATH, "%s: thread attr init failed", __FUNCTION__);
		return -1;
	}

	//set cpu againt scope(system level)
	res = pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);

	if (0 != res)
	{
		l_debug(ERR_LOG_PATH, "%s: set cpu againt scope failed", __FUNCTION__);
		return -1;		
	}

	//set thread detach
	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	if (0 != res)
	{
		l_debug(ERR_LOG_PATH, "%s: set thread detach failed", __FUNCTION__);
		return -1;		
	}

	//get uart configuration
	Uart_GetUartConfigInfo();

	//create thread
	for (i = 0; i < 4; ++i)
	{
		if (((UDS_PORT_NO << i) & ports))
		{
			//open uart
			Uart_Open((i + 1));

			res = pthread_create(&thread, &thread_attr, (void*)UdsRecvThrd, (i + 1));
			
			if (0 != res)
			{
				l_debug(ERR_LOG_PATH, "%s: create thread failed", __FUNCTION__);
				return -1;
			}
		}
	}

	res = pthread_create(&thread, &thread_attr, (void*)UdsSendThrd, NULL);

	if (0 != res)
	{
		l_debug(ERR_LOG_PATH, "%s: create thread failed", __FUNCTION__);
		return -1;
	}

	//destory thread_attr
	res = pthread_attr_destroy(&thread_attr);

	return 0;
}
/***********************************************************************
**Function Name	: UdsDataProcess
**Description	: process uart data.
**Parameters	: portNo - port no.
				: pBuff - in.
				: len - data len.
**Return		: none.
***********************************************************************/
void Uds_DataProcess(int portNo, uchar *pBuff, int len)
{
	UdsDataProcess(portNo, pBuff, len);
}

/***********************************************************************
**Function Name	: UdsRecvThrd
**Description	: receive uart data and process.
**Parameters	: pArg - in.
**Return		: none.
***********************************************************************/
static void	 UdsRecvThrd(void *pArg)
{
	uchar rx_buff[UART_RX_BUFFER_SIZE] = {0};
	int read_len = 0;
	int port = (int)pArg;

	l_debug(NULL, "%s: start %d uart data recv thread", __FUNCTION__, port);

	while (1)
	{
		read_len = Uart_RecvData(port, rx_buff, UART_RX_BUFFER_SIZE, 0);

		if (0 < read_len)
		{
			UdsDataProcess(port, rx_buff, read_len);
		}

		DelayMS(100);
	}
}

/***********************************************************************
**Function Name	: UdsSendThrd
**Description	: send uart data.
**Parameters	: pArg - in.
**Return		: none.
***********************************************************************/
static void UdsSendThrd(void *pArg)
{
	uchar tx_buff[UART_RX_BUFFER_SIZE] = {0};
	int write_len = 0;
	int i = 0;
	int j = 0;
	Data dat = {0};

	l_debug(NULL, "%s: start uart data send thread", __FUNCTION__);

	while (1)
	{
		dat = DQ_GetData(RDS_TYPE);

		if (strlen(dat.m_RdsData.m_MiddleNo))
		{
			//mul port
			for (i = 0; i < 4; ++i)
			{
				if (((UDS_PORT_NO << i) & dat.m_RdsData.m_Ports))
				{
					l_debug(NULL, "%s: port is %d, data type is %d, term is %d", 
						__FUNCTION__, 
						(i + 1), 
						dat.m_RdsData.m_DataType,
						dat.m_RdsData.m_TermialNo[0]);
					//mul termial
					/*
					for (j = 0; j < dat.m_RdsData.m_TermialSize; ++j)
					{
						RGP_Post((i + 1)), 
								dat.m_RdsData.m_DataType, 
								dat.m_RdsData.m_TermialNo[j], 
								dat.m_RdsData.m_Data, 
								dat.m_RdsData.m_DataLen);
					}
					//*/
				}
			}
		}

		DelayMS(100);
	}
}

/***********************************************************************
**Function Name	: UdsDataProcess
**Description	: process uart data.
**Parameters	: portNo - port no.
				: pBuff - in.
				: len - data len.
**Return		: none.
***********************************************************************/
static void UdsDataProcess(int portNo, uchar *pBuff, int len)
{
	RGPPInfo rgp_info = {RGP_NULL, 0, 0, 0};
	int i = 0;

	if (NULL == pBuff || 8 > len)
	{
		return;
	}

	while ((len - i) >= 0)
	{
		rgp_info = RGP_DataParse(&pBuff[i], len);
		//l_debug(NULL, "%s:port no %d,0x%.4x", __FUNCTION__, portNo,rgp_info.m_RGPType);
		switch (rgp_info.m_RGPType)
		{
			case RGP_GET:
				len = len - 11;
				i = i + 11;

				break;
			case RGP_POST:
				len = len - rgp_info.m_Content.m_DataInfo.m_Len - 9;
				i = i + rgp_info.m_Content.m_DataInfo.m_Len + 9;
				ProcessPostMethod(portNo, rgp_info);
				break;
			case RGP_RESPONSE:
				len = len - 8;
				i = i + 8;

				break;
			default:
				len--;
				i++;
				break;
		}
	}
	
}
/***********************************************************************
**Function Name	: ProcessPostMethod
**Description	: dispose post method.
**Parameters	: portNo - in
				: rgpInfo - in.
**Return		: none.
***********************************************************************/
static void ProcessPostMethod(int portNo, RGPPInfo rgpInfo)
{
	Data dat = {0};

	dat.m_UdsData.m_PortNo = portNo;
	dat.m_UdsData.m_Data = rgpInfo;

	//l_debug(NULL, "%s:%d",__FUNCTION__, dat.m_UdsData.m_Data.m_DataType );

	DQ_InsertData(dat, UDS_TYPE);
}