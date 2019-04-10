/*
 * runtask.c
 *
 *  Created on: 2017��9��29��
 *      Author: yzg
 */
#include "work.h"
#include "base.h"
#include "SysAlarm.h"
#include "TestIo.h"
#include "SysKey.h"
#include "ctrlcard.h"
#include "CheckTask.h"
#include "public.h"
#include "public2.h"
#include "math.h"
#include "int_pack.h"
#include "runtask.h"
#include "THMotion.h"
#include "ManualMotion.h"
#include "Manual_Ctrl.h"

OS_SEM		UnpackDataSem;//�ı������ź���
OS_SEM		RunSpeedCountSem;//�ӹ��ٶȼ����ź���
OS_SEM		EditSem;//�ı��޸ı����ź���
INT16S 		g_iRunLine=0;//��ҡʱ������ʾ����

static 		OS_ERR		os_err;

/*
 * ���ݽ�������
 */
void	DataUpack_Task(void *data)
{

	CPU_TS    p_ts;
	OS_ERR		os_err;

	ps_debugout("DataUpack_Task Running\r\n");

	for(;;)
	{
		OSSemPend(&UnpackDataSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);

		ps_debugout1("======DataUpack_Task Running=====\r\n");
		if(os_err==OS_ERR_NONE)
		{
			INT16S tmp;

			g_bUnpackRuning=TRUE;
			MessageBox(Unpack_Data);
			if((tmp=UnpackData()) != 0)
			{
				UnpackErrDisp(tmp);
				ps_debugout1("UnpackErr = %d\n",tmp);
			}
			else
			{
				MessageBox(Unpack_Data_Over);
			}

			g_bUnpackRuning=FALSE;
			//g_bModify = FALSE;
		}

	}

}


/*
 * �����ֺм��ӹ�����
 */
