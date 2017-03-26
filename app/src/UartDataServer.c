#include "string.h"
#include "stdlib.h"
#include "pthread.h"
#include "stdio.h"
#include "errno.h"
#include "UartDataServer.h"
#include "Common.h"
#include "RGPProtocol.h"
#include "RemoteDataServer.h"

static void 	DisposePost(int portNo, RGPPInfo rgpInfo);

/***********************************************************************
**Function Name	: Uds_DataDipose
**Description	: dispose uart data.
**Parameters	: portNo - port no.
				: pData - data.
				: len - data len.
**Return		: none.
***********************************************************************/
void Uds_DataDispose(int portNo, uchar *pData, int len)
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
				DisposePost(portNo, rgp_info);
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
**Function Name	: DisposePost
**Description	: dispose post method.
**Parameters	: portNo - in
				: rgpInfo - in.
**Return		: none.
***********************************************************************/
static void DisposePost(int portNo, RGPPInfo rgpInfo)
{
	uchar tbuff[1024] = {0};
	RdsPack tpack = {0};
	int i = 0;

	memcpy(tpack.m_MiddleNo, "YZ00000001", 10);

	tpack.m_PortNo = (RDS_PORT_NO << (portNo - 1));

	tpack.m_TermialSize = 1;
	tpack.m_TermialNo[0] = rgpInfo.m_Addr;
	tpack.m_DataType = rgpInfo.m_DataType;
	l_debug(NULL, "%s:%d",__FUNCTION__,tpack.m_DataType );
	tpack.m_DataLen = rgpInfo.m_Content.m_DataInfo.m_Len;
	for (i = 0; i < tpack.m_DataLen; ++i)
	{
		tpack.m_Data[i] = rgpInfo.m_Content.m_DataInfo.m_PData[i];
	}

	Rds_JsonString(tpack, tbuff);

	//set socket use status
	g_SocketUseStatus |= (SOCKET_INIT_STATUS << (portNo - 1));

	//connect server
	if (INVALID_SOCKET == g_SocketParam.m_Fd)
	{
		if (EBUSY != pthread_mutex_trylock(&g_CounterMutex))
		{
			g_SocketParam.m_Fd = Socket_ConnectServer(1, g_SocketParam);

			pthread_mutex_unlock(&g_CounterMutex);
		}
		else
		{
			TIME start_time = {0};

			GET_SYS_CURRENT_TIME(start_time);

			while (INVALID_SOCKET == g_SocketParam.m_Fd)
			{
				if (IS_TIMEOUT(start_time, 1000))
				{
					break;
				}

				DelayMS(10);
			}
		}
	}

	if (INVALID_SOCKET != g_SocketParam.m_Fd)
	{
		Socket_SendData(g_SocketParam.m_Fd, tbuff, strlen(tbuff));
	}

	//clear socket use status
	g_SocketUseStatus &= ~(SOCKET_INIT_STATUS << (portNo - 1));
}