#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/time.h>
#include "sqlite3.h"
#include "Type.h"

#ifndef NULL
#define NULL (void*)0
#endif

#define PLATFORM	"mips"

//--------------------------------------MACRO---------------------------------------//

#define TIME					struct timeval
#define GET_SYS_CURRENT_TIME(n)	gettimeofday(&n, NULL)
#define IS_TIMEOUT(n,m)			IsTimeout((int)&n, m)

#define CONV_TO_INT(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | d)

#if 1
#define L_DEBUG	printf
#else
#define L_DEBUG
#endif

#define MAX_SLAVE_SUM		64	//-- the max number of connecting slaves a aisle --//
#define SLAVE_ADDR_LEN		2	//-- the length of slave address --//
#define INVAILD_SLAVE_ADDR	65535

 //-----------------------------------MACRO END-------------------------------------//

//-------------------------------------------NEW TYPE------------------------------------//

//-----------------------------------------NEW TYPE END-----------------------------------//

//---------------------------------------DECLARATION VARIAVLE--------------------------------------------//

//-------------------------------------DECLARATION VARIABLE END-------------------------------------------//

//--------------------------------------------------DECLARATION FUNCTION----------------------------------------//

/***********************************************************************
**Function Name	: IsTimeout
**Description	: check whether interval time(start time ~ current time) is over threshold
				: note: min threshold value is 1ms
**Parameters	: StartTime - start time.
				: threshold - in
**Return		: 0 - not timeout, 1 - timeout
***********************************************************************/
extern int 				IsTimeout(int StartTime, unsigned int threshold);
/***********************************************************************
**Function Name	: l_debug
**Description	: print debug information or save it.
**Parameters	: pLogPath - in.
				: fmt - format.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern void 			l_debug(const uchar *pLogPath, uchar *fmt,...);
/***********************************************************************
**Function Name	: DelayMS
**Description	: delay ? ms,but it is not exact.
**Parameters	: xms - in.
**Return		: none.
***********************************************************************/
extern void 			DelayMS(unsigned int xms);	
/***********************************************************************
**Function Name	: CheckCode
**Description	: create a crc code.
**Parameters	: pData - in.
				: len - data len .
**Return		: crc code.
***********************************************************************/
extern ushort			CheckCode(const uchar *pData, int len); 			

//-----------------------------------------------DECLARATION FUNCTION END--------------------------------------------//

#endif