void    Run_CheckIO_Task(void *data)
{

	INT16U key=0;
	INT16S Count=0;
	INT32S pos=0;
	static MOTIONTYPE sendcmd;

	CPU_SR_ALLOC();

	ps_debugout("Run_CheckIO_Task Running\r\n");

	for(;;)
	{
		DelaySec(10);

		//�ļ��޸�������Ҫ���½���   //Ӧ����������ͣ������� �������½����ı������Է�������ҡ�ֽ��в���ʱ����
		//Ӧ������������״̬�������ݽ���������ѹ�ɻ��쳤ʱ����������ʱ��������ʵʱ����
#if 1
		if(g_bModify && !g_bUnpackRuning && !g_bBackZero && g_iWorkStatus!=GoZero/* && g_iWorkStatus!=Run && g_iWorkStatus!=Pause*/)
		{
			ps_debugout1("OSSemPost UnpackDataSem\r\n");
			//g_bUnpackRuning=TRUE;//  �������ݽ���
			g_bModify=FALSE;//
			OSSemPost(&UnpackDataSem,OS_OPT_POST_1,&os_err);
		}
#endif

		//�ⲿ���ܰ������
		key = GetInput1();


		/*if(key != NONE_KEY)
		{

			switch( key )
			{
				case TEST_KEY://���԰�ť

					//if(g_iWorkStatus==Run)
					{
						if(AlarmFlag)//����
						{
							break;
						}
						SetMM(17,0,FALSE);

						if(!g_bMode)
						{
							g_bTestToSingle=FALSE;
							g_bTestStart = TRUE;
							g_bMode=TRUE;
							Write_Output_Ele(OUT_SENDADD,0);
						}

						MessageBox(EnterTestModePromit);//��ʾ�������ģʽ
					}
					break;

				default:break;

			}

		}*/

		//���水ť
		/*if(!g_bPressSaveKey && key==SAVE_KEY)
		{
			g_bPressSaveKey=TRUE;


			if(g_bModify)
			{
				//MessageBox(ExitTestModePromit);//�˳�����ģʽ
			}
		}
		//��ֹ���뱣��
		if(g_bPressSaveKey && key!=SAVE_KEY)
		{
			g_bPressSaveKey=FALSE;
		}*/

		//�ⲿ���㰴ť add by yzg 2018.08.31
		if(!g_bPressResetIO && key==GOZERO_KEY)
		{
			g_bPressResetIO=TRUE;
			//��ֹͣ״̬����ʵ��û�б���������ʱ������Ϣ����û�����
			if(!g_Alarm && g_iWorkStatus==Stop)
			{
				Write_Alarm_Led(0);
				DispErrCode(No_Alarm);
			}

			//��ֹͣ״̬����û�б����������
			if(g_iWorkStatus==Stop && !AlarmFlag)
			{
				sendcmd.mode = Manual_Total_GoZero;
				sendcmd.axis = 0;
				sendcmd.pos = 0.0;
				Post_Motion_Mess(&sendcmd);
			}
		}
		//��ֹ�������
		if(g_bPressResetIO && !Read_Input_Func(IN_GOZERO) && key!=GOZERO_KEY)
		{
			g_bPressResetIO=FALSE;
		}


		//���԰�ť
		if(!g_bPressTestIO && key==TEST_KEY)
		{
			g_bPressTestIO=TRUE;

			//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� add by yzg 19.02.26
			if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage && g_Interface==0 && AlarmFlag)
			{
				g_ResetMessage=TRUE;
				//Write_Alarm_Led(0);
				//DispErrCode(No_Alarm);
				for(;;)
				{
					if(!g_ResetMessage)
					{
						break;
					}
					My_OSTimeDly(50);//��ʱ�ȴ����µ���Ϣˢ��
				}
			}

			if(!AlarmFlag  || (g_SysAlarmNo == ProbeFail || g_SysAlarmNo == ProbeFail1) )//û�б��� || ��̽��ʧ�ܻ�һֱ������״̬��
			{
				if(g_iWorkStatus==Run || g_iWorkStatus==Pause)
				{
					SetMM(17,0,FALSE);
					g_iWorkStatus=Pause;
					SetRange(0);

					if(!g_bMode)
					{
						//g_bTestToSingle=FALSE;
						//g_bTestStart = TRUE;
						g_bMode=TRUE;
						Write_Output_Ele(OUT_SENDADD,0);
					}

					if(g_iWorkStatus==Pause && (g_SysAlarmNo == ProbeFail || g_SysAlarmNo == ProbeFail1))//�������ͣ״̬
					{
						DispErrCode(No_Alarm);//��̽��ʧ�ܵ�����°��²��Լ����Լ�����ҡ�ӹ�
					}

					g_bHandset=FALSE;//�������᷽��Ҫ��,���Կ��ش�,���ֿ��ؾ�һ��Ҫ�ر�.

					if(g_Sysparam.iTapMachineCraft)
					{
						g_bStepRunMode =FALSE;	//����ģʽ�˳�
						g_bStepRunStart =TRUE;	//��������
					}
					MessageBox(EnterTestModePromit);//��ʾ�������ģʽ
				}
				else
				{
					//g_bMode=!g_bMode;//del by yzg 18.10.18,�������᷽��Ҫ��ɾ��������
					if(g_bMode)//����������������ģʽ�ر�
					{
						g_bMode=!g_bMode;
					}
					else//������ģʽ����Ҫ����һЩ���
					{
						//�����ֺ������ᶯ������²ſ��Ը���
						if(g_iWorkStatus!=Manual_HandBox)
						{
							g_bHandset=FALSE;//�������᷽��Ҫ��,���Կ��ش�,���ֿ��ؾ�һ��Ҫ�ر�.
							g_bMode=!g_bMode;
						}
					}

					if(g_Sysparam.iTapMachineCraft)
					{
						g_bStepRunMode =FALSE;	//����ģʽ�˳�
					}

					if(g_bMode)
					{
						MessageBox(EnterTestModePromit);//��ʾ�������ģʽ
						//g_bTestToSingle=FALSE;
					}
					else
					{
						MessageBox(ExitTestModePromit);//�˳�����ģʽ
					}
				}
			}
		}
		//��ֹ�������
		if(g_bPressTestIO && !Read_Input_Func(IN_PAUSE) && key!=TEST_KEY)
		{
			g_bPressTestIO=FALSE;
		}


		//���ΰ�ť  �ڰ������������ú���Ч
		if(!g_bPressStepIO && key==SINGLE_KEY && g_Sysparam.iTapMachineCraft)
		{
			g_bPressStepIO=TRUE;

			//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� add by yzg 19.02.26
			if(!g_Alarm && g_iWorkStatus==Stop && !g_ResetMessage && g_Interface==0 && AlarmFlag)
			{
				g_ResetMessage=TRUE;
				//Write_Alarm_Led(0);
				//DispErrCode(No_Alarm);
				for(;;)
				{
					if(!g_ResetMessage)
					{
						break;
					}
					My_OSTimeDly(50);//��ʱ�ȴ����µ���Ϣˢ��
				}
			}

			if(!AlarmFlag)//û�б���
			{
				if(g_iWorkStatus==Run || g_iWorkStatus==Pause)
				{
					if(g_bStepRunMode)												//�ӹ����мӵĵ���ģ��ֻ�ܴӵ����л����Զ�ģʽ
					{
						g_bStepRunMode	=FALSE;
						MessageBox(ExitSingleModePromit);//�˳�����ģʽ
					}
					else
					{

					}
				}
				else
				{
					if(g_bStepRunMode)
					{
						g_bStepRunMode	=FALSE;
						MessageBox(ExitSingleModePromit);//�˳�����ģʽ
					}
					else
					{
						g_bMode =FALSE;
						g_bStepRunMode	=TRUE;
						MessageBox(EnterSingleModePromit);//���뵥��ģʽ
					}
				}
			}
		}

		//��ֹ���뵥��
		if(g_bPressStepIO && key!=SINGLE_KEY)
		{
			g_bPressStepIO=FALSE;
		}


		//����
		if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//������״̬   ����ͣ״̬
		{
			if (!g_bPressSingleIO && key==STOP_KEY)
			{
				g_bPressSingleIO=TRUE;

				//���ð���������
				if(g_Sysparam.iTapMachineCraft)
				{
					g_bStepRunMode =FALSE;	//����ģʽ�˳�
					g_bStepRunStart =TRUE;	//��������
					if(g_bMode)
					{
						g_bMode=FALSE;
					}
				}

				if(g_bBackZero)
				{
					StopAll();
				}
				else
				//if(!AlarmFlag)
				{
					if(g_bMode)
					{
						//g_bTestToSingle=TRUE;
						MessageBox(TestEndPromit);//��ʾ�������һ��
					}
					else
						MessageBox(WorkEndPromit);//��ʾ�ӹ����һ��

					g_bAutoRunMode = FALSE;

				}

			}

			//��ֹ���뵥��
			if(g_bHandset)//�ֺ�ʹ��
			{
				if(g_bPressSingleIO && !Read_Input_Func(IN_STOP) && !Read_Input_Func(IN_STOP1) && key!=STOP_KEY)
				{
					g_bPressSingleIO=FALSE;
				}
			}
			else
			{
				if(g_bPressSingleIO && !Read_Input_Func(IN_STOP1) && key!=STOP_KEY)
				{
					g_bPressSingleIO=FALSE;
				}
			}
		}


		//*******************���ּӹ�***********************//
		//�ڲ���ģʽ�����ֺ�ʹ����������״̬����ͣ״̬  �Ҳ��ڲ��Լӹ�������
		//if(g_bMode && g_bHandset && !g_bStepIO  && (g_iWorkStatus==Run || g_iWorkStatus==Pause))

		//�������᷽�����Ҫ��,����ģʽ���ֺ��ϵ�����Ҳ���Բ�����ҡ���Լӹ�
		if(g_bMode  && !g_bStepIO  && (g_iWorkStatus==Run || g_iWorkStatus==Pause))
		{

			if(!g_bBackZero &&  !AlarmFlag)//���ڹ������ && ���ڱ���״̬
			{
				Count++;

				if(Count > g_Sysparam.iHandRunSpeed*3)//��ҡ����
				{
					INT32U t2=0;
					INT32S tmp=0;
					static INT32S pos=0;

					Count= 0;

					t2   = OSTimeGet(&os_err);
					tmp  = t2-g_lHandRun;

					if(tmp <= 0)
						tmp = 1;

					g_lHandRun = t2;

					pos = GetPulse(17,FALSE);

					SetMM(17,0,FALSE);

					if(pos>0 )
					{
						FP32 CurSpeedBi=0.01;

						if(g_bHandRun==FALSE)
						{
							g_bHandRun = TRUE;
						}

						pos = pos*50;  										//��CNC820 836����Ӧ������ΪMXC312������һ����1����FPGA��4

						CurSpeedBi=(FP32)(pos)/(tmp*g_Sysparam.fHandRunRate);//��ҡ����

						//ps_debugout1("tmp*g_Sysparam.fHandRunRate=%f  pos=%d \n",tmp*g_Sysparam.fHandRunRate,pos);

						if(CurSpeedBi>1.0)
							CurSpeedBi=1.0;

						if(fabs(g_fSpeedBi-CurSpeedBi)>0.04)
						{
							g_fSpeedBi=CurSpeedBi;
							MessageBox(StartHandPromit);//��ʾ��ʼ��ҡ����
							g_iWorkStatus=Run;//��־��������״̬
							//ps_debugout1("g_fSpeedBi=%f  \n",g_fSpeedBi);
							SetRange((INT32S)(g_fSpeedBi*8000));
						}
						else
						{
							MessageBox(StopHandPromit);//��ʾ��ҡ�ӹ�ֹͣ
						}
					}
					else if(GetRange()!=0)
					{
						g_iWorkStatus=Pause;//������ͣ״̬
						g_fSpeedBi = 0.0;
						//ps_debugout("here set 0\n");
						SetRange(0);

						if(g_bHandRun)
						{
							g_bHandRun = FALSE;
							MessageBox(StopHandPromit);//��ʾ��ҡ�ӹ�ֹͣ

						}

						//g_iRunLine  = vm_getindex()%500-1;
						//ps_debugout("g_iRunLine==%d\r\n",g_iRunLine);
					}
					else
					if(GetRange()==0 && g_bHandRun)
					{
						g_bHandRun=FALSE;
					}
				}//end if(Count>g_Ini.iHandRunSpeed)
			}

		}
		else
		{
			g_bHandRun = FALSE;//��־��ҡ����״̬
		}


		if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//������״̬
		{
			if(!g_bPressStartIO && key==START_KEY)	//��Ļ�ϵ��������������ֳֺ��ϵ�������
			{
				g_bPressStartIO=TRUE;

				//���ޱ���������»� ̽��һֱ������̽��δ��������ߣ����ߣ����߱����Ҷ��ߣ����ߣ����߱����ź�����ʧ  ��������Ч
				if(!AlarmFlag ||(g_SysAlarmNo == WaitInputTimeOut1) || (g_SysAlarmNo == ProbeFail || g_SysAlarmNo == ProbeFail1) || ((g_SysAlarmNo == DUANXIAN_ALARM
				   || g_SysAlarmNo == SONGXIANIOCONST_ALARM || g_SysAlarmNo == PAOXIANDI_ALARM) && !g_Alarm_b.DUANXIANALARM
				   && !g_Alarm_b.SONGXIANIOCONSTALARM && !g_Alarm_b.PAOXIANDIALARM))
				{

					if(g_bMode)
					{
						g_bStepIO=FALSE;
						g_bMode=FALSE;
						g_bHandRun = FALSE;
					}

					if(g_iWorkStatus==Pause)//�������ͣ״̬
					{
						g_iWorkStatus=Run;
						DispErrCode(No_Alarm);
					}

					//���ð���������
					if(g_Sysparam.iTapMachineCraft)
					{
						g_bStepRunStart =TRUE;	//��������
					}

					SetRange(8000);

					//���ð���������
					if(g_Sysparam.iTapMachineCraft)
					{
						if(g_bStepRunMode)
						{
							//g_bDispRunLine = TRUE;
							MessageBox(StartStepWorkPromit);//��ʾ��ʼ�����ӹ�
						}
						else
						{
							//g_bDispRunLine = FALSE;
							MessageBox(StartAutoWorkPromit);//��ʾ��ʼ�Զ��ӹ�
						}
					}
					else
					{
						MessageBox(StartAutoWorkPromit);//��ʾ��ʼ�Զ��ӹ�
					}
					if(AxParam.iAxisSwitch)
					{
						Write_Output_Ele(OUT_SENDADD,g_Sysparam.iXianjiaDrive?1:0);
					}
				}

			}

			//��ֹ��������
			if(g_bHandset)//�ֺ�ʹ��
			{
				if(g_bPressStartIO && !Read_Input_Func(IN_START) && !Read_Input_Func(IN_START1)  && key!=START_KEY)
				{
					g_bPressStartIO=FALSE;
				}
			}
			else
			{
				if(g_bPressStartIO && !Read_Input_Func(IN_START1) && key!=START_KEY)
				{
					g_bPressStartIO=FALSE;
				}
			}
		}





		//************************���ߡ�����*********************************//

		if(g_Sysparam.iIOSendWire==1 && !g_bMode && g_iWorkStatus==Stop)
		{
			INT16S dir =0;
			FP32 Speed=10.0;

			if(!CheckWarning())
			{
				if(g_Sysparam.iMachineCraftType==2)
				{


					if(g_iSpeedBeilv==3)									//����
					{
						Speed = AyParam.lHandSpeed*AyParam.iStepRate;
					}
					else if(g_iSpeedBeilv==2)								//����
					{
						Speed = AyParam.lHandSpeed;
					}
					else													//����
						Speed = AyParam.lHandSpeed * 0.2;


					if(Read_Input_Func(IN_SONGXIAN))
					{

						if(IsFree(2))//ȷ���ᴦ��ֹͣ��״̬
						{
							dir =1;//����

							AddMoveMM(Ay0+1,dir?10000:-10000,1.0,Speed,AyParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_SONGXIAN))
									break;

								DelaySec(1);
							}

							Stop2(2);
						}

					}

					if(Read_Input_Func(IN_TUIXIAN))
					{

						if(IsFree(2))//ȷ���ᴦ��ֹͣ��״̬
						{
							dir =0;//����

							AddMoveMM(Ay0+1,dir?10000:-10000,1.0,Speed,AyParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_TUIXIAN))
									break;

								DelaySec(1);
							}

							Stop2(2);
						}

					}
				}
				else
				{

					if(g_iSpeedBeilv==3)									//����
					{
						Speed = AxParam.lHandSpeed*AxParam.iStepRate;
					}
					else if(g_iSpeedBeilv==2)								//����
					{
						Speed = AxParam.lHandSpeed;
					}
					else													//����
						Speed = AxParam.lHandSpeed * 0.2;


					if(Read_Input_Func(IN_SONGXIAN))
					{

						if(IsFree(1))//ȷ���ᴦ��ֹͣ��״̬
						{
							dir =1;//����

							AddMoveMM(Ax0+1,dir?10000:-10000,1.0,Speed,AxParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_SONGXIAN))
									break;

								DelaySec(1);
							}

							Stop2(1);
						}

					}

					if(Read_Input_Func(IN_TUIXIAN))
					{

						if(IsFree(1))//ȷ���ᴦ��ֹͣ��״̬
						{
							dir =0;//����

							AddMoveMM(Ax0+1,dir?10000:-10000,1.0,Speed,AxParam.lHandSpeed);

							while(TRUE)
							{
								if(!Read_Input_Func(IN_TUIXIAN))
									break;

								DelaySec(1);
							}

							Stop2(1);
						}

					}
				}
			}

		}

		//�ֺн̵��������
		//���ֺ�ʹ�� && ֹͣ״̬ && ��������
		{
			static BOOLEAN TeachKeyMark=0;
			if(g_bHandset && (g_iWorkStatus==Stop) && g_Interface==0)
			{
				if(Read_Input_Func(IN_HANDTEACH) && !TeachKeyMark)//�ֺн̵�
				{
					TeachKeyMark=TRUE;
					//g_TeachKey=FALSE;
				}

				if(TeachKeyMark && !Read_Input_Func(IN_HANDTEACH))
				{
					//g_TeachKey=TRUE;
					g_TeachKey++;
					if(g_TeachKey>=65535)
					{
						g_TeachKey=0;
					}
					TeachKeyMark=FALSE;
				}
			}
			else
			{
				//g_TeachKey=FALSE;
				TeachKeyMark=FALSE;
			}
		}


		//�쳤 �������
		if(g_iWorkStatus==Run || g_iWorkStatus==Pause)//������״̬ ����ͣ״̬
		{
			//������0
			if(g_Sysparam.fReviseValue>0.0001)
			{
				static INT32S s_lRunCount = 0;

				//�쳤��
				if(Read_Input_Func(CheckLongP) && s_lRunCount !=g_Productparam.lRunCount)
				{
					g_fReviseValue+=g_Sysparam.fReviseValue;
					g_bModify =TRUE;
					s_lRunCount =g_Productparam.lRunCount;
				//	ps_debugout("g_fReviseValue=%f\n",g_fReviseValue);
				}
				else //�쳤��
				if(Read_Input_Func(CheckLongN) && s_lRunCount !=g_Productparam.lRunCount)
				{
					g_fReviseValue-=g_Sysparam.fReviseValue;
					g_bModify =TRUE;
					s_lRunCount =g_Productparam.lRunCount;

				}

			}

			//������1
			if(g_Sysparam.fReviseValue1>0.0001)
			{
				static INT32S s_lRunCount1 = 0;

				//�쳤��
				if(Read_Input_Func(CheckLongP1) && s_lRunCount1 !=g_Productparam.lRunCount)
				{
					g_fReviseValue+=g_Sysparam.fReviseValue1;
					g_bModify =TRUE;
					s_lRunCount1 =g_Productparam.lRunCount;
				//	ps_debugout("g_fReviseValue=%f\n",g_fReviseValue);
				}
				else //�쳤��
				if(Read_Input_Func(CheckLongN1) && s_lRunCount1 !=g_Productparam.lRunCount)
				{
					g_fReviseValue-=g_Sysparam.fReviseValue1;
					g_bModify =TRUE;
					s_lRunCount1 =g_Productparam.lRunCount;

				}

			}
		}

		#if 1
		{
			static INT16S uTmpNow  =  0 ;
			static INT16S uTmpLast =  0 ;


			if(g_bHandset && g_iWorkStatus==Stop)//�ֺ�ʹ��
			{
				{

					static INT16S uTmp1=0,uTmp2=0;//�����μ����ѡ״̬

					static int iCount=0;

					int HA_PORTS[4]={IN_HANDMA,IN_HANDMB,IN_HANDMC,IN_HANDMD};//   4 λ2������ָʾ12����
					//int HA_PORTS[4]={56,58,60,62};//   4 λ2������ָʾ12����
					int i;


					if(iCount ==0)
					{
						uTmp1 = 0;

						for(i=0;i<4;i++)
						{
							//if(0==ReadBit(HA_PORTS[i]))
							//if(0==Read_Port_Ele(HA_PORTS[i]))
							if(Read_Input_Func(HA_PORTS[i]))//��ȡ��Ч��ƽ
							{
								uTmp1 = uTmp1|(1<<i);
							}
						}
					}

					iCount++;

					if(iCount>15)//����15��ɨ�� Ϊ�˱��⿪����������λ֮��ʱ��ѡ������һ��
					{
						iCount = 0;

						uTmp2 = 0;

						for(i=0;i<4;i++)
						{
							//if(0==ReadBit(HA_PORTS[i]))
							//if(0==Read_Port_Ele(HA_PORTS[i]))
							if(Read_Input_Func(HA_PORTS[i]))//��ȡ��Ч��ƽ
							{
								uTmp2 = uTmp2|(1<<i);
							}
						}

						if(uTmp2==uTmp1)
							uTmpNow = uTmp2 +1;// 0000�ǵ�һ��

					}

					if(uTmpNow != uTmpLast)
					{
						uTmpLast = uTmpNow;

						switch(uTmpNow)
						{
							case 1:
								if(g_Sysparam.AxisParam[0].iAxisSwitch)
								{
									g_iWorkNumber=1;
								}
								break;

							case 2:
								if(g_Sysparam.AxisParam[1].iAxisSwitch && g_Sysparam.TotalAxisNum>1)
								{
									g_iWorkNumber=2;
								}
								break;
							case 3:
								if(g_Sysparam.AxisParam[2].iAxisSwitch && g_Sysparam.TotalAxisNum>2)
								{
									g_iWorkNumber=3;
								}
								break;

							case 4:
								if(g_Sysparam.AxisParam[3].iAxisSwitch && g_Sysparam.TotalAxisNum>3)
								{
									g_iWorkNumber=4;
								}
								break;

							case 5:
								if(g_Sysparam.AxisParam[4].iAxisSwitch && g_Sysparam.TotalAxisNum>4)
								{
									g_iWorkNumber=5;
								}
								break;

							case 6:
								if(g_Sysparam.AxisParam[5].iAxisSwitch && g_Sysparam.TotalAxisNum>5)
								{
									g_iWorkNumber=6;
								}
								break;

							case 7:
								if(g_Sysparam.AxisParam[6].iAxisSwitch && g_Sysparam.TotalAxisNum>6)
								{
									g_iWorkNumber=7;
								}
								break;

							case 8:
								if(g_Sysparam.AxisParam[7].iAxisSwitch && g_Sysparam.TotalAxisNum>7)
								{
									g_iWorkNumber=8;
								}
								break;

							case 9:
								if(g_Sysparam.AxisParam[8].iAxisSwitch && g_Sysparam.TotalAxisNum>8)
								{
									g_iWorkNumber=9;
								}
								break;

							case 10:
								if(g_Sysparam.AxisParam[9].iAxisSwitch && g_Sysparam.TotalAxisNum>9)
								{
									g_iWorkNumber=10;
								}
								break;

							case 11:
								if(g_Sysparam.AxisParam[10].iAxisSwitch && g_Sysparam.TotalAxisNum>10)
								{
									g_iWorkNumber=11;
								}
								break;

							case 12:
								if(g_Sysparam.AxisParam[11].iAxisSwitch && g_Sysparam.TotalAxisNum>11)
								{
									g_iWorkNumber=12;
								}
								break;

							default:
								break;
						}
						ps_debugout1("g_iWorkNumber==%d\r\n",g_iWorkNumber);
					}

				}

				{// 2λ�����Ʊ�ʾ�ߵ�����
					static INT16S SpeedStatus[2]={-1,-1};
					INT16S IO1,IO2;


					IO1 = Read_Input_Ele(IN_HANDSP1);//����1 //�Թ��ܺŶ�ȡ����ʵ�ʵ�ƽ
					IO2 = Read_Input_Ele(IN_HANDSP2);//����2
					//IO1 = Read_Port_Ele(64);//����1
					//IO2 = Read_Port_Ele(65);//����2

					if(SpeedStatus[0] != IO1 || SpeedStatus[1] != IO2)
					{
						SpeedStatus[0] = IO1;
						SpeedStatus[1] = IO2;

						if(1==IO1 && IO2 == 1)
						{
							g_iSpeedBeilv = 1;//����
						}
						else if(0==IO1 && IO2 == 1)
						{
							g_iSpeedBeilv = 2;//����
						}
						else if(1==IO1 && IO2 == 0)
						{
							g_iSpeedBeilv = 3;//����
						}

						ps_debugout1("g_iSpeedBeilv==%d\r\n",g_iSpeedBeilv);
					}
				}

			}
			else
			{
				uTmpNow  =  0 ;
				uTmpLast =  0 ;
			}
		}
		#endif

