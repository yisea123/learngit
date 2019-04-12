/*
 * Teach1.c
 *
 *  Created on: 2017��9��25��
 *      Author: yzg
 */

#include <public1.h>
#include <Parameter.h>
#include "Teach1.h"
#include "ff.h"
#include "public.h"
#include "base.h"
#include "string.h"
#include "rtc.h"

#define  	PRG_PTH     "\\PROG"
#define  	PAR_PTH     "\\PARA"


INT16U Current_Teach_Num;//��ǰ���ڱ༭���ݵ�
TEACH_COMMAND		Line_Data[MAXLINE+1];	//����ֱ�������������ʽ���Է�ֹȫ�ֱ����������ർ�³���64K������
TProductParam	g_Productparam; //�ļ�����
TEACH_HEAD		g_TeachHead;    //��ǰʹ���ļ���Ϣ
INT16U Line_Data_Size=0;//ָʾ��ǰ��������

INT32U Teach_Over_Time;	//��������


BOOLEAN bProductSaveFlag; //�ļ��������
BOOLEAN bTeachFileSaveFlag;  //�̵��ļ��������
INT8U File_SHA[ZEN_SHA1_HASH_SIZE]; //�ļ�У��

/*
�����ļ��г�ʼ��
*/
void File_Init(void)  						//20070123 modify add
{

	char    current_path[30];
	FRESULT res;
	DIR dp;

	strcpy(current_path,"1:\\PROG\\");
	res = f_opendir(&dp,current_path);

	if(res !=FR_OK)
	{
		res = f_mkdir(PRG_PTH);        /* Create a sub directory */
	}

	f_closedir (&dp);          /* Close an open directory */

	memset(current_path,0x00,sizeof(current_path));
	strcpy(current_path,"1:\\PARA\\");
	res = f_opendir(&dp,current_path);

	if(res !=FR_OK)
	{
		res = f_mkdir(PAR_PTH);        /* Create a sub directory */
	}

	f_closedir (&dp);

}


/***********************************************************************
                         �ļ�����У��ͳ�ʼ��
***********************************************************************/
void ProductPara_Save(void)
{

	while(bProductSaveFlag) My_OSTimeDly(10);
	bProductSaveFlag = TRUE;

    INT32U *p,XORECC,*p2;
    MG_CRC32(0,0);
	p=p2=(INT32U*)&g_Productparam;
	XORECC=MG_CRC32((unsigned char *)p,4);
	p2+=sizeof(TProductParam)/4;
	p2--;
	for(;;)
	{
		p++;
		if(p>=p2)break;
		XORECC=MG_CRC32((unsigned char *)p,4);
	}
	MG_CRC32(0,0xffffffff);
	g_Productparam.ECC=XORECC;
   // ProgramParamSave2(3,(INT8U *)&g_Productparam ,sizeof(TProductParam));
	ProgramParamSave1(0,(INT8U *)&g_Productparam ,sizeof(TProductParam));
	ps_debugout("ProgramParamSave1\r\n");
    bProductSaveFlag=FALSE;

}


