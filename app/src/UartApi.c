
/* uart_api.c */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "UartApi.h"
#include "json.h"
#include "Config.h"
#include "Common.h"
#include "UartDataServer.h"
#include "Type.h"

/*
Description			: uart dev information (max device is 4)
Default value		: 0.
The scope of value	: /.
First used			: /
*/
UartDevInfo g_UartDevInfo[USER_COM_SIZE] = {-1, 0, 0, 0, 0, 0};

static int 		SetUart(int fd, int baud_rate, int data_bits, uchar parity, int stop_bits);
static void 	RecieveUartDataThrd(void *pArg);

/***********************************************************************
**Function Name	: Uart_GetUartConfigInfo
**Description	: get device information from file.
**Parameters	: none.
**Return		: 0 - ok -1 - failure.
***********************************************************************/
int Uart_GetUartConfigInfo()
{
#if 1	
	uchar sys_config_info[SYS_CONFIG_BUFF_SIZE] = {0};
	struct json_object *p_sys_conf = NULL;
	struct json_object *p_obj = NULL;
	struct json_object *p_port_dev = NULL;
	FILE *fp = NULL;
	int dev_sum = 0;	//device sum;
	int i = 0;

	fp = fopen(SYS_CONFIG_PATH, "r");

	if (NULL == fp)
	{
		l_debug(ERR_LOG_PATH, "open %s error\n", SYS_CONFIG_PATH);
		return -1;
	}

	fscanf(fp, "%s", sys_config_info);
	fclose(fp);

	p_sys_conf = json_tokener_parse(sys_config_info);
	p_obj = json_object_array_get_idx(p_sys_conf, SYS_CONFIG_UART_DEV_INFO);
	p_port_dev = json_object_object_get(p_obj, "uartDevInfo");
	
	//save device information to g_UartDevInfo
	dev_sum = json_object_array_length(p_port_dev);

	for (i = 0; i < dev_sum; ++i)
	{
		p_obj = json_object_array_get_idx(p_port_dev, i);
		
		//baud rate
		g_UartDevInfo[i].m_BaudRate = json_object_get_int(json_object_object_get(p_obj, "baudRate"));
		//data bit
		g_UartDevInfo[i].m_DataBit = json_object_get_int(json_object_object_get(p_obj, "dataBit"));
		//parity
		g_UartDevInfo[i].m_Parity = json_object_get_string(json_object_object_get(p_obj, "parity"))[0];
		//stop bit
		g_UartDevInfo[i].m_StopBit = json_object_get_int(json_object_object_get(p_obj, "stopBit"));

		p_obj = json_object_object_get(p_obj, "dev");

		memcpy(g_UartDevInfo[i].m_Dev, json_object_get_string(p_obj), strlen(json_object_get_string(p_obj)));
		
		///*
		printf("%d,%d,%c,%d,%s\n",
			g_UartDevInfo[i].m_BaudRate,
			g_UartDevInfo[i].m_DataBit,
			g_UartDevInfo[i].m_Parity,
			g_UartDevInfo[i].m_StopBit,
			g_UartDevInfo[i].m_Dev);
		//*/
	}
	
	json_object_put(p_sys_conf);
#else
	uuchar *p_dev[] = {"/dev/ttyUSB0","/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3"};

	g_UartDevInfo[0].m_BaudRate = 9600,
	g_UartDevInfo[0].m_DataBit = 8,
	g_UartDevInfo[0].m_Parity = 'N',
	g_UartDevInfo[0].m_StopBit = 1,
	memcpy(g_UartDevInfo[0].m_Dev, p_dev[0], strlen(p_dev[0]));

	g_UartDevInfo[1].m_BaudRate = 9600,
	g_UartDevInfo[1].m_DataBit = 8,
	g_UartDevInfo[1].m_Parity = 'N',
	g_UartDevInfo[1].m_StopBit = 1,
	memcpy(g_UartDevInfo[1].m_Dev, p_dev[1], strlen(p_dev[1]));

	g_UartDevInfo[2].m_BaudRate = 9600,
	g_UartDevInfo[2].m_DataBit = 8,
	g_UartDevInfo[2].m_Parity = 'N',
	g_UartDevInfo[2].m_StopBit = 1,
	memcpy(g_UartDevInfo[2].m_Dev, p_dev[2], strlen(p_dev[2]));

	g_UartDevInfo[3].m_BaudRate = 9600,
	g_UartDevInfo[3].m_DataBit = 8,
	g_UartDevInfo[3].m_Parity = 'N',
	g_UartDevInfo[3].m_StopBit = 1,
	memcpy(g_UartDevInfo[3].m_Dev, p_dev[3], strlen(p_dev[3]));
#endif
	return 0;	
}
/***********************************************************************
**Function Name	: Uart_Open
**Description	: open uart device.
**Parameters	: portNo - in.
**Return		: 0 - ok -1 - failure.
***********************************************************************/
int Uart_Open(int portNo)
{
	int fd;

	//open uart g_UartDevInfo[portNo - 1].m_Dev
	fd = open(g_UartDevInfo[portNo - 1].m_Dev, O_RDWR|O_NOCTTY|O_NDELAY);
		
	if (fd < 0)
	{
		perror("open serial port");
		return(-1);
	}
	
	/*»Ö¸´´®¿ÚÎª×èÈû×´Ì¬*/
	if (fcntl(fd, F_SETFL, 0) < 0)
	{
		perror("fcntl F_SETFL\n");
		return(-2);
	}
	
	/*²âÊÔÊÇ·ñÎªÖÕ¶ËÉè±¸*/
	if (isatty(STDIN_FILENO) == 0)
	{
		perror("standard input is not a terminal device");
		return(-3);
	}
	
	//set uart paramter
	SetUart(fd, 
		g_UartDevInfo[portNo - 1].m_BaudRate, 
		g_UartDevInfo[portNo - 1].m_DataBit, 
		g_UartDevInfo[portNo - 1].m_Parity, 
		g_UartDevInfo[portNo - 1].m_StopBit);

	g_UartDevInfo[portNo - 1].m_Fd = fd;

	return 0;
}