#define TEST_BUG 0

// ****************�ֺ������ֶ�����*************************************//

		if(g_bHandset && !g_bMode && (g_iWorkStatus==Stop || g_iWorkStatus== Manual_HandBox || TEST_BUG))//���ֺд��ҷǲ���ģʽ��
		{

			pos = GetPulse(17,FALSE);//17�����ֳֺе�

			pos=pos*(g_Sysparam.iHandWhellDrect==0?1:-1);

			if(labs(pos)>1)
			{
				//ps_debugout1("hand hand\r\n");

				//����ŷ���������߻���߻����߱���
				if(CheckWarning())//�������  һ�����ŷ����������ᶼ���ŷ�����
				{
					SetMM(17,0,FALSE);
					g_bHandset=FALSE;//�ֺй�
					continue ;
				}


				/*if(g_iCurEditNo!=9)//�����༭״̬�����ƶ�����
				{
					MessageBox(EditModePromit);//���ڱ༭״̬�������༭��
					SetMM(17,0,FALSE);
					return;
				}*/

				if(g_iWorkNumber >0 && g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)						//20141128�ڴ綯�ٶȱ༭�¿�������ҡ��
				{
					MessageBox(CurAxisClosedPronmit);//��ǰ���ѹر�
					SetMM(17,0,FALSE);
					continue ;
				}

				if(g_Sysparam.iMachineCraftType==2)//���ܻ�
				{
					if(g_iWorkNumber == 2 && !g_Sysparam.iYDaoXian && pos < 0)						//20141128�ڴ綯�ٶȱ༭�¿�������ҡ��
					{
						MessageBox(DaoXianDisablePromit);//�����Ե���
						SetMM(17,0,FALSE);
						continue ;
					}
				}
				else
				{
					if(g_iWorkNumber == 1 && !g_Sysparam.iYDaoXian && pos < 0)						//20141128�ڴ綯�ٶȱ༭�¿�������ҡ��
					{
						MessageBox(DaoXianDisablePromit);//�����Ե���
						SetMM(17,0,FALSE);
						continue ;
					}
				}

				if (!IsFree(g_iWorkNumber))//�ȴ���ǰ���˶���
				{
					continue ;
				}

				if(g_iWorkNumber<g_Sysparam.TotalAxisNum+1 && g_iWorkNumber > 0)//�Զ�ģʽ�������ƶ�������
				{
					//FP32 	mv = -10000.0;
					//FP32 	pL[4] = {0.5,1.0,2.0,4.0};					//1��ֵΪû���ֺУ�2��ֵΪ���ٵ���3��ֵΪ���ٵ���4��ֵΪ���ٵ���
					FP32 PL;
					INT32S  lTempPos =0;
					FP32	speed1 = 1.0;
					INT32S  speed=1;
					//INT32S  Ypos = 0,Apos = 0;

					//PL
					if(g_iSpeedBeilv==1)//����
					{
						PL=g_Sysparam.fHandRateL;//��ҡ���ٱ���
					}
					else
					if(g_iSpeedBeilv==2)//����
					{
						PL=g_Sysparam.fHandRateM;//��ҡ���ٱ���
					}
					else
					if(g_iSpeedBeilv==3)//����
					{
						//PL=g_Sysparam.fHandRateH;//��ҡ���ٱ���
						//PL=g_Sysparam.fHandRateH;//��ҡ���ٱ���
						PL=g_Sysparam.AxisParam[g_iWorkNumber-1].iStepRate;//
					}
					else
					{
						PL=0.2;//
					}

					g_lHandRun = OSTimeGet(&os_err);

					//speed = pL[g_iSpeedBeilv-1]*g_Sysparam.AxisParam[g_iWorkNumber-1].lHandSpeed*GetUnitPulse(g_iWorkNumber);
					speed1 = PL*g_Sysparam.AxisParam[g_iWorkNumber-1].lHandSpeed*GetUnitPulse(g_iWorkNumber);

					//ps_debugout1("speed=%f\r\n",speed1);

					speed=(INT32S)speed1;

					g_iWorkStatus= Manual_HandBox;

					//����ͻ����ƷǱ깦�ܣ�ת��ʱ���߲�������
					if(g_iWorkNumber==g_Sysparam.iReviseFTLAxisNo && g_Sysparam.iReviseFTLEnable &&
					   g_Sysparam.fReviseFTLMolecular>0 && g_Sysparam.fReviseFTLDenominator>0)
					{
						g_FTLRun=TRUE;//ת�����˶�ʱ���߲�����־
						g_iMoveAxis  = g_iWorkNumber;
						g_bHandMove  = TRUE;
						FP32 Pos0,Pos1;
						if(g_Sysparam.iMachineCraftType==2)//���ܻ�
						{
							Pos0=g_Sysparam.AxisParam[Ay0].ScrewPitch*20;
						}
						else
						{
							Pos0=g_Sysparam.AxisParam[Ax0].ScrewPitch*20;
						}
						Pos1=g_Sysparam.AxisParam[g_iWorkNumber-1].ScrewPitch*20*(g_Sysparam.fReviseFTLDenominator/g_Sysparam.fReviseFTLMolecular);
						if(pos>0)
						{
							if(g_Sysparam.iMachineCraftType==2)//���ܻ�
							{
								ManualMove1(2,g_iWorkNumber,Pos0,Pos1);
							}
							else
							{
								ManualMove1(1,g_iWorkNumber,Pos0,Pos1);
							}
						}
						else
						{
							if(g_Sysparam.iMachineCraftType==2)//���ܻ�
							{
								ManualMove1(2,g_iWorkNumber,-Pos0,-Pos1);
							}
							else
							{
								ManualMove1(1,g_iWorkNumber,-Pos0,-Pos1);
							}
						}
						if(pos<0)//�����˶�
						{
							ClearLimitAlarmMes(g_iWorkNumber,0);
						}
						else
						if(pos>0)
						{
							ClearLimitAlarmMes(g_iWorkNumber,1);
						}
						g_iMoveAxis  = 0;
						g_bHandMove  = FALSE;
						g_FTLRun=FALSE;//ת�����˶�ʱ���߲�����־
					}
					else
					{
						switch(g_iWorkNumber)
						{
							case 1:
								ps_debugout1("g_iWorkNumber==1\r\n");
								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
								{
									break;
								}

								g_iMoveAxis  = 1;
								g_fSpeedBi   = 0.05;
								//g_fSpeedBi   = 1;
								g_bHandMove  = TRUE;

								//���ܻ��ڶ�����
								if(g_Sysparam.iYDaoXian || g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode != 3)
								{
									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))//��鵱ǰ���Ƿ���λ����
									{
										SetMM(17,0,FALSE);

										g_iMoveAxis=0;
										g_bHandMove=FALSE;
										g_iWorkStatus = Stop;
										//return;
										break;
									}

									if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
									{
										lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
										SetPulse(g_iWorkNumber, lTempPos,TRUE);
									}

									MessageBox(HAND_AxisMove);

									ShortlyPmove(g_iWorkNumber,pos,speed);

									if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode != 3)
									{
										GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
									}

									while (1)
									{
										if (IsFree(g_iWorkNumber))
										{
											//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
											if(pos<0)//�����˶�
											{
												ClearLimitAlarmMes(g_iWorkNumber,0);
											}
											else
											if(pos>0)
											{
												ClearLimitAlarmMes(g_iWorkNumber,1);
											}
											break;
										}

										//ps_debugout("wait move\r\n");
										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
											Stop1(g_iWorkNumber);

										DelaySec(1);
									}

									g_iMoveAxis = 0;
									g_bHandMove = FALSE;
									g_iWorkStatus = Stop;
									MessageBox(HAND_AxisMoveStop);
								}
								else
								{

									if(pos>0 )
									{
										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										{
											SetMM(17,0,FALSE);
											g_iMoveAxis=0;
											g_bHandMove=FALSE;
											g_iWorkStatus = Stop;
											//return;
											break;
										}

										MessageBox(HAND_AxisMove);

										ShortlyPmove(g_iWorkNumber,pos,speed);

										while (1)
										{
											if (IsFree(g_iWorkNumber))
											{
												//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
												if(pos<0)//�����˶�
												{
													ClearLimitAlarmMes(g_iWorkNumber,0);
												}
												else
												if(pos>0)
												{
													ClearLimitAlarmMes(g_iWorkNumber,1);
												}
												break;
											}

											//ps_debugout("wait move\r\n");

											if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
												Stop1(g_iWorkNumber);

											DelaySec(1);
										}

										g_bHandMove = FALSE;
										g_iMoveAxis = 0;
										g_iWorkStatus = Stop;
										MessageBox(HAND_AxisMoveStop);

									}
									else
									{
										SetMM(17,0,FALSE);
									}
								}
								break;

							case 2:
								ps_debugout1("g_iWorkNumber==2\r\n");
								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis  = 2;
								g_fSpeedBi   = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove  = TRUE;


								//���ܻ��ڶ�����
								if(g_Sysparam.iYDaoXian || g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode != 3)
								{
									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
									{
										SetMM(17,0,FALSE);

										g_iMoveAxis=0;
										g_bHandMove=FALSE;
										g_iWorkStatus= Stop;

										//return;
										break;
									}



									if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
									{
										lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
										SetPulse(g_iWorkNumber, lTempPos,TRUE);
									}

									MessageBox(HAND_AxisMove);


									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־


									while (1)
									{
										if (IsFree(g_iWorkNumber))
										{
											//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
											if(pos<0)//�����˶�
											{
												ClearLimitAlarmMes(g_iWorkNumber,0);
											}
											else
											if(pos>0)
											{
												ClearLimitAlarmMes(g_iWorkNumber,1);
											}
											break;
										}


										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
											Stop1(g_iWorkNumber);

										DelaySec(1);
									}

									g_iMoveAxis = 0;
									g_bHandMove = FALSE;
									g_iWorkStatus= Stop;
									MessageBox(HAND_AxisMoveStop);
								}
								else
								{


									if(pos>0 )
									{
										if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										{
											SetMM(17,0,FALSE);
											g_iMoveAxis=0;
											g_bHandMove=FALSE;
											g_iWorkStatus = Stop;
											//return;
											break;
										}

										MessageBox(HAND_AxisMove);

										ShortlyPmove(g_iWorkNumber,pos,speed);

										while (1)
										{
											if (IsFree(g_iWorkNumber))
											{
												//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
												if(pos<0)//�����˶�
												{
													ClearLimitAlarmMes(g_iWorkNumber,0);
												}
												else
												if(pos>0)
												{
													ClearLimitAlarmMes(g_iWorkNumber,1);
												}
												break;
											}

											//ps_debugout("wait move\r\n");

											if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
												Stop1(g_iWorkNumber);

											DelaySec(1);
										}

										g_bHandMove = FALSE;
										g_iMoveAxis = 0;
										g_iWorkStatus = Stop;
										MessageBox(HAND_AxisMoveStop);

									}
									else
									{
										SetMM(17,0,FALSE);
									}

								}
								break;

							case 3:
								ps_debugout1("g_iWorkNumber==3\r\n");
								if(g_Sysparam.TotalAxisNum<3)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;


								g_iMoveAxis  = 3;
								g_fSpeedBi   = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove  = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus= Stop;
									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus= Stop;
								MessageBox(HAND_AxisMoveStop);

								break;

							case 4:
								ps_debugout1("g_iWorkNumber==4\r\n");
								if(g_Sysparam.TotalAxisNum<4)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 4;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus= Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);

								break;

							case 5:
								ps_debugout1("g_iWorkNumber==5\r\n");
								if(g_Sysparam.TotalAxisNum<5)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 5;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);

								break;

							case 6:
								ps_debugout1("g_iWorkNumber==6\r\n");
								if(g_Sysparam.TotalAxisNum<6)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 6;
								g_fSpeedBi    = 0.05;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 7:
								ps_debugout1("g_iWorkNumber==7\r\n");
								if(g_Sysparam.TotalAxisNum<7)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 7;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}
								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}

								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 8:
								ps_debugout1("g_iWorkNumber==8\r\n");
								if(g_Sysparam.TotalAxisNum<8)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 8;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;

								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}
								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 9:
								ps_debugout1("g_iWorkNumber==9\r\n");
								if(g_Sysparam.TotalAxisNum<9)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 9;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}
								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 10:
								ps_debugout1("g_iWorkNumber==10\r\n");
								if(g_Sysparam.TotalAxisNum<10)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 10;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 11:
								ps_debugout1("g_iWorkNumber==11\r\n");
								if(g_Sysparam.TotalAxisNum<11)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 11;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							case 12:
								ps_debugout1("g_iWorkNumber==12\r\n");
								if(g_Sysparam.TotalAxisNum<12)
									break;

								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisSwitch==0)
									break;

								g_iMoveAxis   = 12;
								g_fSpeedBi    = 0.05 ;
								//g_fSpeedBi   = 1;
								g_bHandMove   = TRUE;


								if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
								{
									SetMM(17,0,FALSE);

									g_iMoveAxis=0;
									g_bHandMove=FALSE;
									g_iWorkStatus = Stop;

									//return;
									break;
								}


								if(g_Sysparam.AxisParam[g_iWorkNumber-1].iAxisRunMode == 0)
								{
									lTempPos = GetPulse(g_iWorkNumber,TRUE) % g_Sysparam.AxisParam[g_iWorkNumber-1].ElectronicValue;		//�������к����ڶ�Ȧ��ҡʱ�����ö�Ȧ��ҡ��
									SetPulse(g_iWorkNumber, lTempPos,TRUE);
								}

								MessageBox(HAND_AxisMove);

								{
									ShortlyPmove(g_iWorkNumber,pos,speed);
									GoProcessZeroMark[g_iWorkNumber-1]=TRUE;//�ֶ��ƶ���ӹ�ǰ���������־
								}

								while (1)
								{
									if (IsFree(g_iWorkNumber))
									{
										//��ֹͣ״̬����ʵ��û�б���,����ʱ������Ϣ����û����� ,���ᶯʱ���෴�������λ������Ϣ�Զ����add by yzg 18.10.24
										if(pos<0)//�����˶�
										{
											ClearLimitAlarmMes(g_iWorkNumber,0);
										}
										else
										if(pos>0)
										{
											ClearLimitAlarmMes(g_iWorkNumber,1);
										}
										break;
									}


									if(CheckLimit(TRUE,TRUE,g_iWorkNumber,pos))
										Stop1(g_iWorkNumber);

									DelaySec(1);
								}


								g_iMoveAxis = 0;
								g_bHandMove = FALSE;
								g_iWorkStatus = Stop;
								MessageBox(HAND_AxisMoveStop);
								break;

							default:
								break;
						}
					}
					g_iWorkStatus = Stop;
					SetMM(17,0,FALSE);
					//DelaySec(2);

				}
				else
				{
					SetMM(17,0,FALSE);
				}

			}
		}


	}

}

