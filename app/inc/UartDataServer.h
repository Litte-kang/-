#ifndef _UART_DATA_SERVER_H_
#define _UART_DATA_SERVER_H_

#include "Type.h"
#include "RGPProtocol.h"

//for RdsPack Port bit
#define UDS_PORT_NO 		0x01
#define UDS_PORT_NO_01		UDS_PORT_NO
#define UDS_PORT_NO_02		(UDS_PORT_NO << 1)
#define UDS_PORT_NO_03		(UDS_PORT_NO << 2)
#define UDS_PORT_NO_04		(UDS_PORT_NO << 3)
#define UDS_PORT_NO_EMPTY	0x00

typedef struct _UdsPack
{
	uchar		m_PortNo;
	RGPPInfo 	m_Data;
}UdsPack;

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
extern int		Uds_Start(uchar ports);

/***********************************************************************
**Function Name	: UdsDataProcess
**Description	: process uart data.
**Parameters	: portNo - port no.
				: pBuff - in.
				: len - data len.
**Return		: none.
***********************************************************************/
extern void 	Uds_DataProcess(int portNo, uchar *pBuff, int len);

#endif //_UART_DATA_SERVER_H_
