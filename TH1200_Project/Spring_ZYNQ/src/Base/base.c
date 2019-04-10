/*
 * base.c
 *
 *  Created on: 2015年9月8日
 *      Author: Administrator
 */

#include "base.h"
#include "runtask.h"
#include "Parameter_public.h"

//不打印串口调试信息标志位
BOOL 	bNoPrint;
//掉电中断标志位
BOOL g_bPW_LOW = FALSE;


U16 GetRandom(U16 tMin, U16 tMax)
{
	return (U16)( (rand()%(tMax-tMin+1)) + tMin);
}

void My_OSTimeDly(int num)
{
	OS_ERR ERR;
	OSTimeDly(num, OS_OPT_TIME_DLY, &ERR);
}

void Uart_SendByte(u8 data)
{
	uart_send(0, data);
}

void Uart_SendString(char *string)
{
	while(*string)
		Uart_SendByte(*string++);
}

/*
 * 打印信息
 */
void ps_debugout(char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Uart_SendString(string);
	va_end(ap);

}

void ps_debugout1(char *fmt,...)
{
	va_list ap;
	char string[256];
	if(g_Sysparam.DebugEnable)//调试信息使能
	{
		va_start(ap,fmt);
		vsprintf(string,fmt,ap);
		Uart_SendString(string);
		va_end(ap);
	}

}

void Debugout(char *fmt,...)
{
	//不打印串口调试信息函数
	if(bNoPrint) return;

	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Uart_SendString(string);
	va_end(ap);
}
//void ps_debugout(char *fmt, ...)
//{
//	va_list ap;
//	char str[256];
//	va_start(ap,fmt);
//	vsprintf(str, fmt, ap);
//	printf("%s",str);
//	va_end(ap);
//	return ;
//}


/*
 *  加载字库
 * */

void Hzk_Init(void)
{
	int result;

	result = InitHzk();
	if(!(result & 0x01)){
		ps_debugout1("XXX InitHzk HZK16 err = %d XXX\r\n",result);
	}
	if(!(result & 0x02)){
		ps_debugout1("XXX InitHzk HZK24 err = %d XXX\r\n",result);
	}
	if(result & 0x03){
		ps_debugout1("*** InitHzk OK ***\r\n");
	}
	ps_debugout1("/*********************************/ \r\n");

}

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
#if 0
void get_build_date_time(unsigned short *usYear, unsigned char *ucMonth,  unsigned char *ucDay,
                         unsigned char  *ucHour, unsigned char *ucMinute, unsigned char *ucSecond)
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

	*ucMonth = i + 1;
	if(buf[4] != ' ')
		*ucDay   = (buf[4] - '0')*10   + (buf[5] - '0');
	else
		*ucDay   = (buf[5] - '0');
	*usYear  = (buf[7] - '0')*1000 + (buf[8] - '0')*100 + (buf[9] - '0')*10 + (buf[10] - '0');

	strcpy(buf, __TIME__);
//	ps_debugout("TIME :buf = %s \n", buf);

	*ucHour   = (buf[0] - '0')*10 + (buf[1] - '0');
	*ucMinute = (buf[3] - '0')*10 + (buf[4] - '0');
	*ucSecond = (buf[6] - '0')*10 + (buf[7] - '0');
}
#endif
/*
 * 掉电中断检测
 */

static UCOS_INT_FNCT_PTR PW_LOW_fun(void)
{
	if(FALSE == g_bPW_LOW){
		g_bPW_LOW = TRUE;
		ps_debugout("*******************\r\n");
		ps_debugout("*                 *\r\n");
		ps_debugout("*  掉电响应成功   *\r\n");
		ps_debugout("*                 *\r\n");
		ps_debugout("*******************\r\n");
//		UCOS_IntSrcDis(PW_LOW_INT_IRQ_ID);
	}
	return NULL;
}

/*
 * 掉电检测
 */
void PW_LOW_Init()
{
	UCOS_IntVectSet(PW_LOW_INT_IRQ_ID,
	                     0u,
	                     0u,
	                     (void*)PW_LOW_fun,
	                     (void *)0);

	UCOS_IntSrcEn(PW_LOW_INT_IRQ_ID);
}



