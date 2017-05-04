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

static void 	ProcessPostMethod(int portNo, RGPPInfo rgpInfo);

/***********************************************************************
**Function Name	: Uds_DataProcess
**Description	: process uart data.
**Parameters	: portNo - port no.
				: pData - data.
				: len - data len.
**Return		: none.
***********************************************************************/
void Uds_DataProcess(int portNo, uchar *pData, int len)
{
	RGPPInfo rgp_info = {RGP_NULL, 0, 0, 0};
	int i = 0;

	if (NULL == pData || 8 > len)
	{
		return;
	}

	while ((len - i) >= 0)
	{
		rgp_info = RGP_DataParse(&pData[i], len);
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