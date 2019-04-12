/*
 * Dog.c
 *
 *  Created on: 2018年1月17日
 *      Author: yzg
 */

#include "Dog.h"
#include "stdlib.h"
#include "base.h"
#include "public1.h"
#include "SysAlarm.h"
#include "CheckTask.h"

#define		MAXPASSDAY		(365*2)		//前一记录日期与当前日期的最大差值限制

DOG	 Dog;//狗数据结构体
CHECKDOG	CheckDog;//解密狗数据结构体
SYSTIME  SysTime;//系统日期时间
BOOLEAN bDogSaveFlag; //文件互斥操作
INT16U g_DogProcess;//加密狗不需限制标志
INT16U g_DogProcess2;//加密狗2不需限制标志
/*
 * 初始化狗数据并保存狗数据
 */
INT32U 	InitDog(void)
{
	Dog.Dogversion=TDog_Ver;//狗版本
	Dog.User_Active=12345678;     								//指示是否解除用户级密码限制,12345678解除限制
	RtcGetDate(&Dog.User_LastDay);
	Dog.First_Use=0;  											//0为第一次使用产品，其它为非第一次
	Dog.Factory_SerialNo=111111;    							//厂商序列号默认为111111用来区分设备是哪家设备厂生产的
	RtcGetTime(&Dog.Now_Time); 									//记录用户级访问软件狗的当前时间

	Dog.Machine_Password=1234;									//客户设定软件狗的密码默认为1234
	Dog.Machine_SerialNo=123;									//客户机器序列号默认为123
	Dog.Dog_InitData = 11221;  									//用于狗的初始化
	Dog.RandomNum=1111;   										//狗的随机索引
	Dog.User_Level=1;  											//确定用户当前级别以确定需输入那一级别的密码
	Dog.User_RemainDay = 0; 	                                // 当期剩余天数

	Dog.Dog_Type=2;                                             //1-启用多期狗，2-启用单期狗  默认启用单期狗

	{
		unsigned int i = 0;
		for(i = 0; i < 24; i++)
		{
			Dog.User_Days[i]=30;   							// 30 天
			srand(i*10 + GetTimes(Dog.Now_Time)); 			// 给种子数
			Dog.User_Password[i]=rand()%900000 + 100000;	//随机生成6位密码

			ps_debugout1("Init Dog.User_Password[%d]=%d\r\n",i,Dog.User_Password[i]);

			//ps_debugout1("Dog.Now_Time=%d\r\n",GetTimes(Dog.Now_Time));
		}
	}

	WriteDog();//初始化后保存数据

	return TRUE;
}

/*
 * 写入狗数据
 */
INT32U  WriteDog(void)
{
	while(bDogSaveFlag) My_OSTimeDly(10);
	bDogSaveFlag = TRUE;


	{
		INT32U *p,XORECC,*p2;

		MG_CRC32(0,0);
		p=p2=(INT32U*)&Dog;
		XORECC=MG_CRC32((unsigned char *)p,4);
		p2+=sizeof(Dog)/4;
		p2--;
		for(;;)
		{
			p++;
			if(p>=p2)break;
			XORECC=MG_CRC32((unsigned char *)p,4);
		}
		MG_CRC32(0,0xffffffff);
		Dog.ECC=XORECC;

		ProgramParamSave1(1,(INT8U *)&Dog ,sizeof(Dog));
		ps_debugout1("ProgramParamSave1--1\r\n");
		bDogSaveFlag=FALSE;
	}

	return 0;
}

/*
 *读取狗数据
 */
INT32U  ReadDog(void)
{
	ProgramParamLoad1(1,(INT8U *)&Dog ,sizeof(Dog));
	ps_debugout1("Dog sizeof=%d\r\n",sizeof(Dog));

	if(Dog.Dogversion!=TDog_Ver)
	{
		ps_debugout("dog Version err\r\n");
		InitDog();
	}

	{
		INT32U *p,XORECC,*p2;
		MG_CRC32(0,0);
		p=p2=(INT32U*)&Dog;
		XORECC=MG_CRC32((unsigned char *)p,4);
		p2+=sizeof(Dog)/4;
		p2--;
		for(;;)
		{
			p++;
			if(p>=p2)break;
			XORECC=MG_CRC32((unsigned char *)p,4);
		}
		MG_CRC32(0,0xffffffff);

		if(Dog.ECC!=XORECC)
		{
			ps_debugout("Dog check Err...\r\n");
			Dog.ECC=0;
			InitDog();
		}

	}

	return 0;
}

/*
 * 加密狗数据的读取处理
 */
