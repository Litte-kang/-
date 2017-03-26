#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "Common.h"
#include "Config.h"

//--------------------------------------DECLARATION VARIABLE--------------------------------//

//----------------------------------DECLARATION VARIABLE END --------------------------------//


/***********************************************************************
**Function Name	: IsTimeout
**Description	: check whether interval time(start time ~ current time) is over threshold
				: note: min threshold value is 1ms
**Parameters	: StartTime - start time.
				: threshold - in
**Return		: 0 - not timeout, 1 - timeout
***********************************************************************/
int IsTimeout(int StartTime, unsigned int threshold)
{
	TIME start = *(struct timeval*)(StartTime);
	TIME end;
	unsigned long interval = 0;

	GET_SYS_CURRENT_TIME(end);
	
	interval = 	1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
   	interval = interval / 1000;
   	
   	//Delay_ms(1);
	
	return (interval > threshold ? 1 : 0);
}

/***********************************************************************
**Function Name	: DelayMS
**Description	: delay ? ms,but it is not exact.
**Parameters	: xms - in.
**Return		: none.
***********************************************************************/
void DelayMS(unsigned int xms)
{
	struct timeval delay;
	
	delay.tv_sec = 0;
	delay.tv_usec = xms * 1000;
	
	select(0, NULL, NULL, NULL, &delay);
}

/***********************************************************************
**Function Name	: l_debug
**Description	: print debug information or save it.
**Parameters	: pLogPath - in.
				: fmt - format.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
void l_debug(const uchar *pLogPath, uchar *fmt,...)
{
	uchar str[1024] 						= {0};
  	va_list ap;
  	FILE *fp 								= NULL;
  	time_t now_time 						= {0};
	struct tm *p_now_time 					= NULL;

  	va_start(ap, fmt);
  	vsnprintf(str, 250, fmt, ap);
  	va_end(ap);
	
	L_DEBUG("%s\n", str);

	if (NULL != pLogPath)	//-- save debug information --//
	{		
		time(&now_time);
		p_now_time = localtime(&now_time);

		fp = fopen(pLogPath, "a");
		if (NULL == fp)
		{
			L_DEBUG("%s:open %s! error\n", __FUNCTION__, pLogPath);
			return;
		}
		
		fprintf(fp, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ----%s", 
			(p_now_time->tm_year + 1900),
			(p_now_time->tm_mon + 1),
			p_now_time->tm_mday,
			p_now_time->tm_hour,
			p_now_time->tm_min,
			p_now_time->tm_sec, 
			str);
		
		fclose(fp);
	}
	
	return;
}
/***********************************************************************
**Function Name	: CheckCode
**Description	: create a crc code.
**Parameters	: pData - in.
				: len - data len .
**Return		: crc code.
***********************************************************************/
ushort CheckCode(const uchar *pData, int len)
{
	ushort crc_code = 0;

	while (len)
	{
		crc_code += *pData++;

		len--;
	}

	return crc_code;
}