/***********************************************************************
**Function Name	: Uart_Close
**Description	: close uart opened.
**Parameters	: portNo - in.
**Return		: none.
***********************************************************************/
void Uart_Close(int portNo)
{
	close(g_UartDevInfo[portNo - 1].m_Fd);
	
	g_UartDevInfo[portNo - 1].m_Fd = -1;
}

/***********************************************************************
**Function Name	: Uart_StartRecvThrd
**Description	: create a thread to recieve uart data.
**Parameters	: portNo - port no.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
int Uart_StartRecvThrd(int portNo)
{
	pthread_t thread = {0};
	pthread_attr_t thread_attr = {0};
	void *thrd_ret 	= NULL;
	int res 		= 0;

	do
	{
		if (g_UartDevInfo[portNo - 1].m_Fd < 0)
		{
			l_debug(ERR_LOG_PATH, "%s: %d fd error\n", __FUNCTION__, portNo);
			break;
		}

		res = pthread_attr_init(&thread_attr);
		if (0 != res)
		{
			l_debug(ERR_LOG_PATH, "%s: thread attr init failed!\n", __FUNCTION__);
			return -1;
		}

		res = pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
		if (0 != res)
		{
			l_debug(ERR_LOG_PATH, "%s:set scope failed!\n", __FUNCTION__);
			return -1;		
		}

		res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		if (0 != res)
		{
			l_debug(ERR_LOG_PATH, "%s:set detach state failed\n", __FUNCTION__);
			return -1;
		}

		res = pthread_create(&thread, &thread_attr, (void*)RecieveUartDataThrd, (void*)portNo);
		if (0 != res)
		{
			l_debug(ERR_LOG_PATH, "%s:create thread task failed!\n", __FUNCTION__);
			return -1;
		}

		pthread_attr_destroy(&thread_attr);
	
	}while(0);

	return 0;

}
/***********************************************************************
**Function Name	: Uart_SendData
**Description	: send data to uart.
**Parameters	: portNo - port no.
				: pData - data.
				: len - data len.
**Return		: 0 - ok -1 - failed.
***********************************************************************/
int Uart_SendData(int portNo, const uchar *pData, int len)
{
	int write_real = 0;

	write_real = write(g_UartDevInfo[portNo - 1].m_Fd, pData, len);

	if (len == write_real)
	{	
		return 0;
	}

	return -1;
}
/***********************************************************************
**Function Name	: RecieveUartDataThrd
**Description	: this is a thread to recieve uart data.
**Parameters	: pArg - in.
**Return		: none.
***********************************************************************/
static void	RecieveUartDataThrd(void *pArg)
{
	uchar rx_buff[UART_RX_BUFFER_SIZE] = {0};
	int real_read = 0;
	int fd = 0;
	int max_fd = 0;
	int res = 0;
	int port_no = (int)(pArg);
	fd_set set;
	fd_set tmp_set;

	l_debug(RUN_LOG_PATH, "-------start %d uart recieve thread-------\n", port_no);

	fd = g_UartDevInfo[port_no - 1].m_Fd;
	max_fd = fd;

	FD_ZERO(&set);
	FD_SET(fd, &set);

	while (FD_ISSET(fd, &set))
	{
		tmp_set = set;
		//block mode
		res = select(max_fd + 1, &tmp_set, NULL, NULL, NULL);

		switch(res)
		{
			case -1:
				l_debug(ERR_LOG_PATH, "%s:%d uart select error", __FUNCTION__, port_no);
				break;
			case 0:
				l_debug(ERR_LOG_PATH, "%s:%d uart select tiemout error", __FUNCTION__, port_no);
				break;
			default:
				
				if (FD_ISSET(fd, &tmp_set))
				{
					//clear rx_buff;
					memset(rx_buff, 0, UART_RX_BUFFER_SIZE);

					real_read = read(fd, rx_buff, UART_RX_BUFFER_SIZE);

					if (0 < real_read)
					{
#if 1
						{
						    int j = 0;
						    l_debug(NULL, "----------------------------------uart %d(%d) data\n", port_no, fd);
						    for (; j < real_read; ++j)
						    {
						        L_DEBUG("0x%.2x,",rx_buff[j]);
						    }
						    L_DEBUG("\n");
						}
#endif		
						//do here
						Uds_DataProcess(port_no, rx_buff, real_read);
					}
				}
			
				break;
		}
	}

	l_debug(ERR_LOG_PATH, "%s:%d uart error", __FUNCTION__,  port_no);
}
/***********************************************************************
**Function Name	: SetUart
**Description	: set uart paramter.
**Parameters	: fd - uart fd.
				: baud_rate - in.
				: data_bits - in.
				: parity - in.
				: stop_bits - in.
**Return		: none.
***********************************************************************/
static int SetUart(int fd, int baud_rate, int data_bits, uchar parity, int stop_bits)
{
	struct termios new_cfg,old_cfg;
	int speed;

	/*±£´æ²âÊÔÏÖÓÐ´®¿Ú²ÎÊýÉèÖÃ£¬ÔÚÕâÀïÈç¹û´®¿ÚºÅµÈ³ö´í£¬»áÓÐÏà¹ØµÄ³ö´íÐÅÏ¢*/
	if  (tcgetattr(fd, &old_cfg)  !=  0) 
	{
		perror("tcgetattr");
		return -1;
	}
	
	/*²½ÖèÒ»£¬ÉèÖÃ×Ö·û´óÐ¡*/
	new_cfg = old_cfg;
	cfmakeraw(&new_cfg);
	new_cfg.c_cflag &= ~CSIZE; 
	
  	/*ÉèÖÃ²¨ÌØÂÊ*/
  	switch (baud_rate)
  	{
  		case 2400:
		{
			speed = B2400;
		}
		break;

  		case 4800:
		{
			speed = B4800;
		}
		break;

  		case 9600:
		{
			speed = B9600;
		}
		break;
  		
		case 19200:
		{
			speed = B19200;
		}
		break;

  		case 38400:
		{
			speed = B38400;
		}
		break;
		
		default:
		case 115200:
		{
			speed = B115200;
		}
		break;
  	}
	cfsetispeed(&new_cfg, speed);
	cfsetospeed(&new_cfg, speed);

	/*ÉèÖÃÍ£Ö¹Î»*/
	switch (data_bits)
	{
		case 7:
		{
			new_cfg.c_cflag |= CS7;
		}
		break;
		
		default:
		case 8:
		{
			new_cfg.c_cflag |= CS8;
		}
		break;
  	}
  	
  	/*ÉèÖÃÆæÅ¼Ð£ÑéÎ»*/
  	switch (parity)
  	{
		default:
		case 'n':
		case 'N':
		{
			new_cfg.c_cflag &= ~PARENB;   
			new_cfg.c_iflag &= ~INPCK;    
		}
		break;
		
		case 'o':
		case 'O':
		{
			new_cfg.c_cflag |= (PARODD | PARENB); 
			new_cfg.c_iflag |= INPCK;            
		}
		break;
		
		case 'e':
		case 'E':
		{
			new_cfg.c_cflag |= PARENB;     
			new_cfg.c_cflag &= ~PARODD;   
			new_cfg.c_iflag |= INPCK;    
		}
		break;

		case 's':  /*as no parity*/
		case 'S':
		{
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_cflag &= ~CSTOPB;
		}
		break;
	}
		
	/*ÉèÖÃÍ£Ö¹Î»*/
	switch (stop_bits)
	{
		default:
		case 1:
		{
			new_cfg.c_cflag &=  ~CSTOPB;
		}
		break;

		case 2:
		{
			new_cfg.c_cflag |= CSTOPB;
		}
	}
	
	/*ÉèÖÃµÈ´ýÊ±¼äºÍ×îÐ¡½ÓÊÕ×Ö·û*/
	
	new_cfg.c_cc[VTIME]  = 1;
	new_cfg.c_cc[VMIN] = (UART_RX_BUFFER_SIZE - 1);
	
	/*´¦ÀíÎ´½ÓÊÕ×Ö·û*/
	tcflush(fd, TCIFLUSH);
	
	/*¼¤»îÐÂÅäÖÃ*/
	if((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)
	{
		perror("tcsetattr");
		return -1;
	}
	
	return 0;
}

