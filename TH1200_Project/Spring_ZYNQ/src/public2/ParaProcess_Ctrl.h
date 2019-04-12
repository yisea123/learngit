/*
 * ParaProcess_Ctrl.h
 *
 *  Created on: 2019Äê3ÔÂ18ÈÕ
 *      Author: yzg
 */

#ifndef PARAPROCESS_CTRL_H_
#define PARAPROCESS_CTRL_H_

#include "adt_datatype.h"
#include "os.h"

typedef enum
{
	PARAPROCESSCTL_NO = 0,
	PARAPROCESSCTL_BAK,
	PARAPROCESSCTL_INITIO,
	PARAPROCESSCTL_INITSYSPARA,
	PARAPROCESSCTL_INITALL

}PARAPROCESSCTL;

typedef struct
{
	PARAPROCESSCTL mode;
}PARAPROCESSTYPE;

extern OS_Q Q_ParaProcess_Msg;

INT8U Post_ParaProcess_Mess(PARAPROCESSTYPE *mess);
void ParaProcessTask (void *p_arg);


#endif /* PARAPROCESS_CTRL_H_ */
