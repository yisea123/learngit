/*
 * WorkManage.h
 *
 *  Created on: 2017��9��2��
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

#define		FILE_OUT_ENABLE	//���ڴ�ӡ��־λ

#ifdef	FILE_OUT_ENABLE
	#define	FileOut	ps_debugout		//����0��ӡ������Ϣ
#else
	#define	FileOut	FileNoOut		//����0����ӡ������Ϣ
#endif

#define	FS_FDISK			0x00	//����
#define	FS_FDIR				0x01	//Ŀ¼
#define	FS_FDOC				0x02	//�ĵ�

typedef struct _CMM_DIRENT_
{
	INT8U	Attr;			//�ļ�����(0������  1��Ŀ¼  2���ĵ�)
	char	Name[FILE_LEN+1];		//�ļ���(20+3��ʽ)

	INT32U 	Size;			//�ļ���С(���̴�С)
	INT32U	FreeSize;		//����ʣ���ֽڴ�С
	INT16U	Fdate;			//�޸�����
	INT16U	Ftime;			//�޸�ʱ��
	//INT16U  Fower;			//�ļ�������//�ڶ�ͨ��ʱʹ�ã�Ϊ������ļ�Ϊ�ĸ�ͨ����ʹ��

//	INT8S 	Time[9];		//�ļ�����޸�ʱ��
}CMM_DIRENT;//��Ԫ��С24���ֽ�

typedef struct _CMM_DIR_
{
	INT32U	uiDirent;			 //Ŀ¼�б���Ŀ

	CMM_DIRENT pDirent[MAX_LIST];//Ŀ¼�б���Ϣ

}CMM_DIR;

#ifndef WIN32
extern INT8U g_ProgList[FS_DIRNAME_MAX][FILE_LEN+10];
extern INT8U g_FilePath[MAX_LIST];//�ļ�·��
extern TParamTable WorkRegTab[];
extern TH_1600E_VER THVer;
extern INT8U g_FileOpraPath[MAX_LIST];
extern INT16S ReturnUpdateErr;
extern TH_1600E_SYS THSys;
extern INT8U g_CopyFinish;
extern INT8U g_CutExecute;//���߶���ִ�б�־
extern INT8U g_TDExecute;//�˵�����ִ�б�־
extern INT8U g_CutSta;//���߶���ִ��״̬
extern INT8U g_TdSta;//�˵�����ִ��״̬
extern INT8U g_GenerateStatus;//��������״̬
extern INT16U g_GenerateFinish;//����������ɱ�־
extern INT8U g_ResetMessage;//��λ�������㰴ťʱ��������Ϣ�����־


BOOLEAN	Copy_file_to_file(char *DestPath,char *SrcPath,BOOLEAN mode);
void WorkManage_Init(void);
void UpdateSystem(void);
void FileNoOut(char *fmt, ...);
#endif // WIN32

#endif /* WORKMANAGE_H_ */
