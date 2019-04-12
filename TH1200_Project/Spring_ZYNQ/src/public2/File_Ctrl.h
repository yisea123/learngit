/*
 * File_Ctrl.h
 *
 *  Created on: 2017��9��16��
 *      Author: yzg
 */

#ifndef FILE_CTRL_H_
#define FILE_CTRL_H_

#include "adt_datatype.h"
#include "os.h"

#define		FILE_OUT_ENABLE	//���ڴ�ӡ��־λ

#ifdef	FILE_OUT_ENABLE
	#define	FileOut	ps_debugout		//����0��ӡ������Ϣ
#else
	#define	FileOut	FileComOut		//����0����ӡ������Ϣ
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
