#ifndef _ASYNC_CMD_H_
#define _ASYNC_CMD_H_

typedef struct _AsynCmd
{
	
}AsynCmd;

/***********************************************************************
**Function Name	: AsynCmd_Init
**Description	: init async cmd queue and create a thread to performace
				  cmd.
**Parameters	: none.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern int		AsynCmd_Init();
/***********************************************************************
**Function Name	: AsynCmd_Add
**Description	: add a cmd.
**Parameters	: none.
**Return		: 0 - ok, -1 - failed.
***********************************************************************/
extern int 		AsynCmd_Add();

#endif //_ASYNC_CMD_H_