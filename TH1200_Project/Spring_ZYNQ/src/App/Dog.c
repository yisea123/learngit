/*
 * Dog.c
 *
 *  Created on: 2018��1��17��
 *      Author: yzg
 */

#include "Dog.h"
#include "stdlib.h"
#include "base.h"
#include "public1.h"
#include "SysAlarm.h"
#include "CheckTask.h"

#define		MAXPASSDAY		(365*2)		//ǰһ��¼�����뵱ǰ���ڵ�����ֵ����

DOG	 Dog;//�����ݽṹ��
CHECKDOG	CheckDog;//���ܹ����ݽṹ��
SYSTIME  SysTime;//ϵͳ����ʱ��
BOOLEAN bDogSaveFlag; //�ļ��������
INT16U g_DogProcess;//���ܹ��������Ʊ�־
INT16U g_DogProcess2;//���ܹ�2�������Ʊ�־
/*
 * ��ʼ�������ݲ����湷����
 */
INT32U 	InitDog(void)
{
	Dog.Dogversion=TDog_Ver;//���汾
	Dog.User_Active=12345678;     								//ָʾ�Ƿ����û�����������,12345678�������
	RtcGetDate(&Dog.User_LastDay);
	Dog.First_Use=0;  											//0Ϊ��һ��ʹ�ò�Ʒ������Ϊ�ǵ�һ��
	Dog.Factory_SerialNo=111111;    							//�������к�Ĭ��Ϊ111111���������豸���ļ��豸��������
	RtcGetTime(&Dog.Now_Time); 									//��¼�û�������������ĵ�ǰʱ��

	Dog.Machine_Password=1234;									//�ͻ��趨�����������Ĭ��Ϊ1234
	Dog.Machine_SerialNo=123;									//�ͻ��������к�Ĭ��Ϊ123
	Dog.Dog_InitData = 11221;  									//���ڹ��ĳ�ʼ��
	Dog.RandomNum=1111;   										//�����������
	Dog.User_Level=1;  											//ȷ���û���ǰ������ȷ����������һ���������
	Dog.User_RemainDay = 0; 	                                // ����ʣ������

	Dog.Dog_Type=2;                                             //1-���ö��ڹ���2-���õ��ڹ�  Ĭ�����õ��ڹ�

	{
		unsigned int i = 0;
		for(i = 0; i < 24; i++)
		{
			Dog.User_Days[i]=30;   							// 30 ��
			srand(i*10 + GetTimes(Dog.Now_Time)); 			// ��������
			Dog.User_Password[i]=rand()%900000 + 100000;	//�������6λ����

			ps_debugout1("Init Dog.User_Password[%d]=%d\r\n",i,Dog.User_Password[i]);

			//ps_debugout1("Dog.Now_Time=%d\r\n",GetTimes(Dog.Now_Time));
		}
	}

	WriteDog();//��ʼ���󱣴�����

	return TRUE;
}

/*
 * д�빷����
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
 *��ȡ������
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
 * ���ܹ����ݵĶ�ȡ����
 */