void Init_ProductPara(void)
{
    //ProgramParamLoad2(3,(INT8U *)&g_Productparam ,sizeof(TProductParam));
	ProgramParamLoad1(0,(INT8U *)&g_Productparam ,sizeof(TProductParam));
    ps_debugout1("TProductParam sizeof=%d\r\n",sizeof(TProductParam));

    ps_debugout1("g_Productparam.version=%d\r\n",g_Productparam.version);

    if(g_Productparam.version!=TProductParam_Ver)
	{
    	ps_debugout("@@SRAM Check Err,Inital The SRAM\r\n");
	   	Init_ProductPara1();
	}

    ps_debugout1("g_Productparam.CurrentFileName==%s\r\n",g_Productparam.CurrentFileName);

    {
		INT32U *p,XORECC,*p2;
		MG_CRC32(0,0);
		p=p2=(INT32U*)&g_Productparam;
		XORECC=MG_CRC32((unsigned char *)p,4);
		p2+=sizeof(TProductParam)/4;
		p2--;
		for(;;)
		{
			p++;
			if(p>=p2)break;
			XORECC=MG_CRC32((unsigned char *)p,4);
		}
		MG_CRC32(0,0xffffffff);

		if(g_Productparam.ECC!=XORECC)
		{
			ps_debugout("g_Sysparam check Err...\r\n");
			g_Productparam.ECC=0;
			Init_ProductPara1();
		}

	}

    //�ӹ��ܵ��ٶ����ƣ���ֹδ���ó������³����µ�ϵͳ�У���У��ͨ�����������������
    if(g_Productparam.WorkSpeedRate<1)
    {
    	g_Productparam.WorkSpeedRate=1;//�ӹ����ٶȰٷֱ�����
    	ProductPara_Save();
    }
    else
    if(g_Productparam.WorkSpeedRate>300)
    {
    	g_Productparam.WorkSpeedRate=300;
    	ProductPara_Save();
    }

    //������ť�ٷֱ�����
    if(g_Productparam.Da_Value<0)
	{
		g_Productparam.Da_Value=0;//������ť�ٷֱ�����
		ProductPara_Save();
	}
	else
	if(g_Productparam.Da_Value>100)
	{
		g_Productparam.Da_Value=100;
		ProductPara_Save();
	}

#if (g_bRelease)
    //ps_debugout("reset the g_Productparam.PASSWFLAG...........\r\n");
    g_Productparam.PASSWFLAG=REG_USER;  //�ϵ��Ĭ�ϲ������ͣ���ǰ�����û����ͣ�����Ա
#endif
}

void Init_ProductPara1(void)
{
	strcpy((char *)g_Productparam.CurrentFileName,"ADT1.THJ");
	strcpy((char *)g_Productparam.CurrentFilePath,"0:/PROG/ADT1.THJ");

	g_Productparam.lRunCount = 0;//�Ѽӹ�����
	g_Productparam.WorkSpeedRate = 100;//�ӹ����ٶȰٷֱ�����
	g_Productparam.SuperPassw=0;			//�����û�����
	g_Productparam.ProgrammerPassw=0;	//��̵�����Ա����
	g_Productparam.OperationPassw=0;		//����Ա����
	g_Productparam.PASSWFLAG=REG_GUEST;          //��ǰ�����û����ͣ�����
	g_Productparam.Da_Value=0;//������ť�ٷֱ�����
	g_Productparam.version=TProductParam_Ver;

	ProductPara_Save();
}

/*
 * ���ص�ǰ�ӹ��ļ�
 */
void LoadWorkFile(void)
{
	ReadProgFile(g_Productparam.CurrentFilePath);
	ps_debugout("LoadFile:%s\r\n",g_Productparam.CurrentFilePath);

}