/*
 * ʵʱ���������ٶ�
 */
#define		OS_TICKS_PER_SEC    (1000)
void    Display_RunSpeedCount_Task(void *data)
{

		CPU_TS           p_ts;

		ps_debugout("Display_RunSpeedCount_Task Running\r\n");
		for(;;)
		{
			OSSemPend(&RunSpeedCountSem,0,OS_OPT_PEND_BLOCKING,&p_ts,&os_err);

			//ˢ�������ٶ�
			if(os_err==OS_ERR_NONE)
			{

				if(g_lRunTime!=0)
				{
					if(g_lRunTime/OS_TICKS_PER_SEC > 20)
						g_fRunSpeed=(60.0/((FP32)g_lRunTime/OS_TICKS_PER_SEC));
					else
						g_fRunSpeed=(60.0/((FP32)g_lRunTime/OS_TICKS_PER_SEC))*0.7+g_fRunSpeed*0.3;
				}
				else
					g_fRunSpeed=100.0;

				if(g_fRunSpeed>999.0)
					g_fRunSpeed=999.0;

			}

			//ˢ��ʣ��ʱ��
			//if(g_bAutoRunMode && !g_bMode && g_Sysparam.lDestCount>0 && g_iWorkStatus==Run)
			if(g_bAutoRunMode  && g_Sysparam.lDestCount>0 && g_iWorkStatus==Run)
			{
				if(g_fRunSpeed>=0.01)
				{
					//�õ��ӹ���Ŀ�������ʣ���ʱ��(S)
					g_LeaveTime=(INT32S)(((FP32)(g_Sysparam.lDestCount-g_Productparam.lRunCount))/(g_fRunSpeed/60.0));
				}
			}

		}

}


