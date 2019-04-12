/*
 * Teach1.c
 *
 *  Created on: 2017年9月25日
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


INT16U Current_Teach_Num;//当前正在编辑数据点
TEACH_COMMAND		Line_Data[MAXLINE+1];	//不用直接声明数组的形式可以防止全局变量声明过多导致超过64K的限制
TProductParam	g_Productparam; //文件参数
TEACH_HEAD		g_TeachHead;    //当前使用文件信息
INT16U Line_Data_Size=0;//指示当前数据行数

INT32U Teach_Over_Time;	//成型周期


BOOLEAN bProductSaveFlag; //文件互斥操作
BOOLEAN bTeachFileSaveFlag;  //教导文件互斥操作
INT8U File_SHA[ZEN_SHA1_HASH_SIZE]; //文件校验

/*
磁盘文件夹初始化
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
                         文件参数校验和初始化
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

    //加工总的速度限制，防止未设置出错或更新程序到新的系统中，但校验通过的情况下做防错处理
    if(g_Productparam.WorkSpeedRate<1)
    {
    	g_Productparam.WorkSpeedRate=1;//加工中速度百分比设置
    	ProductPara_Save();
    }
    else
    if(g_Productparam.WorkSpeedRate>300)
    {
    	g_Productparam.WorkSpeedRate=300;
    	ProductPara_Save();
    }

    //测试旋钮百分比限制
    if(g_Productparam.Da_Value<0)
	{
		g_Productparam.Da_Value=0;//测试旋钮百分比设置
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
    g_Productparam.PASSWFLAG=REG_USER;  //上电后默认操作类型，当前操作用户类型：操作员
#endif
}

void Init_ProductPara1(void)
{
	strcpy((char *)g_Productparam.CurrentFileName,"ADT1.THJ");
	strcpy((char *)g_Productparam.CurrentFilePath,"0:/PROG/ADT1.THJ");

	g_Productparam.lRunCount = 0;//已加工产量
	g_Productparam.WorkSpeedRate = 100;//加工中速度百分比设置
	g_Productparam.SuperPassw=0;			//超级用户密码
	g_Productparam.ProgrammerPassw=0;	//编程调试人员密码
	g_Productparam.OperationPassw=0;		//操作员密码
	g_Productparam.PASSWFLAG=REG_GUEST;          //当前操作用户类型：来宾
	g_Productparam.Da_Value=0;//测试旋钮百分比设置
	g_Productparam.version=TProductParam_Ver;

	ProductPara_Save();
}

/*
 * 加载当前加工文件
 */
void LoadWorkFile(void)
{
	ReadProgFile(g_Productparam.CurrentFilePath);
	ps_debugout("LoadFile:%s\r\n",g_Productparam.CurrentFilePath);

}

//读取加工文件
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

	//Line_Data_Size=g_TeachHead.DataLine;//当前文件有效数据行数
	g_Sysparam.lDestCount=g_TeachHead.lDestCount;//当前文件目标产量
	g_Sysparam.lProbeCount=g_TeachHead.lProbeCount;//当前文件探针次数

	for(i=Ax0;i<MaxAxis;i++)
	{
		g_Sysparam.AxisParam[i].fStartOffset = g_TeachHead.fStartOffset[i];
	}

	bSystemParamSaveFlag=TRUE;//系统参数保存标志

	Line_Data_Size = Filelen/sizeof(TEACH_COMMAND);
	ps_debugout("Line_Data_Size==%d,g_TeachHead.DataLine==%d\r\n",Line_Data_Size,g_TeachHead.DataLine);

	memset((char *)&Line_Data,0x00,sizeof(TEACH_COMMAND)*Line_Data_Size);

	f_read(&file,(char *)&Line_Data,sizeof(TEACH_COMMAND)*Line_Data_Size,(UINT *)&size);

	f_close(&file);

	//sha1((INT8U*)&Line_Data,Line_Data_Size*sizeof(TEACH_COMMAND),File_SHA);
	//ps_debugout("ReadProgFile::File_SHA==%s\r\n",File_SHA);
	return 0;

}


//保存加工文件
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
		RtcGetDateStr((INT8S*)g_TeachHead.ModifyData);//文件修改日期
		RtcGetTimeStr((INT8S*)g_TeachHead.ModifyTime);//文件修改时间
		g_TeachHead.DataLine=Line_Data_Size;//当前文件行数
		g_TeachHead.lDestCount=g_Sysparam.lDestCount;//当前文件目标产量
		g_TeachHead.lProbeCount=g_Sysparam.lProbeCount;//当前文件探针次数
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
 * 以下4个函数为加工文件教导操作函数
 */
//修改当前条数据
INT8U Teach_Data_Replace(TEACH_COMMAND *data,INT16U num)
{

	if(num > Line_Data_Size)
	{
		return 1;
	}

	//替代当前条的组号
	Line_Data[num] = (TEACH_COMMAND)(*data);

	return 0;
}

//插入,添加数据,num	要插入的位置,从0开始
INT8U Teach_Data_Insert(TEACH_COMMAND *data,INT16U num)
{

	INT16S i;
	INT16U InsetNum; //待插入点

	if((Line_Data_Size) > MAXLINE || num > Line_Data_Size)
	{
		return 3;
	}

	if(num == 0)  //如果插入首行
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
		for(i=Line_Data_Size-1; i>=InsetNum; i--)  //需要后移的数据
		{
			Line_Data[i+1] = Line_Data[i];
		}

		for(i=0;i<1;i++)  //待插入的数据
		{
			Line_Data[i+InsetNum] = (TEACH_COMMAND)(*data);
		}
	}

	Line_Data_Size = Line_Data_Size+1;

	return 0;

}

/*
 *  清空全部数据
 */
INT8U Teach_Data_Delete_ALL(void)
{

	memset(Line_Data,0x00,sizeof(Line_Data[0])*(MAXLINE+1));
	Line_Data_Size=0;
	return 0;

}

//删除一条数据
INT8U Teach_Data_Delete(INT16U num)
{

	INT16S i;		//必须使用有符号数据,防止发生反转

	if(Line_Data_Size == 0 || num >= Line_Data_Size)
	{
		return 1;
	}

	//向前移动一条
	for(i=num; i<Line_Data_Size-1; i++)
	{
		Line_Data[i] = Line_Data[i+1];
	}

	Line_Data_Size --;


	return 0;

}


