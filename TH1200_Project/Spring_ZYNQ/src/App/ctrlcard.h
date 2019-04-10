/*
 * ctrlcard.h
 *
 *  Created on: 2017年9月14日
 *      Author: yzg
 */

#ifndef CTRLCARD_H_
#define CTRLCARD_H_
#include <public1.h>
#include "adt_datatype.h"
#include "public.h"


////

typedef void (*FUNCTION)(void);

extern FP64		g_fPulseOfAngle[MaxAxis];
extern FP64		g_fPulseOfMM[MaxAxis];
extern INT32S		g_lPulseOfCircle[MaxAxis];
extern int			g_iCardBeiLv[MaxAxis];
extern INT32U  	g_ulOutStatus;
extern INT16S		g_iWheelAxis;
extern INT8U		g_ucEtherCATErr;



void	CtrlCard_Init(void);
BOOL  	InitBoard(void) ;
int BaseSetCommandPos(int axis,long pos);				//当缓存设零时，与get_command_pos偏移量基值
int BaseGetCommandPos(int axis,long *pos);				//获取缓存偏移量基值上座标位置 ;
int BaseSetCommandOffset(int axis,long posoffset);

void	SetUnitPulse(INT16S ch,FP32 mm,INT32S pulse);
FP32	GetUnitPulse(INT16S ch);
void	SetMMConSpeed(INT16S ch,FP32 mmspeed);
void	SetMMAddSpeed(INT16S ch,FP32  mmstartspeed,FP32 mmspeed,FP32 mmaddspeed);
void	SetMMAddSpeed1(INT16S ch,INT16S ch1,FP32  mmstartspeed,FP32 mmspeed,FP32 mmaddspeed);
void	SetPulseConSpeed(INT16S ch,INT32S speed);
void	SetMM(INT16S ch,FP32 mm,BOOL cur);
void	SetAngle(INT16S ch,float angle,BOOL cur);
void	SetPulse(INT16S ch, INT32S pos,BOOL cur);
FP32	GetMM(INT16S ch,BOOL cur);
FP32	GetAngle(INT16S ch,BOOL cur);
INT32S	GetPulse(INT16S ch,BOOL cur);
float	GetMMFromPulse(INT16S ch,INT32S pulse) ;
INT32S	GetPulseFromMM(INT16S ch,FP32 mm);
float	GetAngleFromPulse(INT16S ch,INT32S pulse);
INT32S	GetPulseFromAngle(INT16S ch,FP32 angle);
float	GetMMFromAngle(INT16S ch,FP32 angle);
float	GetAngleFromMM(INT16S ch,FP32 mm);
void	Start( INT16S ch );
void	Stop1( INT16S ch );
void	Stop2( INT16S ch );
void	StopAll();

INT16S	IsFree(INT16S ch ) ;
INT16S 	IsInpEnd(INT16S no) ;

INT16U  GetStopData(INT16S ch);
FP32    GetInpSpeed(void);

void	ShortlyPmove(INT16S ch,INT32S pos2,INT32S speed);
void	ShortlyPmoveLimit(INT16S ch,FP32 posLimit,INT32S speed,INT16S dir);
void	ShortlyInp_Move2(INT32S pos2,INT32S pos4,INT32S speed);
void	ShortlyMoveAngle(INT16S ch,FP32 angle,FP32 speed);
void	MoveMM(INT16S ch,FP32 mm,FP32 speed,FUNCTION pfn);
void	AddMoveMM(INT16S ch,FP32 pos,FP32 startspeed,FP32 speed,FP32 addspeed);
void	AddMoveMM1(INT16S ch,FP32 pos,FP32 startspeed,FP32 speed,FP32 addspeed);
void	ShortlyMoveMM(INT16S ch, FP32 mm, FP32 speed);
BOOL 	CheckHome(INT16S);
BOOL	BackZero(INT16S ch,INT16S dir);
BOOL    ZeroByOrder(void);
INT16S 	BackZeroByOrder(int axisNum[14]);
BOOL	TestBackZero(INT16S ch,INT16S dir);
BOOL	FindZero(INT16S ch, INT16S style, INT32S speed);
INT16S  ReadBit(INT16S io);
INT16S 	RealIO(INT16S io,INT16S status,INT16S ti);
INT16S 	WriteBit(INT16S io,INT16S status);
INT16S 	GetOut(INT16S io);
INT16U inportb(INT16U port);
void outportb(INT16U port,INT16U data);
INT16S OpenProbe(INT16S cardno,INT16S pro,INT16S output);
INT16S CloseProbe(INT16S cardno);
INT16S ReadProbe(INT16S cardno,INT16S pro);
BOOL DA_Out(FP32 value,INT16S ch);
INT16S CheckWarning(void);
BOOL HomeCheckLimit(INT8S ch,INT8S dir);
INT16S  CheckHardLimit(INT16S ch, INT32S dir);
INT16S  CheckSoftLimit(INT16S ch,INT32S dir);
INT16S ReadFifo(void);
INT16S	SetRange(INT32S rate);
INT16S	SetQuiteRange(INT32S rate);
INT32S GetRange();
INT16S CheckLimit(BOOL bSoftCk,BOOL bHardCk,INT16S ch,INT32S dir);
int vm_sync(void);
BOOL	TDorCutFindZero(INT16S ch,INT16S style, INT32S speed);
int TDExecute(int SpeedRate);
int JXExecuteZero();




#endif /* CTRLCARD_H_ */
