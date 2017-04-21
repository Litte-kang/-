#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"
#include "json.h"
#include "Common.h"
#include "Config.h"
#include "UartApi.h"
#include "Socket.h"
#include "RemoteDataServer.h"
#include "Type.h"

static void AppInit();
static void InitTestThread();
static void TestThrd01(void *pArg);

void main()
{

	AppInit();

	while(1) sleep(1);
}

static void TestThrd01(void *pArg)
{
	int thread_no = (int)pArg;

	uchar rgp1[12] = {0xff,0xfe,0x01,0x01,0x00,0x02,0x00,0x01,0x02};
	uchar rgp2[12] = {0xff,0xfe,0x02,0x01,0x00,0x02,0x00,0x03,0x04};
	uchar rgp3[12] = {0xff,0xfe,0x03,0x01,0x00,0x02,0x00,0x05,0x06};
	uchar rgp4[12] = {0xff,0xfe,0x04,0x01,0x00,0x02,0x00,0x07,0x08};
	ushort crc_code = 0;

	crc_code = CheckCode(rgp1, 9);

	rgp1[9] = (uchar)crc_code;
	rgp1[10] = (uchar)(crc_code >> 8);

	crc_code = CheckCode(rgp2, 9);

	rgp2[9] = (uchar)crc_code;
	rgp2[10] = (uchar)(crc_code >> 8);

	crc_code = CheckCode(rgp3, 9);

	rgp3[9] = (uchar)crc_code;
	rgp3[10] = (uchar)(crc_code >> 8);

	crc_code = CheckCode(rgp4, 9);

	rgp4[9] = (uchar)crc_code;
	rgp4[10] = (uchar)(crc_code >> 8);

	while (1)
	{	
		switch (thread_no)
		{
			case 1:
				Uds_DataDispose(1, rgp1, 11);
				DelayMS(1000);
				break;
			case 2:
				Uds_DataDispose(2, rgp2, 11);
				DelayMS(800);
				break;
			case 3:
				Uds_DataDispose(3, rgp3, 11);
				DelayMS(600);
				break;
			case 4:
				Uds_DataDispose(4, rgp4, 11);
				DelayMS(1200);
				break;
		}
		DelayMS(10);
	}
}

static void InitTestThread()
{
	pthread_t thread = {0};
	pthread_attr_t thread_attr = {0};
	int res = 0;

	res = pthread_attr_init(&thread_attr);
	if (0 != res)
	{
		l_debug(NULL, "init attr failed");
		return;
	}

	res = pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
	if (0 != res)
	{
		l_debug(NULL, "set scope failed");
		return;		
	}

	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if (0 != res)
	{
		l_debug(NULL, "set detachstate failed");
		return;						
	}

	res = pthread_create(&thread, &thread_attr, (void*)TestThrd01, 1);
	if (0 != res)
	{
		l_debug(NULL, "create thread failed");
		return;			
	}

	res = pthread_create(&thread, &thread_attr, (void*)TestThrd01, 2);
	if (0 != res)
	{
		l_debug(NULL, "create thread failed");
		return;			
	}

	res = pthread_create(&thread, &thread_attr, (void*)TestThrd01, 3);
	if (0 != res)
	{
		l_debug(NULL, "create thread failed");
		return;			
	}

	res = pthread_create(&thread, &thread_attr, (void*)TestThrd01, 4);
	if (0 != res)
	{
		l_debug(NULL, "create thread failed");
		return;			
	}

	pthread_attr_destroy(&thread_attr);

}

static void AppInit()
{
	///*
	if (0 == Rds_Start())
	{
		InitTestThread();
	}
	else
	{
		printf("get net config failure\n");
	}
 	//*/
	/*
	//get uart information (baud rate, data bit, parity, stop bit )
	Uart_GetUartConfigInfo();

	//open uart	
	Uart_Open(UART_PORT1);
	Uart_Open(UART_PORT2);

	printf("uart fd:%d,%d\n", g_UartDevInfo[0].m_Fd,g_UartDevInfo[1].m_Fd);
	
	//start uart recieve thread
	Uart_StartRecvThrd(UART_PORT1);
	Uart_StartRecvThrd(UART_PORT2);

	sleep(2);

	{
		uchar rgp1[12] = {0xff,0xfe,0x01,0x01,0x00,0x02,0x00,0x01,0x02};
		uchar rgp2[12] = {0xff,0xfe,0x02,0x01,0x00,0x02,0x00,0x03,0x04};
		ushort crc_code = 0;

		crc_code = CheckCode(rgp1, 9);

		rgp1[9] = (uchar)crc_code;
		rgp1[10] = (uchar)(crc_code >> 8);

		crc_code = CheckCode(rgp2, 9);

		rgp2[9] = (uchar)crc_code;
		rgp2[10] = (uchar)(crc_code >> 8);

		while(1)
		{		
			l_debug(NULL, "send data");	
			Uart_SendData(UART_PORT1, rgp1, 11);
			//sleep(1);
			Uart_SendData(UART_PORT2, rgp2, 11);
			//printf("send hello to uart 1\n");
			sleep(1);
		}
	}
	//*/	
}