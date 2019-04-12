/*
 * public.c
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#include "Public.h"
#include "adt_datatype.h"
#include "string.h"

//�����ƿ��ڳ�ʼ����ʱ�򣬳�ʼ�����Զ������õ�������
//��������,ֱ�������ȥ��ȡ,���������Ƶ�������
char *Axis_Name[MaxAxis]=
{
	"��1",
	"��2",
	"��3",
	"��4",
	"��5",
	"��6",
	"��7",
	"��8",
	"��9",
	"��10",
	"��11",
	"��12",
	"��13",
	"��14",
	"��15",
	"��16"
};

/*
 * ���ܻ��������Ƴ�ʼ��
 */
char *Axis_Name1[MaxAxis]=
{
	"�е�",//��ʼ��   ������
	"����",
	"תо",
	"����1",
	"����2",
	"�ж�",
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
