#ifndef _RGP_PROTOCOL_H_
#define _RGP_PROTOCOL_H_

#include "Type.h"

#define RGP_GET			0xfdff	//get method: get data from other dev
#define RGP_POST 		0xfeff	//post method: send data to other dev
#define RGP_RESPONSE 	0xffff	//response method: response get or post method
#define RGP_NULL		0x0000	//null method

#define RGP_BUFF_SIZE	256

typedef struct _RGPPInfo
{
	ushort 	m_RGPType;
	uchar 	m_DataType;
	ushort 	m_Addr;
	union
	{
		struct
		{
			int		m_Len;
			uchar 	m_Data[RGP_BUFF_SIZE];			
		}m_DataInfo;
		uchar m_Status;
	}m_Content;

}RGPPInfo;

/***********************************************************************
**Function Name	: Rgp_Get
**Description	: get data.
**Parameters	: portNo - port no.
				: type - data type.
				: addr - termial addr.
				: pData - data.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
extern int 			RGP_Get(int portNo, uchar type, ushort addr, uchar *pData);
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
extern int			RGP_Post(int portNo, uchar type, ushort addr, uchar *pData, int len);
/***********************************************************************
**Function Name	: RGP_Response
**Description	: response.
**Parameters	: portNo - port no.
				: type - data type.
				: addr - termial addr.
				: status - in.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
extern int			RGP_Response(int portNo, uchar type, ushort addr, uchar status);
/***********************************************************************
**Function Name	: RGP_DataParse
**Description	: parse rgp data.
**Parameters	: pData - rgp data.
				: len - data length.
**Return		: RGPPInfo.
***********************************************************************/
extern RGPPInfo		RGP_DataParse(uchar *pData, int len);

#endif //_RGP_PROTOCOL_H_