//�����ֶ����Ƶ���ֹͣ����
void	Check_Hand_Task(void *data)
{

#if 1
	INT16S  	Count=0;
	BOOL    	predir=TRUE;
	BOOL    	curdir=TRUE;
	INT32S 		pos=0;
	INT32S      speed=0;


	CPU_SR_ALLOC();

	ps_debugout("Check_Hand_Task Running\r\n");
	for (;;)
	{
		if((g_iWorkStatus==Manual_HandBox)  && g_bHandMove)//����ҡ״̬������ҡ�����ƶ������״̬��
		{

			if(g_iMoveAxis<17 && g_iMoveAxis!=0)
			{
				//ps_debugout("g_iMoveAxis<17 && g_iMoveAxis!=0\r\n");
				Count++;

				if(Count > g_Sysparam.iHandRunSpeed)
				{

					INT32U t2=0;
					INT32S tmp=0;

					//ps_debugout("Count > g_Sysparam.iHandRunSpeed\r\n");
					Count= 0;
					t2   = OSTimeGet(&os_err);
					tmp  = t2-g_lHandRun;

					if(tmp <= 0)
						tmp = 1;

					g_lHandRun = t2;

					pos = GetPulse(17,FALSE);//�ֳֺ�

					SetMM(17,0,FALSE);

					if(pos>0)
						curdir = TRUE;
					else
						curdir = FALSE;

					if(g_FTLRun)//�㽭����Ǳ궨��ת��ʱ���߲�������
					{
						if(pos!=0)
						{
							if(predir != curdir)
							{
								predir = curdir;

								OS_CRITICAL_ENTER();
								StopAll();
								OS_CRITICAL_EXIT();

							}
						}
						else
						{
							OS_CRITICAL_ENTER();
							StopAll();
							OS_CRITICAL_EXIT();
						}
					}
					else// ������ҡ�����˶�
					{
						if(pos!=0)
						{
							if(predir != curdir)
							{
								predir = curdir;

								OS_CRITICAL_ENTER();
								Stop1(g_iMoveAxis);

								OS_CRITICAL_EXIT();

							}
							else
							{

								FP32 CurSpeedBi;

								if(!IsFree(g_iMoveAxis) && pos!=0)
								{
									pos = pos * 50;  											//�Ŵ�������ʹ����ҡ�ķ�Ӧ��������

									CurSpeedBi = (FP32)(labs(pos))/(tmp*g_Sysparam.fHandRunRate); 	//20061013����֮����

									if(CurSpeedBi > 1.0)
										CurSpeedBi = 1.0;

									if(fabs(g_fSpeedBi-CurSpeedBi) > 0.02)
									{
										g_fSpeedBi = CurSpeedBi;

										speed = (INT32S)((FP32)g_lSpeed*g_fSpeedBi);

										if(speed<2)
											speed =2;

										//ps_debugout("pos=%d  tmp=%d  g_Sysparam.fHandRunRate=%f  CurSpeedBi=%f  g_lSpeed=%f  speed=%d\n",pos,tmp,g_Sysparam.fHandRunRate,CurSpeedBi,g_lSpeed,speed);

										OS_CRITICAL_ENTER();

										SetPulseConSpeed(g_iMoveAxis,speed);

										OS_CRITICAL_EXIT();
									}
								}

							}
						}
						else
						{
							OS_CRITICAL_ENTER();
							Stop1(g_iMoveAxis);
							OS_CRITICAL_EXIT();
						}
					}
				}

			}

		}

		DelaySec(12);


	}

#endif
}



