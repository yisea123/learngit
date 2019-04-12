/*
 * runtask.h
 *
 *  Created on: 2017年9月29日
 *      Author: yzg
 */

#ifndef RUNTASK_H_
#define RUNTASK_H_

extern OS_SEM		UnpackDataSem;
extern OS_SEM		RunSpeedCountSem;
extern OS_SEM		EditSem;//文本修改保护信号量
extern INT16S 		g_iRunLine;//手摇时跟行显示行数


void	DataUpack_Task(void *data);
void    Get_ComData_Task(void *data);
void    Run_CheckIO_Task(void *data);
void	Check_Hand_Task(void *data);
void    Display_XYZ_Task(void *data);
void    Display_RunSpeedCount_Task(void *data);


#endif /* RUNTASK_H_ */
