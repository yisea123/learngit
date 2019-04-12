/*
 * base.c
 *
 *  Created on: 2015��9��8��
 *      Author: Administrator
 */

#include "base.h"
#include "runtask.h"
#include "Parameter_public.h"

//����ӡ���ڵ�����Ϣ��־λ
BOOL 	bNoPrint;
//�����жϱ�־λ
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
 * ��ӡ��Ϣ
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
	if(g_Sysparam.DebugEnable)//������Ϣʹ��
	{
		va_start(ap,fmt);
		vsprintf(string,fmt,ap);
		Uart_SendString(string);
		va_end(ap);
	}

}

void Debugout(char *fmt,...)
{
	//����ӡ���ڵ�����Ϣ����
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
 *  �����ֿ�
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
Description: ��ȡϵͳ�ļ������������ʱ��
Input:          ��
Output:   usYear��ucMonth��ucDay  ��\��\��
                ucHour��ucMinute��ucSecond  ʱ\��\��
Return   ��
Others:   ��
һ��        Jan.        January
����        Feb.        February
����        Mar.        March
����        Apr.        April
����        May.        May
����        June.       June
����        July.       July
����        Aug.        Aguest
����        Sept.       September
ʮ��        Oct.        October
ʮһ��      Nov.        November
ʮ����      Dec.        December *******************************************************************************/
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
	//���� ����
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
 * �����жϼ��
 */

static UCOS_INT_FNCT_PTR PW_LOW_fun(void)
{
	if(FALSE == g_bPW_LOW){
		g_bPW_LOW = TRUE;
		ps_debugout("*******************\r\n");
		ps_debugout("*                 *\r\n");
		ps_debugout("*  ������Ӧ�ɹ�   *\r\n");
		ps_debugout("*                 *\r\n");
		ps_debugout("*******************\r\n");
//		UCOS_IntSrcDis(PW_LOW_INT_IRQ_ID);
	}
	return NULL;
}

/*
 * ������
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