void DogDataProcess(void)
{

	DATE_T	today;
	TIME_T	now;


	ReadDog();//��ȡ������

	if(Dog.Dog_InitData != 11221)
	{
		InitDog();	//	��ʼ�����ݲ���������
	}

	if(Dog.First_Use!=123456) //
	{
		RtcGetDate(&Dog.User_LastDay);//��¼����
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
		//�����ƣ�ֱ�������߼���
		ps_debugout("���ܹ� ������\r\n");
	}
	else
	if(Dog.Dog_Type==1)//���ö��ڹ�
	{
		//  ���ȼ������
		INT32S NowDays,DogRecordDays,err;

		RtcGetDate(&today);
		RtcGetTime(&now);

		NowDays=GetDays(today);
		DogRecordDays=GetDays(Dog.User_LastDay);
		err=NowDays-DogRecordDays;

		if(err>0)
		{
																//���ο���������ϴο��������Ѿ������û���������
			if(err>MAXPASSDAY)										//����ʱ�䶪ʧ���¸�λ����ִ�����
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

			Dog.User_LastDay=today; 							//��¼���ο�����Ӧ������
		}
		else
		{
																//��ǰһ���ڷ�������
			if(err==0&&Dog.User_LastDay.year==today.year
					&&Dog.User_LastDay.month==today.month
					&&Dog.User_LastDay.day==today.day)
			{
																//ʱ�䱻������ǰ�����������Զ���1
				if(GetTimes(now)<GetTimes(Dog.Now_Time))
					Dog.User_RemainDay--;
			}
			else												//���ڱ�������ǰ�����������Զ���1,ͬʱ��¼��ǰ����
			{
				Dog.User_RemainDay--;
				Dog.User_LastDay=today;
			}
		}



		if(Dog.User_RemainDay < 1)
		{
			//��Ҫ��λ����Ͻ���
			g_DogProcess=1;//��λ�����Ʊ�־���ϣ���ʼ����
			ps_debugout("�ȴ�������ܹ�����\r\n");
		}
		else
		{
			Dog.Now_Time=now;
			WriteDog();//������ܹ�����
		}
	}
	else
	if(Dog.Dog_Type==2)//���õ��ڹ�
	{

		//  ���ȼ������
		INT32S NowDays,DogRecordDays,err;

		RtcGetDate(&today);
		RtcGetTime(&now);

		NowDays=GetDays(today);
		DogRecordDays=GetDays(Dog.User_LastDay);
		err=NowDays-DogRecordDays;

		if(err>0)
		{
																//���ο���������ϴο��������Ѿ������û���������
			if(err>MAXPASSDAY)										//����ʱ�䶪ʧ���¸�λ����ִ�����
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

			Dog.User_LastDay=today; 							//��¼���ο�����Ӧ������
		}
		else
		{
																//��ǰһ���ڷ�������
			if(err==0&&Dog.User_LastDay.year==today.year
					&&Dog.User_LastDay.month==today.month
					&&Dog.User_LastDay.day==today.day)
			{
																//ʱ�䱻������ǰ�����������Զ���1
				if(GetTimes(now)<GetTimes(Dog.Now_Time))
					Dog.User_RemainDay--;
			}
			else												//���ڱ�������ǰ�����������Զ���1,ͬʱ��¼��ǰ����
			{
				Dog.User_RemainDay--;
				Dog.User_LastDay=today;
			}
		}


		if(Dog.User_RemainDay < 5)
		{
			if(Dog.User_RemainDay < 1)
			{
				//��Ҫ��λ�����+��λ�����������Ͻ���
				g_DogProcess2=1;//��λ�����Ʊ�־���ϣ���ʼ����
				ps_debugout("�ȴ�������ܹ�2����\r\n");
			}
			else
			{
				g_DogProcess2=2;//��λ�����Ʊ�־���ϣ���ʼ��ʾ��Ҫ����
				Dog.Now_Time=now;
				WriteDog();//������ܹ�����
			}
		}
		else
		{
			WriteDog();//������ܹ�����
		}

	}

	return;

}

/*
 * �����ݼ�⴦���Դﵽ����Ŀ��
 */
void CheckPassword(void)
{
	TIME_T	now;

	/*
	 * �ȴ���λ�������ܹ�������ú��������ߣ���λ��Ӧ����������һ����λ�������ߵı�־
	 */
	for(;;)
	{
		if(Dog.Dog_Type==1)//���ڹ�
		{
			if(!g_DogProcess)
			{
				RtcGetTime(&now);
				Dog.Now_Time=now;
				WriteDog();//������ܹ�����
				ps_debugout("������ܹ�����\r\n");
				MessageBox(DogDecryptSucess);//���ܹ����ܳɹ�
				break;//ͨ��
			}
		}
		else
		if(Dog.Dog_Type==2)//���ڹ�
		{
			if(!g_DogProcess2)
			{
				RtcGetTime(&now);
				Dog.Now_Time=now;
				WriteDog();//������ܹ�����
				ps_debugout("������ܹ�2����\r\n");
				break;//ͨ��
			}
			else
			if(g_DogProcess2==2)//���ܹ������С��5���ˣ����ѿͻ���ǰ����
			{
				break;
			}
		}
		My_OSTimeDly(50);
	}

	return;

}
