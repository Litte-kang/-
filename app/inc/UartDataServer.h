#ifndef _UART_DATA_SERVER_H_
#define _UART_DATA_SERVER_H_

#include "Type.h"

/***********************************************************************
**Function Name	: Uds_DataDipose
**Description	: dispose uart data.
**Parameters	: portNo - port no.
				: pData - data.
				: len - data len.
**Return		: none.
***********************************************************************/
extern void		Uds_DataDispose(int portNo, uchar *pData, int len);

#endif //_UART_DATA_SERVER_H_
