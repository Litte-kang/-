/* uart_api.h */
#ifndef		UART_API_H
#define		UART_API_H

#include "Type.h"

#define		GNR_COM			0
#define		USB_COM			1
#define 	COM_TYPE		USB_COM

#define		UART_PORT1	1
#define		UART_PORT2	2
#define		UART_PORT3	3
#define		UART_PORT4	4

//host pc test
#define     HOST_COM_PORT_SIZE	4

//target pc
#define		TARGET_COM_PORT_SIZE	4

#if (COM_TYPE == GNR_COM)
#define 	MAX_COM_NUM		4
#define		USER_COM_SIZE	TARGET_COM_PORT_SIZE
#else
#define     MAX_COM_NUM     3
#define		USER_COM_SIZE	HOST_COM_PORT_SIZE
#endif

#define 	BUFFER_SIZE		1024*2
#define		TIME_DELAY		180
#define		SEL_FILE_NUM		2
#define		RECV_FILE_NAME		"recv.dat"

typedef struct _UartDevInfo
{	
	int 	m_Fd;
	int 	m_BaudRate;	
	int 	m_DataBit;
	uchar 	m_Parity;
	int 	m_StopBit;
	uchar	m_Dev[20];	//dev use 19byte the last is 0
}UartDevInfo;

/*
Description			: uart dev information (max device is 4)
Default value		: 0.
The scope of value	: /.
First used			: /
*/
extern UartDevInfo 	g_UartDevInfo[USER_COM_SIZE];

/***********************************************************************
**Function Name	: Uart_GetUartConfigInfo
**Description	: get device information from sys.config.
**Parameters	: none.
**Return		: 0 - ok -1 - failure.
***********************************************************************/
extern int 		Uart_GetUartConfigInfo();
/***********************************************************************
**Function Name	: Uart_Open
**Description	: open uart device.
**Parameters	: portNo - in.
**Return		: 0 - ok -1 - failure.
***********************************************************************/
extern int 		Uart_Open(int portNo);
/***********************************************************************
**Function Name	: Uart_SendData
**Description	: send data to uart.
**Parameters	: portNo - port no.
				: pData - data.
				: len - data len.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
extern int 		Uart_SendData(int portNo, const uchar *pData, int len);
/***********************************************************************
**Function Name	: Uart_Close
**Description	: close uart opened.
**Parameters	: portNo - in.
**Return		: none.
***********************************************************************/
extern void		Uart_Close(int portNo); 	
/***********************************************************************
**Function Name	: Uart_StartRecvThrd
**Description	: create a thread to recieve uart data.
**Parameters	: portNo - port no.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern int	 	Uart_StartRecvThrd(int portNo);	

#endif /* UART_API_H */