/*
 * ʵʱˢ����������ೣ����λ���豸��Ϣ�����ֺ�״̬ϵͳģʽ��
 */
void    Display_XYZ_Task(void *data)
{

	ps_debugout("Display_XYZ_Task Running\r\n");

	for(;;)
	{
			//�����������
			DisplayXYZ(TRUE);//����ʵʱ����
			UpdateProbeAndCylinder();//����̽���������ʼ8����������״̬+�����ֺм�ͣ���ⲿ��ͣ״̬
			UpdateDCP();//���¶��߲������ߵ����״̬

			//����ҡ�ӹ�ʱ������ʾ����  ���߲�����ť�ӹ�������ʾ����

			//if(g_bMode && (g_bHandset || g_bStepIO) && (g_iWorkStatus==Run || g_iWorkStatus==Pause))
			//�������᷽������,����ģʽ��,�ֺ�����ҡ�ֲ�����ҡ����.
			if((g_bMode || g_bStepRunMode) && (g_iWorkStatus==Run || g_iWorkStatus==Pause))
			{
				g_iRunLine  = vm_getindex()%500-1;
				if(g_iRunLine<2)//����ˢ�µ���һ��ָ��
				{
					g_iRunLine=1;
				}
			}
			else
			{
				g_iRunLine  = -1;//������ʾ�����и���ˢ��
			}

			THSys.RunLine=g_iRunLine;
			//ps_debugout("g_iRunLine==%d\r\n",g_iRunLine);

			//��ʾ��ǰ�ӹ�״̬
			THSys.WorkStatus=g_iWorkStatus;

			//��ʾ�����ٶ�
			THSys.RunSpeed=(INT32S)(g_fRunSpeed+0.5);

			//��ʾʣ��ʱ��  ʱ���Ѿ��������
			THSys.LeaveTime=g_LeaveTime;

			//̽��ʧ�ܴ�����ʾˢ��
			THSys.lProbeFail=g_lProbeFail;

			//�������״̬
			THSys.ResetStatusBit=GetResetStatus();

			//�̵���ֵ
			//THSys.TeachKey=g_TeachKey;

			DelaySec(20);

	}
}


