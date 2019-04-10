/*
 * ParaManage.h
 *
 *  Created on: 2017Äê8ÔÂ10ÈÕ
 *      Author: yzg
 */

#ifndef PARAMANAGE_H_
#define PARAMANAGE_H_

#include "SysText.h"
#include "adt_datatype.h"

#define REG_READ			0x00
#define REG_WRITE			0x01
#define	REG_MUTEXMSK		0x80

#define ma(a) ((long)(a*10000))

int IOFilterParamProc(TParamTable *pParam, void *pData, INT16U len);
int  UnpackParamProc (TParamTable *pParam, void *pData, INT16U len);
int HomeBitParamProc(TParamTable *pParam, void *pData, INT16U len);
int AdmodeParamProc(TParamTable *pParam, void *pData, INT16U len);
int	LimtParamProc(TParamTable *pParam, void *pData, INT16U len);
int MageParamProc(TParamTable *pParam, void *pData, INT16U len);
int	GenParamProc(TParamTable *pParam, void *pData, INT16U len);
int	AxisParamProc(TParamTable *pParam, void *pData, INT16U len);
int GearParamProc(TParamTable *pParam, void *pData, INT16U len);
int SwitchParamProc(TParamTable *pParam, void *pData, INT16U len);
int StartOffsetProc(TParamTable *pParam, void *pData, INT16U len);
int SlaveZeroSet(TParamTable *pParam, void *pData, INT16U len);
int BackModeProc(TParamTable *pParam, void *pData, INT16U len);
int SetResetFlag(int axis,BOOLEAN flag);
int SetForceResetFlag(int axis,BOOLEAN flag);
int AxisMulRunModeProc(TParamTable *pParam, void *pData, INT16U len);
int AxisRunModeProc(TParamTable *pParam, void *pData, INT16U len);
int  BackOrderParamProc(TParamTable *pParam, void *pData, INT16U len);


#endif /* PARAMANAGE_H_ */
