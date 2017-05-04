#ifndef _UART_DATA_SERVER_H_
#define _UART_DATA_SERVER_H_

#include "Type.h"
#include "RGPProtocol.h"

typedef struct _UdsPack
{
	uchar		m_PortNo;
	RGPPInfo 	m_Data;
}UdsPack;

/***********************************************************************
**Function Name	: Uds_DataProcess
**Description	: dispose uart data.
**Parameters	: portNo - port no.
				: pData - data.
				: len - data len.
**Return		: none.
***********************************************************************/
extern void		Uds_DataProcess(int portNo, uchar *pData, int len);


#endif //_UART_DATA_SERVER_H_