//��ȡ�ӹ��ļ�
int ReadProgFile(char *filepath)
{

	FIL file;
	int res,size,Filelen;
	int i;

	res = f_open(&file,(char *)filepath,FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	if(res != FR_OK)
	{
		ps_debugout("load file Err(%d)\r\n",res);
		return 0;
	}

	memset((char *)&g_TeachHead,0x00,sizeof(TEACH_HEAD));

	Filelen = f_size(&file) - sizeof(TEACH_HEAD);
	if (Filelen%sizeof(TEACH_COMMAND) != 0)
	{
		Line_Data_Size=0;
		ps_debugout("read file Err\r\n");
		f_close(&file);
		return -1;
	}

	f_read(&file,(char *)&g_TeachHead,sizeof(TEACH_HEAD),(UINT *)&size);

	//Line_Data_Size=g_TeachHead.DataLine;//��ǰ�ļ���Ч��������
	g_Sysparam.lDestCount=g_TeachHead.lDestCount;//��ǰ�ļ�Ŀ�����
	g_Sysparam.lProbeCount=g_TeachHead.lProbeCount;//��ǰ�ļ�̽�����

	for(i=Ax0;i<MaxAxis;i++)
	{
		g_Sysparam.AxisParam[i].fStartOffset = g_TeachHead.fStartOffset[i];
	}

	bSystemParamSaveFlag=TRUE;//ϵͳ���������־

	Line_Data_Size = Filelen/sizeof(TEACH_COMMAND);
	ps_debugout("Line_Data_Size==%d,g_TeachHead.DataLine==%d\r\n",Line_Data_Size,g_TeachHead.DataLine);

	memset((char *)&Line_Data,0x00,sizeof(TEACH_COMMAND)*Line_Data_Size);

	f_read(&file,(char *)&Line_Data,sizeof(TEACH_COMMAND)*Line_Data_Size,(UINT *)&size);

	f_close(&file);

	//sha1((INT8U*)&Line_Data,Line_Data_Size*sizeof(TEACH_COMMAND),File_SHA);
	//ps_debugout("ReadProgFile::File_SHA==%s\r\n",File_SHA);
	return 0;

}


//����ӹ��ļ�
void SaveProgFile(char *filepath)
{

	while(bTeachFileSaveFlag) My_OSTimeDly(10);
	bTeachFileSaveFlag = TRUE;

	FIL file;
	int res,size;
	int i;

	res = f_open(&file,(char *)filepath,FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

	if(res != FR_OK)
	{
		ps_debugout("save file Err(%d)\r\n",res);
		return;
	}

	{
		RtcGetDateStr((INT8S*)g_TeachHead.ModifyData);//�ļ��޸�����
		RtcGetTimeStr((INT8S*)g_TeachHead.ModifyTime);//�ļ��޸�ʱ��
		g_TeachHead.DataLine=Line_Data_Size;//��ǰ�ļ�����
		g_TeachHead.lDestCount=g_Sysparam.lDestCount;//��ǰ�ļ�Ŀ�����
		g_TeachHead.lProbeCount=g_Sysparam.lProbeCount;//��ǰ�ļ�̽�����
		for(i=Ax0;i<MaxAxis;i++)
		{
			g_TeachHead.fStartOffset[i]=g_Sysparam.AxisParam[i].fStartOffset;
		}

	}

	f_write(&file,(char *)&g_TeachHead,sizeof(TEACH_HEAD),(UINT *)&size);
	f_write(&file,(char *)Line_Data,sizeof(TEACH_COMMAND)*Line_Data_Size,(UINT *)&size);

	f_close(&file);

	//sha1((INT8U*)&Line_Data,Line_Data_Size*sizeof(TEACH_COMMAND),File_SHA);
	//ps_debugout("SaveProgFile::File_SHA==%s\r\n",File_SHA);
	bTeachFileSaveFlag = FALSE;

}


/*
 * ����4������Ϊ�ӹ��ļ��̵���������
 */
//�޸ĵ�ǰ������
INT8U Teach_Data_Replace(TEACH_COMMAND *data,INT16U num)
{

	if(num > Line_Data_Size)
	{
		return 1;
	}

	//�����ǰ�������
	Line_Data[num] = (TEACH_COMMAND)(*data);

	return 0;
}

//����,�������,num	Ҫ�����λ��,��0��ʼ
INT8U Teach_Data_Insert(TEACH_COMMAND *data,INT16U num)
{

	INT16S i;
	INT16U InsetNum; //�������

	if((Line_Data_Size) > MAXLINE || num > Line_Data_Size)
	{
		return 3;
	}

	if(num == 0)  //�����������
	{
		if(Line_Data_Size != 0)
		{
			for(i=Line_Data_Size-1; i>=0; i--)
			{
				Line_Data[i+1] = Line_Data[i];
			}
		}


		Line_Data[0] = (TEACH_COMMAND)(*data);

	}
	else
	{
		InsetNum = num;
		for(i=Line_Data_Size-1; i>=InsetNum; i--)  //��Ҫ���Ƶ�����
		{
			Line_Data[i+1] = Line_Data[i];
		}

		for(i=0;i<1;i++)  //�����������
		{
			Line_Data[i+InsetNum] = (TEACH_COMMAND)(*data);
		}
	}

	Line_Data_Size = Line_Data_Size+1;

	return 0;

}

/*
 *  ���ȫ������
 */
INT8U Teach_Data_Delete_ALL(void)
{

	memset(Line_Data,0x00,sizeof(Line_Data[0])*(MAXLINE+1));
	Line_Data_Size=0;
	return 0;

}

//ɾ��һ������
INT8U Teach_Data_Delete(INT16U num)
{

	INT16S i;		//����ʹ���з�������,��ֹ������ת

	if(Line_Data_Size == 0 || num >= Line_Data_Size)
	{
		return 1;
	}

	//��ǰ�ƶ�һ��
	for(i=num; i<Line_Data_Size-1; i++)
	{
		Line_Data[i] = Line_Data[i+1];
	}

	Line_Data_Size --;


	return 0;

}