void DogDataProcess(void)
{

	DATE_T	today;
	TIME_T	now;


	ReadDog();//读取狗数据

	if(Dog.Dog_InitData != 11221)
	{
		InitDog();	//	初始化数据并保存数据
	}

	if(Dog.First_Use!=123456) //
	{
		RtcGetDate(&Dog.User_LastDay);//记录日期
		//Dog.User_RemainDay = 0;
		Dog.First_Use=123456;
		WriteDog();
	}

	if(Dog.User_RemainDay>9999)
	{
		Dog.User_RemainDay=9999;
		WriteDog();
	}

	if(Dog.User_Active==12345678)
	{
		//无限制，直接往下走即可
		ps_debugout("加密狗 无限制\r\n");
	}
	else
	if(Dog.Dog_Type==1)//启用多期狗
	{
		//  首先检测日期
		INT32S NowDays,DogRecordDays,err;

		RtcGetDate(&today);
		RtcGetTime(&now);

		NowDays=GetDays(today);
		DogRecordDays=GetDays(Dog.User_LastDay);
		err=NowDays-DogRecordDays;

		if(err>0)
		{
																//本次开机相对于上次开机天数已经大于用户限制天数
			if(err>MAXPASSDAY)										//日期时间丢失重新复位会出现此现象
			{
				if(Dog.User_RemainDay>1)
				{
					Dog.User_RemainDay-=1;
				}
				else
				{
					Dog.User_RemainDay = 0;
				}
			}
			else
			{
				if(err>=Dog.User_RemainDay)
				{
					Dog.User_RemainDay=0;
				}
				else
					Dog.User_RemainDay-=err;
			}

			Dog.User_LastDay=today; 							//记录本次开机对应的日期
		}
		else
		{
																//当前一天内反复开机
			if(err==0&&Dog.User_LastDay.year==today.year
					&&Dog.User_LastDay.month==today.month
					&&Dog.User_LastDay.day==today.day)
			{
																//时间被更改提前则限制天数自动减1
				if(GetTimes(now)<GetTimes(Dog.Now_Time))
					Dog.User_RemainDay--;
			}
			else												//日期被更改提前则试用天数自动减1,同时记录当前日期
			{
				Dog.User_RemainDay--;
				Dog.User_LastDay=today;
			}
		}



		if(Dog.User_RemainDay < 1)
		{
			//需要上位机配合解密
			g_DogProcess=1;//下位机限制标志置上，开始限制
			ps_debugout("等待解除加密狗限制\r\n");
		}
		else
		{
			Dog.Now_Time=now;
			WriteDog();//保存加密狗数据
		}
	}
	else
	if(Dog.Dog_Type==2)//启用单期狗
	{

		//  首先检测日期
		INT32S NowDays,DogRecordDays,err;

		RtcGetDate(&today);
		RtcGetTime(&now);

		NowDays=GetDays(today);
		DogRecordDays=GetDays(Dog.User_LastDay);
		err=NowDays-DogRecordDays;

		if(err>0)
		{
																//本次开机相对于上次开机天数已经大于用户限制天数
			if(err>MAXPASSDAY)										//日期时间丢失重新复位会出现此现象
			{
				if(Dog.User_RemainDay>1)
				{
					Dog.User_RemainDay-=1;
				}
				else
				{
					Dog.User_RemainDay = 0;
				}
			}
			else
			{
				if(err>=Dog.User_RemainDay)
				{
					Dog.User_RemainDay=0;
				}
				else
					Dog.User_RemainDay-=err;
			}

			Dog.User_LastDay=today; 							//记录本次开机对应的日期
		}
		else
		{
																//当前一天内反复开机
			if(err==0&&Dog.User_LastDay.year==today.year
					&&Dog.User_LastDay.month==today.month
					&&Dog.User_LastDay.day==today.day)
			{
																//时间被更改提前则限制天数自动减1
				if(GetTimes(now)<GetTimes(Dog.Now_Time))
					Dog.User_RemainDay--;
			}
			else												//日期被更改提前则试用天数自动减1,同时记录当前日期
			{
				Dog.User_RemainDay--;
				Dog.User_LastDay=today;
			}
		}


		if(Dog.User_RemainDay < 5)
		{
			if(Dog.User_RemainDay < 1)
			{
				//需要上位机软件+上位机解密软件配合解密
				g_DogProcess2=1;//下位机限制标志置上，开始限制
				ps_debugout("等待解除加密狗2限制\r\n");
			}
			else
			{
				g_DogProcess2=2;//下位机限制标志置上，开始提示快要限制
				Dog.Now_Time=now;
				WriteDog();//保存加密狗数据
			}
		}
		else
		{
			WriteDog();//保存加密狗数据
		}

	}

	return;

}

/*
 * 狗数据检测处理，以达到分期目的
 */
void CheckPassword(void)
{
	TIME_T	now;

	/*
	 * 等待上位机做加密狗检测设置后再往下走，上位机应当往下设置一个下位机往下走的标志
	 */
	for(;;)
	{
		if(Dog.Dog_Type==1)//多期狗
		{
			if(!g_DogProcess)
			{
				RtcGetTime(&now);
				Dog.Now_Time=now;
				WriteDog();//保存加密狗数据
				ps_debugout("解除加密狗限制\r\n");
				MessageBox(DogDecryptSucess);//加密狗解密成功
				break;//通过
			}
		}
		else
		if(Dog.Dog_Type==2)//单期狗
		{
			if(!g_DogProcess2)
			{
				RtcGetTime(&now);
				Dog.Now_Time=now;
				WriteDog();//保存加密狗数据
				ps_debugout("解除加密狗2限制\r\n");
				break;//通过
			}
			else
			if(g_DogProcess2==2)//加密狗离加密小于5天了，提醒客户提前解密
			{
				break;
			}
		}
		My_OSTimeDly(50);
	}

	return;

}
