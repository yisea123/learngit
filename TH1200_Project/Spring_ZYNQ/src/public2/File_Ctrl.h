/*
 * File_Ctrl.h
 *
 *  Created on: 2017年9月16日
 *      Author: yzg
 */

#ifndef FILE_CTRL_H_
#define FILE_CTRL_H_

#include "adt_datatype.h"
#include "os.h"

#define		FILE_OUT_ENABLE	//串口打印标志位

#ifdef	FILE_OUT_ENABLE
	#define	FileOut	ps_debugout		//串口0打印调试信息
#else
	#define	FileOut	FileComOut		//串口0不打印调试信息
#endif

typedef enum
{
	FILE_NO = 0,
	FILE_NEW,
	FILE_LOAD,
	FILE_COPY,
	FILE_PASTE,
	FILE_CUT,
	FILE_GENERATE
}FILECTL;

typedef struct
{
	FILECTL mode;
	INT8U size;
	INT8U type;
	char str[25];
}FILECOMMTYPE;

extern OS_Q Q_FileComm_Msg;
extern OS_SEM g_FileSem;

INT8U Post_FileComm_Mess(FILECOMMTYPE *mess);
void FileCommTask (void *p_arg);
void FileComOut(char *fmt, ...);


#endif /* FILE_CTRL_H_ */
