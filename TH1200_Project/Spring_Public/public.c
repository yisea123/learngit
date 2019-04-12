/*
 * public.c
 *
 *  Created on: 2017年8月10日
 *      Author: yzg
 */

#include "Public.h"
#include "adt_datatype.h"
#include "string.h"

//该名称可在初始化的时候，初始化成自定义配置的轴名称
//各轴名称,直接用轴号去获取,增加轴名称的配置性
char *Axis_Name[MaxAxis]=
{
	"轴1",
	"轴2",
	"轴3",
	"轴4",
	"轴5",
	"轴6",
	"轴7",
	"轴8",
	"轴9",
	"轴10",
	"轴11",
	"轴12",
	"轴13",
	"轴14",
	"轴15",
	"轴16"
};

/*
 * 万能机各轴名称初始化
 */
char *Axis_Name1[MaxAxis]=
{
	"切刀",//开始角   结束角
	"送线",
	"转芯",
	"卷曲1",
	"卷曲2",
	"夹耳",
	"X4",
	"X5",
	"X6",
	"X7",
	"X8",
	"J1",
	"J2",
	"A",
	"B",
	"C"
};

#ifndef WIN32
/*******************************************************************************
Function:  get_build_date_time
Description: 获取系统文件编译的日期与时间
Input:          无
Output:   usYear、ucMonth、ucDay  年\月\日
                ucHour、ucMinute、ucSecond  时\分\秒
Return   无
Others:   无
一月        Jan.        January
二月        Feb.        February
三月        Mar.        March
四月        Apr.        April
五月        May.        May
六月        June.       June
七月        July.       July
八月        Aug.        Aguest
九月        Sept.       September
十月        Oct.        October
十一月      Nov.        November
十二月      Dec.        December *******************************************************************************/
void get_build_date_time1(BUILDDATE *buildate)
{
	char pMonth[12][3] = {
	{'J', 'a', 'n'}, {'F', 'e', 'b'}, {'M', 'a', 'r'},
	{'A', 'p', 'r'}, {'M', 'a', 'y'}, {'J', 'u', 'n'},
	{'J', 'u', 'l'}, {'A', 'u', 'g'}, {'S', 'e', 'p'},
	{'O', 'c', 't'}, {'N', 'o', 'v'}, {'D', 'e', 'c'},
	};

	char buf[0x20] = {0};
	int i, j;

	strcpy(buf, __DATE__);
//	ps_debugout("DATE :buf = %s \n", buf);
	//读出 月数
	for(i = 0; i < 12; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(pMonth[i][j] != buf[j])
			{
				break;
			}
		}

		if(j >= 3)
		{
			break;
		}
	}

	buildate->ucMonth = i + 1;
	if(buf[4] != ' ')
		buildate->ucDay   = (buf[4] - '0')*10   + (buf[5] - '0');
	else
		buildate->ucDay   = (buf[5] - '0');
	buildate->usYear  = (buf[7] - '0')*1000 + (buf[8] - '0')*100 + (buf[9] - '0')*10 + (buf[10] - '0');

	strcpy(buf, __TIME__);
//	ps_debugout("TIME :buf = %s \n", buf);

	buildate->ucHour   = (buf[0] - '0')*10 + (buf[1] - '0');
	buildate->ucMinute = (buf[3] - '0')*10 + (buf[4] - '0');
	buildate->ucSecond = (buf[6] - '0')*10 + (buf[7] - '0');
}
#endif // #ifndef WIN32
