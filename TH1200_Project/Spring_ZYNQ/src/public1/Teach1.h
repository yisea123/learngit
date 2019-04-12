/*
 * Teach1.h
 *
 *  Created on: 2017年9月25日
 *      Author: yzg
 */

#ifndef TEACH1_H_
#define TEACH1_H_



#include "adt_datatype.h"
#include "public.h"
#include "WorkManage.h"
#include "Teach_public.h"
#include "SHA-1_MD5.h"

//在此文件中定义与教导相关的数据

# define	SPEED		0x10
# define	MOVE		0x11
# define	PEND		0x12
# define	JUMP		0x13
# define	GOTO		0x14

//2M
# define	XMSSIZE		(INT32U)(2*1024*1024)
# define	DATASIZE	(INT32U)(2*1024*1024+1)


/*
 * 数据加工解析结构体
 */
typedef struct
{
	INT32U	nLine;							//行号
	INT8U 	nCode;							//指令
	INT8U	SpindleAxis	;					//有主轴，1为X轴，2为Y，3为Z，4为A轴
	INT32S	lValue[17];						//v0-v15为1~16轴，v16为速度
	INT8U	nIO;							//探针
	INT8U	nOut[MAXCYLINDER];				//气缸
	INT32U	nReset;							//回零标示。二进制按位表示1111，0000
	FP32	fDelay;							//

} UNITDATA;



#define TProductParam_Ver 0  //当前加工文件参数版本
typedef struct
{
	INT32U	version;
	char	CurrentFileName[FILE_LEN];  //当前加工文件名
	char	CurrentFilePath[256]; //当前加工文件路径
	INT32U  lRunCount;   //已加工产量
	INT16U  WorkSpeedRate;//加工中速度百分比设置
	INT32U  SuperPassw;			//超级用户密码
	INT32U  ProgrammerPassw;	//编程调试人员密码
	INT32U	OperationPassw;		//操作员密码
	INT16U  PASSWFLAG;//当前操作用户类型
	INT16S  Da_Value;//测试旋钮百分比设置
	INT8S   temp[40];    //留下40个字节做以后结构体扩充兼容老版本用
	INT32U	ECC;
}TProductParam;


//当前文件头数据
typedef struct
{
	INT16U	DataLine;     //数据总行数
	INT8S	ModifyData[10];  //修改日期
	INT8S	ModifyTime[10];  //修改时间
	INT32S lDestCount;//当前文件目标产量
	INT32S lProbeCount;//当前文件探针次数
	FP32	fStartOffset[MaxAxis];//指示各轴原点位置相对于原点开关的偏移值,单位:0.1度(角度), 0.1毫米(长度)
	INT8S  Message[64];//当前文件的辅助信息
	INT8S  temp[64];////留下64个字节做以后结构体扩充兼容老版本用，使之前旧的文件也可以兼容
}TEACH_HEAD;


extern INT16U Current_Teach_Num;//当前正在加工数据点
extern TEACH_COMMAND	Line_Data[MAXLINE+1];//教导缓冲数据段
extern TProductParam	g_Productparam; //产品参数
extern TEACH_HEAD		g_TeachHead;    //当前使用文件信息
extern INT32U Teach_Over_Time;	//成型周期
extern INT16U Line_Data_Size;

extern BOOLEAN bProductSaveFlag; //文件互斥操作
extern BOOLEAN bTeachFileSaveFlag;  //教导文件互斥操作

extern INT8U File_SHA[ZEN_SHA1_HASH_SIZE]; //文件校验

void File_Init(void);  						//20070123 modify add
void ProductPara_Save(void);
void Init_ProductPara(void);
void Init_ProductPara1(void);
void LoadWorkFile();
int  ReadProgFile(char *filepath);
void SaveProgFile(char *filepath);

INT8U Teach_Data_Replace(TEACH_COMMAND *data,INT16U num);
INT8U Teach_Data_Insert(TEACH_COMMAND *data,INT16U num);
INT8U Teach_Data_Delete(INT16U num);
INT8U Teach_Data_Delete_ALL(void);

#endif /* TEACH1_H_ */
