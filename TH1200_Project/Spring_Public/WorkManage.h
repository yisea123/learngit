/*
 * WorkManage.h
 *
 *  Created on: 2017年9月2日
 *      Author: yzg
 */

#ifndef WORKMANAGE_H_
#define WORKMANAGE_H_

#include "adt_datatype.h"
#include "SysText.h"
#include "public.h"

#define FILE_LEN  24
#define FS_DIRNAME_MAX 255
#define MAX_LIST       0x100

#ifndef NAME_MAX
#define NAME_MAX	256
#endif

#define		FILE_OUT_ENABLE	//串口打印标志位

#ifdef	FILE_OUT_ENABLE
	#define	FileOut	ps_debugout		//串口0打印调试信息
#else
	#define	FileOut	FileNoOut		//串口0不打印调试信息
#endif

#define	FS_FDISK			0x00	//磁盘
#define	FS_FDIR				0x01	//目录
#define	FS_FDOC				0x02	//文档

typedef struct _CMM_DIRENT_
{
	INT8U	Attr;			//文件属性(0：磁盘  1：目录  2：文档)
	char	Name[FILE_LEN+1];		//文件名(20+3格式)

	INT32U 	Size;			//文件大小(磁盘大小)
	INT32U	FreeSize;		//磁盘剩余字节大小
	INT16U	Fdate;			//修改日期
	INT16U	Ftime;			//修改时间
	//INT16U  Fower;			//文件所有者//在多通道时使用，为区别该文件为哪个通道所使用

//	INT8S 	Time[9];		//文件最后修改时间
}CMM_DIRENT;//单元大小24个字节

typedef struct _CMM_DIR_
{
	INT32U	uiDirent;			 //目录列表数目

	CMM_DIRENT pDirent[MAX_LIST];//目录列表信息

}CMM_DIR;

#ifndef WIN32
extern INT8U g_ProgList[FS_DIRNAME_MAX][FILE_LEN+10];
extern INT8U g_FilePath[MAX_LIST];//文件路径
extern TParamTable WorkRegTab[];
extern TH_1600E_VER THVer;
extern INT8U g_FileOpraPath[MAX_LIST];
extern INT16S ReturnUpdateErr;
extern TH_1600E_SYS THSys;
extern INT8U g_CopyFinish;
extern INT8U g_CutExecute;//剪线动作执行标志
extern INT8U g_TDExecute;//退刀动作执行标志
extern INT8U g_CutSta;//剪线动作执行状态
extern INT8U g_TdSta;//退刀动作执行状态
extern INT8U g_GenerateStatus;//数据生成状态
extern INT16U g_GenerateFinish;//数据生成完成标志
extern INT8U g_ResetMessage;//上位机按归零按钮时，报警信息清除标志


BOOLEAN	Copy_file_to_file(char *DestPath,char *SrcPath,BOOLEAN mode);
void WorkManage_Init(void);
void UpdateSystem(void);
void FileNoOut(char *fmt, ...);
#endif // WIN32

#endif /* WORKMANAGE_H_ */
