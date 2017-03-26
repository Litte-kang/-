#include "string.h"
#include "stdlib.h"
#include "RGPProtocol.h"
#include "Common.h"
#include "UartApi.h"
#include "Type.h"

static int GetParse(uchar *pData, RGPPInfo *pInfo);
static int PostParse(uchar *pData, RGPPInfo *pInfo);
static int ResponseParse(uchar *pData, RGPPInfo *pInfo);

/***********************************************************************
**Function Name	: Rgp_Get
**Description	: get data.
**Parameters	: portNo - port no.
				: type - data type.
				: addr - termial addr.
				: pData - data.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
int RGP_Get(int portNo, uchar type, ushort addr, uchar *pData)
{
	uchar protocol_dat[11] = {0};
	ushort crc_code = 0;

	//head
	protocol_dat[0] = (uchar)RGP_GET;
	protocol_dat[1] = (uchar)(RGP_GET >> 8);
	//type
	protocol_dat[2] = type;
	//address
	protocol_dat[3] = (uchar)addr;
	protocol_dat[4] = (uchar)(addr >> 8);
	//data
	protocol_dat[5] = pData[1];
	protocol_dat[6] = pData[2];
	protocol_dat[7] = pData[3];
	protocol_dat[8] = pData[4];
	//crc code
	crc_code = CheckCode(protocol_dat, 9);

	protocol_dat[9] = (uchar)crc_code;
	protocol_dat[10] = (uchar)(crc_code >> 8);

	return Uart_SendData(portNo, protocol_dat, 11);
}
/***********************************************************************
**Function Name	: RGP_Post
**Description	: send data.
**Parameters	: portNo - port no.
				: type - data type.
				: addr - termial addr.
				: pData - data.
				: len - data len.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
int	RGP_Post(int portNo, uchar type, ushort addr, uchar *pData, int len)
{
	uchar *p_protocol_dat = NULL;
	int pos = 0;
	ushort crc_code = 0;

	p_protocol_dat = (uchar*)malloc((len + 9)*sizeof(uchar));

	if (NULL == p_protocol_dat)
	{
		return -1;
	}

	//head
	p_protocol_dat[0] = (uchar)RGP_POST;
	p_protocol_dat[1] = (uchar)(RGP_POST >> 8);
	//type
	p_protocol_dat[2] = type;
	//address
	p_protocol_dat[3] = (uchar)addr;
	p_protocol_dat[4] = (uchar)(addr >> 8);
	//data len
	p_protocol_dat[5] = (uchar)len;
	p_protocol_dat[6] = (uchar)(len >> 8);

	//data
	pos = 7;

	while (len)
	{
		p_protocol_dat[pos] = *pData++;

		pos++;
		len--;
	}

	crc_code = CheckCode(p_protocol_dat, pos);

	//crc code
	p_protocol_dat[pos++] = (uchar)crc_code;
	p_protocol_dat[pos++] = (uchar)(crc_code >> 8);

	Uart_SendData(portNo, p_protocol_dat, pos);

	free(p_protocol_dat);

	return 0;
}
/***********************************************************************
**Function Name	: RGP_Response
**Description	: response.
**Parameters	: portNo - port no.
				: type - data type.
				: addr - termial addr.
				: status - in.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
int	RGP_Response(int portNo, uchar type, ushort addr, uchar status)
{
	uchar protocol_dat[8] = {0};
	ushort crc_code = 0;

	//head
	protocol_dat[0] = (uchar)RGP_RESPONSE;
	protocol_dat[1] = (uchar)(RGP_RESPONSE >> 8);
	//type
	protocol_dat[2] = type;
	//address
	protocol_dat[3] = (uchar)addr;
	protocol_dat[4] = (uchar)(addr >> 8);
	//status
	protocol_dat[5] = status;
	//crc code
	crc_code = CheckCode(protocol_dat, 6);

	protocol_dat[6] = (uchar)crc_code;
	protocol_dat[7] = (uchar)(crc_code >> 8);

	return Uart_SendData(portNo, protocol_dat, 8);	
}
/***********************************************************************
**Function Name	: RGP_DataParse
**Description	: parse rgp data.
**Parameters	: pData - rgp data.
				: len - data length.
**Return		: RGPPInfo.
***********************************************************************/
RGPPInfo RGP_DataParse(uchar *pData, int len)
{
	RGPPInfo rgp_info = {RGP_NULL, 0, 0, 0};
	ushort rgp_type = RGP_NULL;
	int i = 0;

	if (NULL == pData || 8 > len)
	{
		return rgp_info;
	}

	while (len)
	{
		rgp_type = (ushort)pData[i];
		rgp_type |= (ushort)(pData[i + 1] << 8);

		switch (rgp_type)
		{
			case RGP_GET:
				if (0 == GetParse(&pData[i], &rgp_info))
				{
					return rgp_info;
				}
				break;
			case RGP_POST:
				if (0 == PostParse(&pData[i], &rgp_info))
				{
					return rgp_info;
				}
				break;
			case RGP_RESPONSE:
				if (0 == ResponseParse(&pData[i], &rgp_info))
				{
					return rgp_info;
				}
				break;
		}

		len--;
		i++;
	}

	return rgp_info;
}
/***********************************************************************
**Function Name	: GetParse
**Description	: parse get method.
**Parameters	: pData - rgp data.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
static int GetParse(uchar *pData, RGPPInfo *pInfo)
{
	ushort crc_code = 0;

	crc_code = (ushort)pData[9];
	crc_code |= (ushort)(pData[10] << 8);

	if (crc_code == CheckCode(pData, 9))
	{
		pInfo->m_RGPType = RGP_GET;
		pInfo->m_DataType = pData[2];
		pInfo->m_Addr = (ushort)pData[3];
		pInfo->m_Addr |= (ushort)(pData[4] << 8);
		pInfo->m_Content.m_DataInfo.m_Len = 4;
		pInfo->m_Content.m_DataInfo.m_PData = &pData[5];

		return 0;
	} 

	return -1;
}
/***********************************************************************
**Function Name	: PostParse
**Description	: parse post method.
**Parameters	: pData - rgp data.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
static int PostParse(uchar *pData, RGPPInfo *pInfo)
{
	ushort crc_code = 0;
	int len = 0;

	len = (int)pData[5];
	len |= (int)(pData[6] << 8); 

	crc_code = (ushort)pData[7 + len];
	crc_code |= (ushort)(pData[8 + len] << 8);

	if (crc_code == CheckCode(pData, (7 + len)))
	{
		pInfo->m_RGPType = RGP_POST;
		pInfo->m_DataType = pData[2];
		pInfo->m_Addr = (ushort)pData[3];
		pInfo->m_Addr |= (ushort)(pData[4] << 8);
		pInfo->m_Content.m_DataInfo.m_Len = len;
		pInfo->m_Content.m_DataInfo.m_PData = &pData[7];
		
		return 0;
	} 

	return -1;
}
/***********************************************************************
**Function Name	: ResponseParse
**Description	: parse response.
**Parameters	: pData - rgp data.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
static int ResponseParse(uchar *pData, RGPPInfo *pInfo)
{
	ushort crc_code = 0;

	crc_code = (ushort)pData[6];
	crc_code |= (ushort)(pData[7] << 8);

	if (crc_code == CheckCode(pData, 6))
	{
		pInfo->m_RGPType = RGP_GET;
		pInfo->m_DataType = pData[2];
		pInfo->m_Addr = (ushort)pData[3];
		pInfo->m_Addr |= (ushort)(pData[4] << 8);
		pInfo->m_Content.m_Status = pData[5];
		
		return 0;
	} 

	return -1;
}