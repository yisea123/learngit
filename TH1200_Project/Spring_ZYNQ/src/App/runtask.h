/*
 * runtask.h
 *
 *  Created on: 2017��9��29��
 *      Author: yzg
 */

#ifndef RUNTASK_H_
#define RUNTASK_H_

extern OS_SEM		UnpackDataSem;
extern OS_SEM		RunSpeedCountSem;
extern OS_SEM		EditSem;//�ı��޸ı����ź���
extern INT16S 		g_iRunLine;//��ҡʱ������ʾ����


void	DataUpack_Task(void *data);
void    Get_ComData_Task(void *data);
void    Run_CheckIO_Task(void *data);
void	Check_Hand_Task(void *data);
void    Display_XYZ_Task(void *data);
void    Display_RunSpeedCount_Task(void *data);


#endif /* RUNTASK_H_ */