/*
 * ������ť�ӹ�����  ������ ����  ���������������Է�����ť
 */
void    Get_ComData_Task(void *data)
{

	INT16S temp=0;

	ps_debugout("Get_ComData_Task Running\n");

	for(;;)
	{
		//temp = g_Da_Value;
		temp = g_Productparam.Da_Value;

		if(g_bMode && !g_bHandRun && (g_iWorkStatus==Run||g_iWorkStatus==Pause) && !AlarmFlag)
		{

			if(temp<1)
			{
				if(GetRange()!=0)
				{
					SetRange(0);
				}

				if(g_bStepIO == TRUE)
				{
					g_bStepIO = FALSE;
					MessageBox(Test_Stop);//���Լӹ�ֹͣ
					g_iWorkStatus=Pause;//��־��ͣ״̬

				}
			}
			else
			{
				if(temp<=100)
				{
					INT32S AD = (INT32S)(8000*(FP32)temp/100.0);

					if(GetRange()!=AD)
					{
						SetRange(AD);
					}

					g_iWorkStatus=Run;//��־����״̬
					if(g_bStepIO == FALSE)
					{
						g_bStepIO =TRUE;
						MessageBox(Test_Run);//���Լӹ�����
					}
				}
			}

		}

	//	ps_dugout("Get_ComData_Task run!\n");
		DelaySec(5);
	}

}
