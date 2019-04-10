/*
 * THMotion.c
 *
 *  Created on: 2017��9��15��
 *      Author: yzg
 */

#define 	VM_PUBLIC
#include   	"includes.h"
#include 	"os_cpu.h"
#include 	"THMotion.h"
#include 	"VMQUEUE_OP.h"
#include    "ctrlcard.h"
#include    "public2.h"
#include    "math.h"

#define 	Bool 		BOOL
#define 	CALRATE		1000.
#define 	VMAX 		VM.SpdMax

//#define 	SET_RANGE(rate)	{FPGARATE=rate;set_range(rate,0);}
#define 	GET_RANGE()		FPGARATE
#define 	min3(a,b,c)		(min(a,b)<min(b,c)?min(a,b):min(b,c))

int 	vm_setrange1(float rate);

static 		OS_ERR 		os_err;
static		CPU_TS		ts;

static 	int 	eventtag=0;

static 	int 	FIFOCOUNT=0;
static 	TIT 	FIFOBUF[2100];

int 	MethodFlag = -1;

long g_pos=0;

int g_ivm_errNo=0;												//���ڼ�¼VM���й����еĴ������
int g_iStopMode=0;												//ֹͣģʽ��0��ʾ����ֹͣ��1��ʾ��ͣ
//OS_SEM *gMoveStopSEM;
//static CPU_TS          p_ts;
static 		int icurSetindex=0;
static 		int icurRunindex=0;

//int g_Ppos[100][14]={0,0,0};
long g_Pspeed[100]={0,0,0};
//int g_index[100]={0,0,0};

int g_lastpulsereg[MAXAXIS];

int SET_RANGE(double rate)
{
	if(rate>1.001 || rate<-0.000001)
		return FALSE;

	FPGARATE=rate;
	adt_set_rate(0,INPA_AXIS,rate);

	//printf("rate=%f\n",rate);

	return TRUE;
//	set_range(rate,0);
}


int vm_get_errNo()
{
	return g_ivm_errNo;
}


int vm_clear_errNo()
{
	g_ivm_errNo = 0;
	return 1;
}


int	ErrRecord(int No)
{
	SYS_ErrNo=No;
	return 1;
}


int SetMethodFlag(int No)										//0�Ǵ���������1�Ǵ��᲻����
{
	MethodFlag=No;
	return 1;
}


static unsigned int T_POSSETCountPush,T_POSSETCountPop;			//����POSSET���趨����Ч�������������������POSSET����

static int SpeedPretreatment( MP *pData,Queue *q)				//�ٶ�Ԥ����
{
	float 		acc,ad;
	int 		i,j,k,N;
	float 		VectorL1,VectorL2;
	int 		i_temp,i_temp2;
	float 		t,tt;
	static 	MP 	*pLastMP,*pa[200+10];

	ad=VM.aconf[INPA].acc;										//�岹���ٶȣ��˴�Ϊ����

	if(pData)
	{
		t=0;

		for(i=Ax;i<MAXAXIS;i++)									//�����ʱ��t����������Ե���Сʱ��Աȣ�ȥ�������ֵ��
		{
			if(i==pData->maxis)continue;

			tt=2*sqrt(SYS_acc(i)*fabsf(pData->axis[i].relpos))/SYS_acc(i);  //���趨���ٶ������ǼӼ��������г̣�pData->axis[i].relpos����Ҫ��ʱ��

			if(t<tt)t=tt;
		    //���ᰴ����ٶ�����岹�����ʱ��
		    tt=fabsf(pData->axis[i].relpos)/VMAX[i];			//VMAX���������ٶ��������г���Ҫ��ʱ��
		    //�����岹ִ�й��̵�ʱ�䲻��С��tt��ȡtt�����ֵ
		    if(t<tt)t=tt;
		}


		if(pData->vmax>fabsf(pData->relpos)/t)					//��������
			pData->vmax=fabsf(pData->relpos)/t;
	}

	pLastMP=GetMPDataIn(0,q);									//��ȡ����β����

	if(pLastMP==NULL)											//�����ǵ�һ���˶���Ϣ����ֱ�߼Ӽ���
	{
		pData->vs=pData->ve=0;
		goto Suc;
	}
	else
	{
		VectorL1=fabsf(pLastMP->relpos);
	}

	VectorL2=fabsf(pData->relpos);
	//��ʽVtƽ��-V0ƽ��=2as
	pData->v0=pow(pData->vs,2);									//v0=vs*vs
	pData->b=2*ad*VectorL2;										//b=2*a*s
	pData->d=pow(pData->vmax,2);								//d=vmax*vmax

	pData->e=2*pData->d;										//e=2*vmax*vmax
	pData->c=min(pData->e,pData->d);
	pData->ve=0;

	//δ���pLastMP->e=�ĺ���
	if(pLastMP)
	{
		pLastMP->v0=pow(pLastMP->vs,2);
		pLastMP->b=2*ad*VectorL1;
		//�����������б仯�������������з����б仯
		if((pLastMP->maxis!=pData->maxis)||(pLastMP->relpos>0 && pData->relpos<0)||(pLastMP->relpos<0 && pData->relpos>0))
			pLastMP->e=0.;
		else
			pLastMP->e=1000000.;

		pLastMP->d=pow(pLastMP->vmax,2);
		pLastMP->c=min3(pLastMP->e,pLastMP->d,pData->d);		//min(pLastMP->e,pLastMP->d);��ԭ����ֻ���Ǳ��߶κ��¹켣�սǺͱ��߶ε���������ӿ����¶������

	}

	N=NNN-1;													//��ȥ1����ǰ����       NNNΪԤ����ǰհ����30

	pLastMP=GetMPDataIn(N,q);									//ȡ������������� �ĵ�N�������е�����
	//��������в岹����С�ڵ���29����pLastMPΪ�գ�����0��NֵȡQueueCount
	if(!pLastMP)												//���������㹻���߶ν��з���
	{
		//Uart_SendByte('1');
		N=MotionQueue.QueueCount;								//ȡ�������ڵ�һ������
	}
	//��MotionQueue.QueueCount���ڵ���30ʱ��Nֵ����29��
	j=N;
	memset(pa,0x00,sizeof(MP *)*(NNN+1));

	//��QueueCount����29ʱ��i��0��28��j-i-1��ΧΪ28��0����QueueCountС�ڵ���29ʱ��
	for(i=0;i<j;i++)
	{
		pa[i]=GetMPDataIn(j-i-1,q);

		if(!pa[i])goto Err;
	}

	pa[i]=pData;												//pa[]������30���岹���ݣ��Ӷ�ͷ0����βN

	N+=1;														//��ԭʵ�ʵ�N��

	for(i=0;i<N;i++)
	{
		j=0;

		for(;;)
		{
			if(j>=N || (j+i+1)>=N)
			{
				acc=0;

				for(k=i+j;k>=i+1;k--)
				{
					acc+=pa[k]->b;
					if(acc>pa[k-1]->c)acc=pa[k-1]->c;
				}

				i_temp=pa[i]->v0+pa[i]->b;
				i_temp2=min(min(acc,i_temp),min(acc,pa[i]->c));
				pa[i]->ve=sqrt(i_temp2);

				if((i+1)<N)
				{
					pa[i+1]->vs=pa[i]->ve;
					pa[i+1]->v0=pow(pa[i+1]->vs,2);

//					pa[i]->speedv=sqrt((pa[i]->v0+pa[i+1]->v0+pa[i]->b)/2);
//					pa[i]->speedv=min(pa[i]->speedv,pa[i]->vmax);
				}
				else
				{
//					pa[i]->speedv=sqrt((pa[i]->v0+pa[i]->b)/2);
//					pa[i]->speedv=min(pa[i]->speedv,pa[i]->vmax);
				}

				break;
			}

			j++;

			if((pa[i+j]->b > pa[i+j-1]->c))						//�����߶δ��ڹս�
			{
				acc=pa[i+j-1]->c;

				for(k=i+j-1;k>=i+1;k--)
				{
					acc+=pa[k]->b;

					if(acc>pa[k-1]->c)acc=pa[k-1]->c;
				}

				i_temp=pa[i]->v0+pa[i]->b;
				i_temp2=min(min(acc,i_temp),min(acc,pa[i]->c));
				pa[i]->ve=sqrt(i_temp2);

				if((i+1)<N)
				{
					pa[i+1]->vs=pa[i]->ve;
					pa[i+1]->v0=pow(pa[i+1]->vs,2);

//					pa[i]->speedv=sqrt((pa[i]->v0+pa[i+1]->v0+pa[i]->b)/2);
//					pa[i]->speedv=min(pa[i]->speedv,pa[i]->vmax);
				}
				else
				{
//					pa[i]->speedv=sqrt((pa[i]->v0+pa[i]->b)/2);
//					pa[i]->speedv=min(pa[i]->speedv,pa[i]->vmax);
				}

				break;
			}

			acc=0;

			for(k=i+j;k>=i+1;k--)
			{
				acc+=pa[k]->b;

				if(acc>pa[k-1]->c)acc=pa[k-1]->c;
			}

			if(acc>=pa[i]->c)									//�����߶��㹻���μ���
			{
				i_temp=pa[i]->v0+pa[i]->b;
				i_temp2=min(i_temp,pa[i]->c);
				pa[i]->ve=sqrt(i_temp2);

				if((i+1)<N)
				{
					pa[i+1]->vs=pa[i]->ve;
					pa[i+1]->v0=pow(pa[i+1]->vs,2);

//					pa[i]->speedv=sqrt((pa[i]->v0+pa[i+1]->v0+pa[i]->b)/2);
//					pa[i]->speedv=min(pa[i]->speedv,pa[i]->vmax);
				}
				else
				{
//					pa[i]->speedv=sqrt((pa[i]->v0+pa[i]->b)/2);
//					pa[i]->speedv=min(pa[i]->speedv,pa[i]->vmax);
				}

				//if(pa[i]->vc<max(pa[i]->vs,pa[i]->ve))debugout("3pa:%f %f %f %f\r\n",pa[i]->vs,pa[i]->vc,pa[i]->ve,pa[i]->vmax);

				break;
			}
		}
	}

//	debugout("angle:%d %x %d %d %d %d %d %d\n",angle,QNodeHead->MD.MType,pData->pulse[Ax],pData->pulse[Ay],pData->pulse[Az],QNodeHead->MD.vs,QNodeHead->MD.vc,QNodeHead->MD.ve);
//	debugout("angle:%d %x %d %d %d %d %d %d\n",angle,QNodeHead->MD.MType,pData->pulse[Ax],pData->pulse[Ay],pData->pulse[Az],QNodeHead->MD.vs,QNodeHead->MD.vc,QNodeHead->MD.ve);
//	debugout("SpeedPretreatment success\r\n");

Suc:
	return SUCCESS;

Err:
	debugout("SpeedPretreatment Err\r\n");
	return FAIL;
}


static void FollowSpeedPretreatment( Queue *q)
{
  	int 		i,j,N;
 	double 		Vavrc,acc,vs,vc,ve,ftemp,maxpos;				//,MinSpeed
 	float		t1,t3;
 	double 		I;
 	static 	MP 	*Currentp,*Lastp;  								//��ǰָ�����һ��ָ��

	for(i=0;i<=NNN;i++)											//�õ������д����ٶȹ滮���ݸ��������30��
  	{
   		N=i;
  		Currentp=GetMPDataIn(i,  q);
  	    if(Currentp==NULL) break;
  	}

  //Uart_Printf("%d\r\n",N);
  	if(N==0) return;

  	for(j=N-1;j>=0;j--)											//���¼�������ֶ�ʱ��
  	{
		Currentp=GetMPDataIn(j,  q);
		Currentp->speedv=max(max(Currentp->vs,Currentp->ve),max(Currentp->ve,Currentp->vmax));
	 	vs=Currentp->vs;
	 	ve=Currentp->ve;
		maxpos=fabsf(Currentp->relpos);
 		//vc=sqrt((2*SYS_acc(INPA)*SYS_acc(INPA)*pP->maxpos+SYS_acc(INPA)*vs*vs+SYS_acc(INPA)*ve*ve)/(SYS_acc(INPA)+SYS_acc(INPA)));
	 	vc=sqrt((2*SYS_acc(INPA)*maxpos+vs*vs+ve*ve)*0.5);

	 	if(vc<Currentp->speedv)									//����߼��ٵ��������ٶȣ���û�����ٶ�
		{
	 		Currentp->speedv=vc;
	 	}

		vc=Currentp->speedv;

	  	t1=(vc-vs)/SYS_acc(INPA);
		t3=(vc-ve)/SYS_acc(INPA);

		ftemp=maxpos-((vc+vs)*t1+(vc+ve)*t3)/2;
		/*T=t1+t3+ftemp/vc;
		fn=T/0.001;
		n=(int)fn;
		if(fn>(float)n)
		{n=n+1; }
		Currentp->T=n*0.001;*/
		Currentp->T=t1+t3+ftemp/vc;

	 	if(j>=1)
	 		Lastp=GetMPDataIn(j-1,  q);
	 	else
	 		Lastp=NULL;

		for(i=Ax;i<MAXAXIS;i++)
		{
			acc=SYS_acc(i);

			if(i==Currentp->maxis) continue;

			if(fabs(Currentp->axis[i].relpos)>0.0001)
			{
		 		if(Lastp==NULL||(Lastp->axis[i].relpos*Currentp->axis[i].relpos)<=0||Lastp->maxis!=Currentp->maxis)
		 		{
		 			Vavrc=fabsf(Currentp->axis[i].relpos/Currentp->T);
		    		Currentp->axis[i].ve=0;

		    		if(Vavrc>=Currentp->axis[i].vs)
		    		{
		    			I=acc*acc*Currentp->T*Currentp->T+2*acc*Currentp->axis[i].vs*Currentp->T-
					  			 4*acc*fabsf(Currentp->axis[i].relpos)-Currentp->axis[i].vs*Currentp->axis[i].vs;//ok

					  	if(I<0)																	//�趨���ٶȹ�С��Ҫ���µ������ٶ�
				     	{
				        	I=0;
				        	acc=(-(2*Currentp->axis[i].vs*Currentp->T-4*fabsf(Currentp->axis[i].relpos))+sqrt((2*Currentp->axis[i].vs*Currentp->T-4*fabsf(Currentp->axis[i].relpos))*(2*Currentp->axis[i].vs*Currentp->T-4*fabsf(Currentp->axis[i].relpos))+
					 		4*Currentp->axis[i].vs*Currentp->axis[i].vs*Currentp->T*Currentp->T))/(2*Currentp->T*Currentp->T);//ok
				      	}

				 		Currentp->axis[i].vc=((acc*Currentp->T+Currentp->axis[i].vs)-sqrt(I))/2;//ok
				 		//Uart_Printf("11111\r\n");
		    		}
		    		else
		    		{
		    			Currentp->axis[i].vc=(2*acc*fabsf(Currentp->axis[i].relpos)-Currentp->axis[i].vs*Currentp->axis[i].vs)/(2*(acc*Currentp->T-Currentp->axis[i].vs));//�������ٶ�ok

			            if(Currentp->axis[i].vc<=0||(Currentp->axis[i].vs/acc)>Currentp->T)		//��������ٶ�ʱ��Ϊ��ֵ��Ҫ���µ������ٶ�ok
		           		{
			             	Currentp->axis[i].vc=0.5*Vavrc;
			                acc=(Currentp->axis[i].vs*Currentp->axis[i].vs-2*Currentp->axis[i].vc*Currentp->axis[i].vs)/(2*(fabsf(Currentp->axis[i].relpos)-Currentp->axis[i].vc*Currentp->T));//ok
		                }
		    		}

		 		}
		 		else
		 		{
		 		    Vavrc=fabsf(Currentp->axis[i].relpos/Currentp->T);

		 			if(Vavrc>=Currentp->axis[i].vs)
		        	{
		          		I=acc*acc*Currentp->T*Currentp->T+2*acc*Currentp->axis[i].vs*Currentp->T-
			        	2*acc*fabsf(Currentp->axis[i].relpos);//ok

			  		  	if(I<0)																	//�趨���ٶȹ�С��Ҫ���µ������ٶ�
			   			{
				   			I=0;
				   			acc=(2*fabsf(Currentp->axis[i].relpos)-2*Currentp->axis[i].vs*Currentp->T)/(Currentp->T*Currentp->T);//ok
			  		    }

		              	Currentp->axis[i].vc=(acc*Currentp->T+Currentp->axis[i].vs)-sqrt(I); 	//�������ٶ�
		              //Uart_Printf("33333\r\n");
		        	}
		       		else
		        	{
		        	 	I=acc*acc*Currentp->T*Currentp->T-2*acc*Currentp->axis[i].vs*Currentp->T+
		               		2*acc*fabsf(Currentp->axis[i].relpos);//ok

			        	if(I<0)																	//�趨���ٶȹ�С��Ҫ���µ������ٶ�
			         	{
				      		I=0;
				     		acc=(2*Currentp->axis[i].vs*Currentp->T-2*fabsf(Currentp->axis[i].relpos))/(Currentp->T*Currentp->T);//ok
			         	}

			       		Currentp->axis[i].vc=(Currentp->axis[i].vs-acc*Currentp->T)+sqrt(I);	//�������ٶ�ok

			      		if (Currentp->axis[i].vc<=0||(Currentp->axis[i].vs/acc)>Currentp->T)	//��������ٶ�Ϊ��ֵ��Ҫ���µ������ٶ�
			         	{
				     	 	Currentp->axis[i].vc=0.5*Vavrc;										//�趨��С�����ٶ�
				      		acc=(Currentp->axis[i].vs-Currentp->axis[i].vc)*(Currentp->axis[i].vs-Currentp->axis[i].vc)/(2*(fabsf(Currentp->axis[i].relpos)-Currentp->axis[i].vc*Currentp->T));//ok
		        		}
		     			//Uart_Printf("44444\r\n");
		 		  	}

		 		   	Currentp->axis[i].ve=Currentp->axis[i].vc;
		 	       	Lastp->axis[i].vs=Currentp->axis[i].ve;
		 	  	}
			}
		 	else
		 	{
		 		Currentp->axis[i].vs=0;
		    	Currentp->axis[i].vc=0;
		    	Currentp->axis[i].ve=0;
		 	}

		 	//Uart_Printf("%f,%f,%f\r\n",Currentp->axis[i].vs,Currentp->axis[i].vc,Currentp->axis[i].ve);
		 	Currentp->axis[i].len1=fabsf(Currentp->axis[i].vs*Currentp->axis[i].vs-Currentp->axis[i].vc*Currentp->axis[i].vc)/(2*acc);
		 	Currentp->axis[i].ad=Currentp->axis[i].dd=acc;
		 	Currentp->axis[i].t1=fabsf(Currentp->axis[i].vs-Currentp->axis[i].vc)/acc;

		 	if(Currentp->axis[i].vc!=Currentp->axis[i].ve)
		 	{
		 		Currentp->axis[i].t3=fabsf(Currentp->axis[i].vc-Currentp->axis[i].ve)/acc;
		 		Currentp->axis[i].t2=Currentp->T-Currentp->axis[i].t3;
		 		Currentp->axis[i].t3=Currentp->T;
		 		Currentp->axis[i].len2=fabsf(Currentp->axis[i].relpos)-fabsf(Currentp->axis[i].ve*Currentp->axis[i].ve-Currentp->axis[i].vc*Currentp->axis[i].vc)/(2*acc);
		 	}
		 	else
		 	{
		 		Currentp->axis[i].t3=Currentp->axis[i].t2=Currentp->T;
		 		Currentp->axis[i].len2=fabsf(Currentp->axis[i].relpos);
		 	}

		 	/* if(i==2)
	            Uart_Printf("%f,%f,%f\r\n",Currentp->axis[i].vs,Currentp->axis[i].vc,Currentp->axis[i].ve);
	            Uart_Printf("%f,%f,%f\r\n",Currentp->axis[i].t1,Currentp->axis[i].t2,Currentp->axis[i].t3);*/
	        //Uart_Printf("%f,%f,%f\r\n",Currentp->axis[i].t1,Currentp->axis[i].t2,Currentp->axis[i].t3);
		 	 //Uart_Printf("%f,%f,%f\r\n",Currentp->axis[i].vs,Currentp->axis[i].vc,Currentp->axis[i].ve); 	*/
		}
		//Uart_Printf("%f,%f,%f\r\n",Currentp->axis[1].vs,Currentp->axis[1].vc,Currentp->axis[1].ve);
	}
}


int Init_coderfollow_T(MP *pMD,int axis)
{
	double 	t,t1,t2;
	float	vm,len;
	double 	ftemp;
	TFM 	*p;

	p=&pMD->axis[axis];

	p->vs=0;
	p->ve=0;
	p->endcoder=pMD->pos;
	p->startcoder=pMD->pos-pMD->relpos;
	p->ad=SYS_acc(axis);
	p->dd=SYS_acc(axis);

//	p->cps=fabsf(pCurMP->relpos)/pCurMP->T;

	if(pMD->T>0)
		p->cps=fabsf(pMD->relpos)/pMD->T;
	else
		p->cps=pMD->speedv;

	p->vcps=1./p->cps;

//	t=fabsf(p->endcoder-p->startcoder)*p->vcps;

	t=pMD->T;

	len=fabsf(p->relpos);										//���ȼ���ȫ������ֵ����

	//	ps_debugout("%f v:%f\r\n",p->ad*t*t/4,v);

	ftemp=p->ad*p->ad*t*t-4*len*p->ad;

	//	ps_debugout("ftemp:%5.5f ",ftemp);
	//	if((p->ad*t*t/4-len)>0.001)

	if(ftemp<0.001)
	{
		p->dd=p->ad=len/(t*t/4);								//���㳤�ȣ���ı���ٶ�����ӹ�����
		t1=t2=t/2;
		vm=p->ad*t1;
		p->len1=p->len2=len/2;
//		ps_debugout("ad:%f vm:%f\r\n",p->ad,vm);
	}
	else
	{
		ftemp=sqrt(ftemp);
		t1=(p->ad*t-ftemp)*0.5/p->ad;

		//ps_debugout("t1:%5.5f ",t1);
//		if(fabsf(p->ad*t1*t1+p->ad*t1*(t-2*t1)-len)>0.001)

		if((2*t1)>t)
		{
			t1=(p->ad*t+ftemp)*0.5/p->ad;
			//Uart_Printf("@t1:%5.5f ",t1);
		}

//		t1=(-t+sqrt(t*t-4*len/p->ad))/-2;
//		t1=(-t-sqrt(t*t-4*len/p->ad))/-2;						//�ñ��ʽ������������̵�����Ҫע��

		t2=t-t1;
		vm=p->ad*t1;
		p->len1=p->ad*pow(t1,2)/2;
		p->len2=len-p->len1;
//		ps_debugout("vm:%f ad:%f\r\n",vm, p->ad);
	}

//ps_debugout("axis:%d ad:%5.3f vm:%5.3f len:%5.3f t:%5.5f\r\n",axis,p->ad,vm,len,t);
	p->vc=vm;
	p->t1=t1;
	p->t2=t2;
	p->t3=t;
//Err:
	return 0;
}


float Get_follow_pos_T(TFM *p,float t)
{
	#define 	PI2  	9.8696044
	#define 	PI	 	3.1415926

	volatile 	float 	pos;
	volatile 	float 	ftemp;

	pos=0;

    //Uart_Printf("%f,%f,%f,%f\r\n",p->vs,p->vc,p->ve,p->ad);

	if(!p->relpos)goto Err;
	//debugout("t:%5.3f t1:%5.3f %5.3f %5.3f\r\n",t,p->t1,p->t2,p->t3);

	if(t<p->t1)													//����
	{
	  	if(p->vs<=p->vc)
			//pos=p->vs*t+p->ad*t*t/2;
			//pos=p->ad*(t*t*0.5+p->t1*p->t1*0.25/PI2*cos(t/p->t1*2*PI)-p->t1*p->t1*0.25/PI2);
			pos=0.5*(p->vc+p->vs)*t-0.5*(p->vc-p->vs)*sin(t*PI/p->t1)*p->t1/PI;
	 	else
	    	//pos=p->vs*t-p->ad*t*t/2;
	    	//ftemp=p->t1-t;
	   		// pos=fabsf(p->relpos)-p->dd*(ftemp*ftemp*0.5+p->t1*p->t1*0.25/PI2*cos(ftemp/p->t1*2*PI)-p->t1*p->t1*0.25/PI2);
	      	pos=0.5*(p->vc+p->vs)*t+0.5*(p->vs-p->vc)*sin(t*PI/p->t1)*p->t1/PI;
	    	//pos=p->len1-(0.5*(p->vs+p->vc)*(p->t1-t)-0.5*(p->vs-p->vc)*sin((p->t1-t)*PI/(p->t1))*(p->t1)/PI);
			//pos=p->ad*(t*t*0.5+p->t1*p->t1*0.25/PI2*cos(t/p->t1*2*PI)-p->t1*p->t1*0.25/PI2);
	}
	else if(t<p->t2)											//����
	{
		pos=p->len1+p->vc*(t-p->t1);
	}
	else if(t<p->t3)											//����
	{
        ftemp=p->t3-t;
		pos=fabsf(p->relpos)-(0.5*(p->vc+p->ve)*ftemp-0.5*(p->vc-p->ve)*sin(ftemp*PI/(p->t3-p->t2))*(p->t3-p->t2)/PI);
	}
	else														//β���ٶȴ���
	{
		pos=fabsf(p->relpos);
		t=p->t3;
	}

	if(p->relpos<0)pos=-pos;

	/*if(t<=p->t1)												//����
	{
	    if(p->vs<=p->vc)
			pos=p->vs*t+p->ad*t*t/2;
	   else
	    	pos=p->vs*t-p->ad*t*t/2;
	}
	else if(t<p->t2)
	{
	    pos=p->len1+p->vc*(t-p->t1);

	}
	else if(t<p->t3)											//����
	{
		ftemp=t-p->t2;
		pos=p->len2+p->vc*ftemp-p->dd*ftemp*ftemp*0.5;
	}
	else														//β���ٶȴ���
	{
		pos=fabsf(p->relpos);
		t=p->t3;
	}*/

	//ps_debugout("%f,%f,%f\r\n",p->vs,p->vc,p->ve);
Err:
	return pos;

}


//�ú��������ڻ���������������󣬶�ʣ�µ�TAGǰδִ�е��¼�����ִ��
#if 0
static int event_end(void)
{
	EP 		*pEvent;

	for(;;)
	{
		pEvent = GetEPDataOut(0,pEPQ);    						//��ȡ����ͷ����

		//ps_debugout("end pEvent=%d\n",pEvent);

		if(pEvent)
		{
			//ps_debugout("end EvTp=%d\n",pEvent->Type);

			switch(pEvent->Type)
			{
				case _T_GPIO:
					WriteBit(pEvent->Data.Gpio.ioport,pEvent->Data.Gpio.iolevel);
					break;

				case _T_POSSET:
					SetPulse((pEvent->Data.Posset.axis+1),0,TRUE);
					//ps_debugout("SetPulse 0 here\n");
					break;

				case _T_TAGSET:
					{
						CurTag=pEvent->Data.Tagset.tag;
						CurIndex=0;
						if(SYS_State==VM_TAGSTOP)
							return 1;							//ע���ڵ�һ��_T_TAGSET�˳�����Ϊ������3������
						else
							break;								//��ʱ�˳�
					}
			}

			DelQueue(pEPQ);
		}
		else
		{
			return 1;
		}
	}

	return 1;
}
#else
static int event_end(void)
{
	EP 		*pEvent;
	int iaxis;
	for(;;)
	{
		pEvent = GetEPDataOut(0,pEPQ);    						//��ȡ����ͷ����

		if(pEvent)
		{
		//	ps_debugout("EvTp=%d\n",pEvent->Type);

			switch(pEvent->Type)
			{
				case _T_GPIO:
					WriteBit(pEvent->Data.Gpio.ioport,pEvent->Data.Gpio.iolevel);
					break;

				case _T_POSSET:
					//SetPulse((pEvent->Data.Posset.axis+1),0,TRUE);
				//	SetPulse((pEvent->Data.Posset.axis+1),pEvent->Data.Posset.pos,TRUE);

					iaxis=pEvent->Data.Posset.axis;

					BaseSetCommandOffset(iaxis,pEvent->Data.Posset.pos - g_lastpulsereg[iaxis]);

				//	ps_debugout("iaxis=%d,a=%d,b=%d\n",iaxis,pEvent->Data.Posset.pos , g_lastpulsereg[iaxis]);
					break;

				case _T_TAGSET:
					{
						CurTag=pEvent->Data.Tagset.tag;

						CurIndex = 0;//�ӹ�������ʱCurIndex����Ϊ0,(��ǰ��ִ���к�)
						if(SYS_State==VM_TAGSTOP)
							return 1;							//ע���ڵ�һ��_T_TAGSET�˳�����Ϊ������3������
						else
							break;								//��ʱ�˳�
					}
			}

			DelQueue(pEPQ);
		}
		else
		{
			return 1;
		}
	}

	return 1;
}
#endif



static int event_proc(int bOver,MP *pCur)
{
	int 	itemp,i,T_POSSET_AXISMAP;
	EP 		*pEvent;
	INT32U 	index;
	#define INVALLIMIT	15										//�ͺ�������޶�����
	static 	long 		lastpulsereg[INVALLIMIT+1][MAXAXIS];
	long 	ltemp[MAXAXIS];
	int 	T_POSSET_TAG=0;

	if(bOver)
	{
		for(i=0;;i++)
		{
			pEvent = GetEPDataOut(i,pEPQ);    					//��ȡ����ͷ����

			if(!pEvent)break;

			if(!pEvent->PCnt)break;
		}

	//	ps_debugout("@%d @%d %d %d %d\r\n",eventtag,pEvent->Type,pEvent->QueueIndex , mdata[INPA].Info.index.QueueIndex,vm_getqueueindex());

		if(pEvent && pEvent->QueueIndex == pCur->Info.index.QueueIndex)					//��ǰ����Ļ����¼�
		{
			T_POSSET_TAG=0;
			T_POSSET_AXISMAP=0;

			for(;;i++)
			{
				pEvent = GetEPDataOut(i,pEPQ);    				//��ȡ����ͷ����
			//	ps_debugout(" @%d %d %d ",pEvent->Type,pEvent->QueueIndex , mdata[INPA].Info.index.QueueIndex);
			//	ps_debugout("@%d %x %x %x %x \r\n",pCur->Info.index.Index,pEvent, pEvent->QueueIndex, pCur->Info.index.QueueIndex, pEvent->PCnt);
				if(!pEvent || pEvent->QueueIndex != pCur->Info.index.QueueIndex || pEvent->PCnt)break;

				pEvent->PCnt=1;

				switch(pEvent->Type)
				{
					case _T_POSSET:
						itemp=pEvent->Data.Posset.axis;

						if(T_POSSET_AXISMAP & (0x01<<itemp))	//���һ������ͬһ���˶��󱻸�λ��Σ���ȡ���ĸ�λֵ
						{

						}
						else
						{
						//	ps_debugout("lastpulse:%d\r\n",lastpulse[itemp]);
							//lastpulsereg[T_POSSETCountPush&0x0F][itemp]=lastpulse[itemp];
							g_lastpulsereg[itemp]=lastpulsereg[T_POSSETCountPush&0x0F][itemp]=lastpulse[itemp];
							T_POSSET_AXISMAP |= (0x01<<itemp);
						}

						lastpulse[itemp]=pEvent->Data.Posset.pos;
						T_POSSET_TAG=1;
						//ps_debugout("@&&%d %d 0x%x %d %d\r\n",T_POSSETCountPush&0x07,itemp,T_POSSET_AXISMAP,lastpulse[itemp],lastpulsereg[T_POSSETCountPush&0x07][itemp]);
						break;

					case _T_TAGSET:
						CurTag=pEvent->Data.Tagset.tag;

						if(SYS_State==VM_TAGSTOPCMD)
							SYS_State=VM_TAGSTOP;
						//	ps_debugout("CurTag:%d\r\n",CurTag);
						break;
				}
			}

			if(T_POSSET_TAG)
			{
				T_POSSETCountPush++;
			}
		}
	}

	T_POSSET_TAG=0;

	pEvent 	= GetEPDataOut(0,pEPQ);    							//��ȡ����ͷ����
	index	= vm_getqueueindex();

	if(pEvent)
	{
		/*
		{
			static int tag;

			if(tag!=index)
			{
			//	if(pEvent->QueueIndex<index)
				ps_debugout("&&%d %d %d\r\n",eventtag,pEvent->QueueIndex,index);
				tag=index;
			}
		}
		*/

//		if((eventtag && pEvent->QueueIndex != index) || pEvent->QueueIndex==0)		//ʵʱ����Ļ����¼�
		if((eventtag && ((((int)pEvent->QueueIndex-(int)index)<0 && (index-pEvent->QueueIndex)<INVALLIMIT) ||((MAXMPCOUNT-pEvent->QueueIndex)<3 && index<3)))
			|| pEvent->QueueIndex==0)												//ʵʱ����Ļ����¼�,Ϊ��ִֹ�й���©��һЩ����
		{
			//ps_debugout("&&... %d %d %d\r\n",eventtag,pEvent->QueueIndex,index);
			T_POSSET_TAG=0;
			memset(ltemp,0x00,sizeof(ltemp));

			for(;;)
			{
				pEvent = GetEPDataOut(0,pEPQ);    				//��ȡ����ͷ����

				if(pEvent && (pEvent->QueueIndex == eventtag))
				{
				//	ps_debugout("&&--- %d %d %d\r\n",eventtag,pEvent->QueueIndex,pEvent->Type);

					switch(pEvent->Type)
					{
						case _T_GPIO:
							WriteBit(pEvent->Data.Gpio.ioport,pEvent->Data.Gpio.iolevel);
							break;

						case _T_PROBE:
						//	ps_debugout("probe %d \r\n",pEvent->Data.Gpio.ioport);
							OpenProbe(0,pEvent->Data.Probe.pro,pEvent->Data.Probe.port);
							break;

						case _T_POSSET:
							itemp=pEvent->Data.Posset.axis;
						//	Uart_Printf("AA=%d %ld\r\n",pEvent->Data.Posset.pos,lastpulsereg[T_POSSETCountPop&0x07][itemp]);
							ltemp[itemp]=pEvent->Data.Posset.pos-lastpulsereg[T_POSSETCountPop&0x0f][itemp];
							T_POSSET_TAG=1;
						//	BaseSetCommandOffset(itemp,pEvent->Data.Posset.pos-lastpulsereg[T_POSSETCountPop&0x07][itemp]);
						//	BaseSetCommandOffset(itemp,pEvent->Data.Posset.pos-lastpulse[itemp]);
						//	lastpulse[itemp]=pEvent->Data.Posset.pos;
						//	Uart_Printf("#%d %d %d\r\n",itemp,lastpulse[itemp],lastpulsereg[itemp]);
							break;

						case _T_TAGSET:
						//	CurTag=pEvent->Data.Tagset.tag;
						//	if(SYS_State==VM_TAGSTOPCMD)
						//		SYS_State=VM_TAGSTOP;
						//	Uart_Printf("CurTag:%d\r\n",CurTag);
							break;
					}

					DelQueue(pEPQ);
					continue;
				}

				break;
			}

			if(T_POSSET_TAG)
			{
				//Uart_Printf("\t&&%d %d %d %d \r\n",ltemp[Ax],ltemp[Ay],ltemp[Az],ltemp[Aa]);
				BaseSetCommandOffset(Ax,ltemp[Ax]);
				BaseSetCommandOffset(Ay,ltemp[Ay]);
				BaseSetCommandOffset(Az,ltemp[Az]);
				BaseSetCommandOffset(Aa,ltemp[Aa]);
				BaseSetCommandOffset(Ab,ltemp[Ab]);
				BaseSetCommandOffset(Ac,ltemp[Ac]);

				BaseSetCommandOffset(A7,ltemp[A7]);
				BaseSetCommandOffset(A8,ltemp[A8]);
				BaseSetCommandOffset(A9,ltemp[A9]);
				BaseSetCommandOffset(A10,ltemp[A10]);
				BaseSetCommandOffset(A11,ltemp[A11]);
				BaseSetCommandOffset(A12,ltemp[A12]);
				BaseSetCommandOffset(A13,ltemp[A13]);
				BaseSetCommandOffset(A14,ltemp[A14]);

				BaseSetCommandOffset(A15,ltemp[A15]);
				BaseSetCommandOffset(A16,ltemp[A16]);



				//Uart_Printf("\t&&%d %d %d %d \r\n",ltemp[Ax],ltemp[Ay],ltemp[Az],ltemp[Aa]);

				T_POSSETCountPop++;
			//	Uart_Printf("@@@%x %x %x\r\n",index,eventtag,pEvent->QueueIndex);
			//	Uart_Printf("%d Pop=%d,push=%d\r\n",pCur->Info.index.Index,T_POSSETCountPop,T_POSSETCountPush);

				if(T_POSSETCountPop>T_POSSETCountPush || abs(T_POSSETCountPop-T_POSSETCountPush)>=INVALLIMIT)
				{
					ps_debugout("T_POSSETCountPush err\r\n");
					return FAIL;
				}
			}
		}

	//	if(pEvent->QueueIndex <= index)
	//	Uart_Printf("$$%d %d %d\r\n",eventtag,index,pEvent->QueueIndex);
	//	if(((int)pEvent->QueueIndex-(int)index)<=0 && (index-pEvent->QueueIndex)<INVALLIMIT)

		if((((int)pEvent->QueueIndex-(int)index)<=0 && (index-pEvent->QueueIndex)<INVALLIMIT)
			||((MAXMPCOUNT-pEvent->QueueIndex)<3 && index<3))
		{
			//if(eventtag!=index)
			//Uart_Printf("\t$$%d %d %d\r\n",eventtag,index,pEvent->QueueIndex);
			eventtag=pEvent->QueueIndex;
		}
		else
			eventtag=0;
	}

	return SUCCESS;
}


static int inp_t_move(int inpg,MP *p,MP *pNextMP,T_PreParamPack *pP)
{
	float 		pos;
	int 		i,iFlag_temp;
	float 		vs,vc,ve;
	float 		ftemp;
	int 		itemp,temp[MAXAXIS];
	int 		ErrNo;

	//unsigned 	short 	sta;
	//OS_ERR 		os_err;


//	Uart_Printf("inp_t_move\n");
//	switch(p->Info.bDataTag)
	{
//		case 0:
			pP->maxpos=fabsf(p->relpos);

			if(pP->maxpos<0.001)
			{
				p->T=0;
				goto Suc;
			}

			pP->axismap=0;

////////////////////////////////////////////

			p->speedv=max(max(p->vs,p->ve),max(p->ve,p->vmax));
			vs=p->vs;
			ve=p->ve;
//			vc=sqrt((2*SYS_acc(INPA)*SYS_acc(INPA)*pP->maxpos+SYS_acc(INPA)*vs*vs+SYS_acc(INPA)*ve*ve)/(SYS_acc(INPA)+SYS_acc(INPA)));
			vc=sqrt((2*SYS_acc(INPA)*pP->maxpos+vs*vs+ve*ve)*0.5);//vc��ʾֻ�м��ٺͼ��ٹ��̣�û�����ٹ��̵��ٽ�����ٶ�

			if(vc<p->speedv)									//����߼��ٵ��������ٶȣ���û�����ٶ�
				p->speedv=vc;
			else
				vc=p->speedv;

			pP->t1=(vc-vs)/SYS_acc(INPA);						//���ٹ���ʱ��
			pP->t3=(vc-ve)/SYS_acc(INPA);						//���ٹ���ʱ��
			ftemp=pP->maxpos-((vc+vs)*pP->t1+(vc+ve)*pP->t3)/2;	//���ٹ���λ��
			p->T=pP->t1+pP->t3+ftemp/vc;						//��ʱ��

////////////////////////////////////////////

			pP->vs=p->vs;
			pP->ve=p->ve;
			pP->vc=p->speedv;
			pP->acc=SYS_acc(inpg);
			pP->dec=SYS_acc(inpg);

//			pP->t1=(pP->vc-pP->vs)/SYS_acc(INPA);
//			pP->t3=(pP->vc-pP->ve)/SYS_acc(INPA);

			pP->L1=(pP->vc+pP->vs)*pP->t1/2;					//���ٹ���λ��
			pP->L3=(pP->vc+pP->ve)*pP->t3/2;
			pP->L3=(pP->vc+pP->ve)*pP->t3/2;					//���ٹ���λ��
			pP->L2=pP->maxpos-pP->L1-pP->L3;					//���ٹ���λ��

			if(pP->L2<0.001)									//������ٶε����峤��Ϊ0
			{
				if(pP->L1>pP->L3)
					pP->L1=pP->maxpos-pP->L3;
				else
					pP->L3=pP->maxpos-pP->L1;

				pP->L2=0;
			}

		//	pP->axismap=0x07;
		//	debugout("%d %f %f %f\r\n",p->maxis,pP->regpos[p->maxis],p->pos,p->relpos);

		//	if(pP->t1>0.001)
		//		pP->acc=(pP->vc-pP->vs)/(pP->t1);

		//	if(pP->t3>0.001)
		//		pP->dec=(pP->vc-pP->ve)/(pP->t3);

			pP->t2=pP->L2/pP->vc;								//���ٶ����е����ʱ��
			pP->t2=pP->t1+pP->t2;								//ִ�������ٶ�ʱ���
			pP->t3=pP->t2+pP->t3;								//ִ�����������̵�ʱ���

			pP->time=pP->t=0;
			p->Info.bDataTag=1;
			p->T=pP->t3;										//�����������̺�ʱ

		//	Uart_Printf("%f,%f,%f\r\n",p->axis[1].vs,p->axis[1].vc,p->axis[1].ve);
		//	Uart_Printf("inp vs:%f vc:%f ve:%f t:%5.3f pos:%f acc:%5.3f dec:%5.3f\r\n",pP->vs,pP->vc,pP->ve,p->T,pP->maxpos,pP->acc,pP->dec);
		//	debugout("G01X%3.3f Y%3.3f Z%3.3f\r\n",pP->SYS_RMacPos[Ax]+p->relpos[Ax],pP->SYS_RMacPos[Ay]+p->relpos[Ay],pP->SYS_RMacPos[Az]+p->relpos[Az]);

			for(i=Ax;i<MAXAXIS;i++)
			{
				if(i!=p->maxis)
				{
					if(fabsf(p->axis[i].relpos)>0.001)
						pP->axismap|=(0x01<<i);

					pP->lastpos[i]=pP->regpos[i]=p->axis[i].pos-p->axis[i].relpos;

				/*
					p->axis[i].cps=pP->vc;
					p->axis[i].vs=0;
					p->axis[i].ve=0;
					p->axis[i].endcoder=p->pos;
					p->axis[i].startcoder=p->pos-p->relpos;
					p->axis[i].ad=SYS_acc(i);
					p->axis[i].dd=SYS_acc(i);
					*/

				    if(MethodFlag==1)
                    {
						Init_coderfollow_T(p,i);
					}
				}
				else
				{
					pP->lastpos[i]=pP->regpos[i]=p->pos-p->relpos;

					if(fabsf(p->relpos)>0.001)
						pP->axismap|=(0x01<<i);
				}

				/*if(i==1)
				{
               	 	Uart_Printf("%f,%f,%f %f,%f,%f\r\n",p->axis[i].vs,p->axis[i].vc,p->axis[i].ve,p->axis[i].ad,p->axis[i].relpos,p->T);
                	//Uart_Printf("%f,%f,%f\r\n",p->axis[i].t1,p->axis[i].t2,p->axis[i].t3);
                }*/
			}
//			break;

//		case 1:
			pos=0;
			iFlag_temp=0;

			//debugout("test1\n");

			/*
			for(i=Ax;i<MAXAXIS;i++)
			{
			 	if(i==p->maxis) continue;
			 	if(i==1)
			 	Uart_Printf("%f,%f,%f\r\n",p->axis[i].vs,p->axis[i].vc,p->axis[i].ve);
		    }
		    */

		    // Uart_Printf("%f\r\n",p->axis[2].relpos);
            // Uart_Printf("%f,%f,%f\r\n",p->axis[2].vs,p->axis[2].vc,p->axis[2].ve);
            // Uart_Printf("%f,%f,%f\r\n",p->axis[2].t1,p->axis[2].t2,p->axis[2].t3);

			//pP->t1��pP->t2��pP->t3�ֱ�Ϊ���ٽ���ʱ��㡢���ٽ���ʱ��㡢���ٽ���ʱ��㣨��������ʱ��㣩
		 	for(;;)
			{
		 		pP->t+=dt;

				if(pP->t<pP->t1)								//����
				{
					pos=pP->vs*pP->t+pP->acc*pP->t*pP->t/2;
				//	Uart_Printf("@1 %5.5f ",pP->t);
		 		}
				else if(pP->t<pP->t2)							//����
				{
				//	pP->t=pP->t2;
				//	pos=pP->L1+pP->L2;
					pos=pP->L1+pP->vc*(pP->t-pP->t1);
				//	Uart_Printf("@2 %5.5f ",pP->t);
		 		}
				else if(pP->t<pP->t3)							//����
				{
					ftemp=pP->t-pP->t2;
					pos=pP->L1+pP->L2+pP->vc*ftemp-pP->dec*ftemp*ftemp/2;
				//	Uart_Printf("@3 %5.5f ",pP->t);
		 		}
				else											//β���ٶȴ���
				{
					pos=pP->maxpos;
					pP->t=pP->t3;
					iFlag_temp=1;
				//	Uart_Printf("@4 %5.5f ",pP->t);
				}

				if(p->relpos<0)pos=-pos;

				pP->descpos[p->maxis]=pos+pP->regpos[p->maxis];

				//Uart_Printf("%f\r\n",pP->descpos[p->maxis]);

				for(i=Ax;i<MAXAXIS;i++)
				{
					if(i!=p->maxis)
					{
					//	pP->descpos[i]=Get_follow_pos_T(&(p->axis[i]),pP->descpos[p->maxis])+pP->regpos[i];
						pP->descpos[i]=Get_follow_pos_T(&(p->axis[i]),pP->t)+pP->regpos[i];

					//	debugout("%d@%f %f\r\n",i,pP->descpos[i],pP->regpos[i]);
					}
				}

				ftemp=pP->t-pP->time;

				if(ftemp>=0.000002)
				{
					//debugout("G01X%fY%fZ%f(%x)\r\n",pP->descpos[Ax],pP->descpos[Ay],pP->descpos[Az],pP->axismap);
					//if(ErrNo=inpmove(pP,pP->axismap,ftemp,pP->lastpos,pP->descpos))goto Err;
					//debugout("time:%f:%f %f %f\r\n",ftemp,pP->descpos[p->maxis]-pP->lastpos[p->maxis],pos,(pP->descpos[p->maxis]-pP->lastpos[p->maxis])/ftemp);

					temp[Ax]=(int)(pP->descpos[Ax]*CALRATE)-lastpulse[Ax];
					temp[Ay]=(int)(pP->descpos[Ay]*CALRATE)-lastpulse[Ay];
					temp[Az]=(int)(pP->descpos[Az]*CALRATE)-lastpulse[Az];
					temp[Aa]=(int)(pP->descpos[Aa]*CALRATE)-lastpulse[Aa];
					temp[Ab]=(int)(pP->descpos[Ab]*CALRATE)-lastpulse[Ab];
					temp[Ac]=(int)(pP->descpos[Ac]*CALRATE)-lastpulse[Ac];

					temp[A7]=(int)(pP->descpos[A7]*CALRATE)-lastpulse[A7];
					temp[A8]=(int)(pP->descpos[A8]*CALRATE)-lastpulse[A8];
					temp[A9]=(int)(pP->descpos[A9]*CALRATE)-lastpulse[A9];
					temp[A10]=(int)(pP->descpos[A10]*CALRATE)-lastpulse[A10];
					temp[A11]=(int)(pP->descpos[A11]*CALRATE)-lastpulse[A11];
					temp[A12]=(int)(pP->descpos[A12]*CALRATE)-lastpulse[A12];
					temp[A13]=(int)(pP->descpos[A13]*CALRATE)-lastpulse[A13];
					temp[A14]=(int)(pP->descpos[A14]*CALRATE)-lastpulse[A14];
					temp[A15]=(int)(pP->descpos[A15]*CALRATE)-lastpulse[A15];
					temp[A16]=(int)(pP->descpos[A16]*CALRATE)-lastpulse[A16];
					//a+=temp[Ax];
					//Uart_Printf("%f\r\n",pP->descpos[Ax]);
                    //Uart_Printf("%f,%f\r\n",pP->descpos[Az],temp[Az]/ftemp);
					//Uart_Printf("^^%d %d\r\n",lastpulse[Az],p->Info.index.QueueIndex);
					//Uart_Printf("iFlag_temp=%d,pP->descpos[Ax]=%f,lastpulse[Ax]=%d,temp[Ax]=%d\n",iFlag_temp,pP->descpos[Ax],lastpulse[Ax],temp[Ax]);
					{
						/*
						int itemp1,itemp2;

						itemp=max( max(abs(temp[Ax]),abs(temp[Ay])), max( max(abs(temp[Az]),abs(temp[Aa])),max(abs(temp[Ab]),abs(temp[Ac])) ) );

						itemp1=max(max(abs(temp[A7]),abs(temp[A8])),max(abs(temp[A9]),abs(temp[A10])));

						itemp2=max(max(abs(temp[A11]),abs(temp[A12])),max(abs(temp[A13]),abs(temp[A14])));

						itemp1=max(itemp1,itemp2);
						itemp =max(itemp1,itemp);

						*/

						int i =0;
						itemp = abs(temp[Ax]);//��itempΪ ����ľ���ֵ���ֵ

						for(i=0;i<MAXAXIS;i++)
						{
							if(abs(temp[i])>itemp)
							{
								itemp = abs(temp[i]);
							}
						}
					}

					lastpulse[Ax]+=temp[Ax];
					lastpulse[Ay]+=temp[Ay];
					lastpulse[Az]+=temp[Az];
					lastpulse[Aa]+=temp[Aa];
					lastpulse[Ab]+=temp[Ab];
					lastpulse[Ac]+=temp[Ac];

					lastpulse[A7]+=temp[A7];
					lastpulse[A8]+=temp[A8];
					lastpulse[A9]+=temp[A9];
					lastpulse[A10]+=temp[A10];
					lastpulse[A11]+=temp[A11];
					lastpulse[A12]+=temp[A12];
					lastpulse[A13]+=temp[A13];
					lastpulse[A14]+=temp[A14];
					lastpulse[A15]+=temp[A15];
					lastpulse[A16]+=temp[A16];

					/*
					lastpulse[Ax]=(int)(lastpulse[Ax]+temp[Ax]);
					lastpulse[Ay]=(int)(lastpulse[Ay]+temp[Ay]);
					lastpulse[Az]=(int)(lastpulse[Az]+temp[Az]);
					lastpulse[Aa]=(int)(lastpulse[Aa]+temp[Aa]);
					*/

/******************************************************/
loop:
					if(SYS_State==VM_STOP)						//�쳣�˳�
					{
						ErrNo=_ERR_AXISSTOP;
						goto Err;
					}

					//read_fifo(&sta);
					//if((sta&0x7ff)>(6*10))
					//if((sta&0x7ff)>(8*10))					//fifo_inp_move6 1������ռ��8���ռ�

					//����icurSetindex=icurSetindex&0x7fffffff;��icurSetindex
					if(get_left_index()>10 )						//��ǰ�����Ϊ10��
					{
						DelaySec(1);
						goto loop;
					}

					if(itemp/ftemp > 500000)					//���Ʒ���Ƶ����500K�ڣ������쳣����
					{
						ErrNo=_ERR_SPEEDERR;
						g_ivm_errNo=1;
						ps_debugout("speed too max:%d %5.5f\r\n",itemp,ftemp);
						SYS_State=VM_STOP;
						goto Err;
					}

/******************************************************/
					event_proc(iFlag_temp,p);					//�����ص����ڵ����һ��ָ��ʱ��iFlag_temp��ֵ1�����Ϊ����ֹͣ����������set_tag_stop������SYS_state=VM_TAGSTOPCMDת��ΪSYS_state=VM_TAGSTOP
					vm_setindex(p->Info.index);					//����ϸ�ֵĲ岹����FIFOBUF[(FIFOCOUNT-count)&0x7ff]��ֵΪp->Info.index����ô�����������еĲ岹������vm_setindex��ȡ��ֵҲ��p->Info.index
				//	fifo_inp_move4(temp[Ax],temp[Ay],temp[Az],temp[Aa],4*itemp/ftemp);
				//	fifo_inp_move6(temp[Ax],temp[Ay],temp[Az],temp[Aa],temp[Ab],temp[Ac],4*itemp/ftemp);
					//fifo_inp_move6_axis(INPA_AXIS,icurSetindex,temp[Ax],temp[Ay],temp[Az],temp[Aa],temp[Ab],temp[Ac],itemp/ftemp);

					if(itemp!=0)
					{
						int RES;
						RES = fifo_inp_move16_axis(INPA_AXIS,++icurSetindex,temp[Ax],temp[Ay],temp[Az],temp[Aa],temp[Ab],temp[Ac],temp[A7],temp[A8],temp[A9],temp[A10],temp[A11],temp[A12],temp[A13],temp[A14],temp[A15],temp[A16],itemp/ftemp);

						if(RES != 0)
							printf("ERR fifo_inp_move14_axis=%d\n",RES);

					}
					/*
					ps_debugout("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",\
					icount++,\
					lastpulse[Ax],(int)((temp[Ax])/ftemp),\
					lastpulse[Ay],(int)((temp[Ay])/ftemp),\
					lastpulse[Az],(int)((temp[Az])/ftemp),\
					lastpulse[Aa],(int)((temp[Aa])/ftemp));
					*/

				//	memcpy(pP->lastpos,pP->descpos,sizeof(pP->lastpos));
				//	pP->lastpos[p->maxis]=pP->descpos[p->maxis];
					pP->time=pP->t;
				}
				else
				{
					event_proc(iFlag_temp,p);
				}

				if(iFlag_temp)//��������������forѭ������ʱ����Ļ���������ִ�еĻ������10��
				{
					p->Info.bDataTag=2;
					break;
				}

		 	} //end for(;;) ��ʱ��Ϊ��λ���в��

		//	ps_debugout("test2\n");
		//	break;
	}



Suc://������Ȼ�������,���絥��״̬��suc����
	return _ERR_NO_ERR;

Err:
	ps_debugout("inp move error\r\n");						//��ֹ
	return ErrNo;
}


OS_SEM  VM_START;
OS_SEM  VM_PV;												//VM�����ź���
OS_SEM  QUEUE_PV;											//Queue�����ź���


void vm_motion_task(void *a)
{
	static 	MP 	*pLastMP;
	int 		ErrNo;
	//INT8U 		err;

	EH.EHCount	=0;

	OSSemCreate (&VM_START,"start_sem",0,&os_err);
	OSSemCreate (&VM_PV,"push_sen",1,&os_err);
	OSSemCreate (&QUEUE_PV,"qpush_sen",1,&os_err);

	vm_setrange1(1);
	vm_setrange2(8000);

	for(;;)
	{
		if(!Get_QueueCount(pMQ))								//���ϵͳ�Ļ���岹����Ϊ0
		{
			//���빦�ܵ��ô���������ִ��̽���У����巢���꣬����û��ִ���꣬Ȼ���ȡ����岹����Ϊ�գ�����ִ�е��˴���while������Ҫ���㣬����ȴ��û��������ִ�����
			while(vm_get_status())								//���ڽ���ʱ�ȴ����淢��
			{
				DelaySec(1);

			//	if(FPGARATE<0.02)
			//		SET_RANGE(0.02);

				if(SYS_State==VM_STOP)
				{
					//int 	status;
					//INT16S	i;

					StopAll();

					vm_getindex();										//�ڼ�ͣVM_STOPʱ��getindex��λ�����ִ�е�λ��


					for(;;)												//���Ӳ岹״̬ȷ�ϣ��������һ��������Ϊ�������⵼���޷��˳�
					{
						SET_RANGE(1.0);

						//adt_get_status_all(&status);
						if(IsInpEnd(1))break;
						DelaySec(1);
					}

					InitQueue(pMQ);
					InitQueue(pEPQ);
					mdata[INPA].Info.bDataTag=3;
					eventtag=0;
					vm_setrange1(1);

					icurSetindex=icurRunindex;

				}

			}


			event_end();										//�����淢�����¼�

			InitQueue(pEPQ);

			//ps_debugout("InitQueue\r\n");

			T_POSSETCountPush=0;
			T_POSSETCountPop=0;
			SYS_State=VM_IDLE;

			OSSemSet (&VM_START,0,&os_err);

			ps_debugout("motion over\r\n");

			OSSemPend(&VM_START,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

			ps_debugout("pend start\r\n");

			g_pos=0;

			if(SYS_State!=VM_STOP)
				SYS_State=VM_RUN;
			else
				printf("@err1\r\n");

			eventtag=0;
			CurQueueIndex=0;									//�����µĻ�������ʱ������岹��index��Ϊ0
			memset(FIFOBUF,0x00,sizeof(FIFOBUF));
		}

		if(Get_QueueCount(pMQ))
		{
			//ps_debugout("Get_QueueCount(pMQ)=%d\n",Get_QueueCount(pMQ));

		//	if(mdata[INPA].Info.bDataTag!=1)
			{
				if(!DeQueue(sizeof(MP),&mdata[INPA],pMQ))//DeQueue����ֵΪ0��ʾɾ���ɹ�
				{
					pLastMP=GetMPDataOut(0,pMQ);		 //vm_motion_task��pLastMPû��ʵ������
				//	ps_debugout("%d %f\r\n",icount,mdata[INPA].pos[Ax]);
				}
				else continue;
			}
		}
		else continue;

//		CurIndex=mdata[INPA].Info.index;

		g_pos=0;

		ErrNo=inp_t_move(INPA,&mdata[INPA],pLastMP,&ParamTemp[INPA]);

		//ps_debugout("g_pos=%d\n",g_pos);

		if(ErrNo)
		{
			ErrRecord(ErrNo);

			switch(ErrNo)
			{
				case _ERR_SPEEDERR:
/*					while(1)
					{
						OSTimeDly(10);
					}
*/					break;

				case _ERR_AXISSTOP:
//				case _ERR_MOTIONCONFLICT:						//�˶���ͻ
//					InitQueue(pMQ);
//					InitQueue(pEPQ);
//					mdata[INPA].Info.bDataTag=3;
				//	ebugout("%d AA_ERR_AXISSTOP!\r\n",ErrNo);
					break;

				default:
					debugout("%d _ERR_AXISSTOP!\r\n",ErrNo);
					break;
			}
		}


		if(SYS_State==VM_STOP)									//�˶�ֹͣ�����ڴ���ͣ��ֹͣ�Ȳ���
		{
			float 	rate,unitrate;
			INT16U 	sta;
			//INT16S	i	;
			//int t2;

			if(g_iStopMode==0)									//����ֹͣ����
			{

				rate=RANGERATE1;

				if(get_left_index()>0)
				{
					unitrate=rate/get_left_index();
				}
				else
					unitrate=1;

				for(;;)
				{
					sta=get_left_index();

					//ps_debugout("VM_STOP sta*unitrate = %f\n",sta*unitrate);

					vm_setrange1(sta*unitrate);

					if(FPGARATE<0.02)
					{
						//ps_debugout("FPGARATE=%f\n",FPGARATE);
						break;
					}

					DelaySec(1);
				}
			}

			StopAll();

			for(;;)					//���Ӳ岹״̬ȷ�ϣ��������һ��������Ϊ�������⵼���޷��˳�
			{
				SET_RANGE(1.0);

				//adt_get_status_all(&status);
				if(IsInpEnd(1))break;
				DelaySec(1);
			}

			vm_getindex();////�ڼ�ͣVM_STOPʱ��getindex��λ�����ִ�е�λ��

			InitQueue(pMQ);
			InitQueue(pEPQ);
			mdata[INPA].Info.bDataTag=3;
			eventtag=0;
			vm_setrange1(1);

			icurSetindex=icurRunindex;

			ps_debugout("VM_STOP ...\r\n");

			continue;
		}

		if(SYS_State==VM_TAGSTOP)								//�˶�������ֹͣ��ǣ��������ڴ����������������˳�����ջ���
		{
			float 	rate,unitrate;
			INT32S 	status;
			INT16U 	sta;

			ps_debugout("VM_TAGSTOP ...\r\n");
			rate=RANGERATE1;

			if(get_left_index()>0)
			{
				unitrate=rate/get_left_index();
			}
			else
				unitrate=1;

			for(;;)
			{
				event_proc(0,&mdata[INPA]);							//�˴����õ�����δ���
				adt_get_pos_reach_status(1,INPA_AXIS,&status);
				sta=get_left_index();
				vm_setrange1(sta*unitrate);
				if(FPGARATE<0.01)
					SET_RANGE(0.01);

				if(!sta && !status)
				{
					break;
				}
				else
				{
				//	ps_debugout("icurRunindex=%d\n",icurRunindex);
				}

				if(SYS_State==VM_STOP)
					break;

				DelaySec(1);
			}

			adt_set_axis_stop(0,0,1);//����������ֹͣ

			//ps_debugout("end2 icurSetindex=%d,icurRunindex=%d\n",icurSetindex,icurRunindex);
			for(;;)
			{
				SET_RANGE(1);
				DelaySec(1);
				if(IsInpEnd(1))break;
			}

			event_end();

			InitQueue(pMQ);
			InitQueue(pEPQ);
			mdata[INPA].Info.bDataTag=3;
			eventtag=0;
			vm_setrange1(1);

			icurSetindex=icurRunindex;

			continue;
		}
		//������ѭ������for�б�����Delaysec(),���ǳ�̬������״̬�¶���������һ���Delaysec()
	}
}


int vm_motion_init(void)
{
	volatile int i;

	ps_debugout("vm_motion_init\r\n");

	memset(&VM,0x00,sizeof(VM));
	vm_efunc_reset();
	SYS_State=VM_IDLE;
	SYS_ErrNo=0;
	ps_debugout("vm_motion_init1\r\n");
	for(i=Ax;i<MAXAXIS;i++)
	{
		VM.Amax[i]=3000;
		VM.SpdMax[i]=100;
		SYS_vc(i)=20;
		SYS_vs(i)=0;
		SYS_ve(i)=0;
		SYS_acc(i)=50;
		SYS_acc(i)=50;
		SYS_admode(i)=1;
		SYS_MacPos[i]=0;
		SYS_RMacPos[i]=0;

		SYS_Rate2[i]=1.;
		SYS_CoderGear[i]=CALRATE;								//Ĭ��Ϊ����һ��
		CoderGear[i]=1./CALRATE;
		SYS_LogicPos[i]=0;
		fSyncTime[i]=0;
		SYS_MODEPULSEDIR[i]=1;
		SYS_MODECODERDIR[i]=1;

		SYS_RMacPos[i]=0;

	}

	fSyncTime[INPA]=0;
	fSyncTime[INPB]=0;

	SYS_Rate1=1;
	SYS_Rate2[INPA]=1.;
	SYS_Rate2[INPB]=1.;

	SYS_admode(INPA)=1;
	SYS_admode(INPB)=1;
	SYS_vc(INPA)=20;
	SYS_vc(INPB)=20;
	SYS_vs(INPA)=0;
	SYS_vs(INPB)=0;
	SYS_ve(INPA)=0;
	SYS_ve(INPB)=0;
	SYS_acc(INPA)=500;
	SYS_acc(INPB)=500;
	SYS_acc(INPA)=500;
	SYS_acc(INPB)=500;
	VM.SpdMax[INPA]=100;
	VM.SpdMax[INPB]=100;
	SYS_Iterations=30;
	SYS_PreCodeBufs=0;

	MethodFlag = 1;					//��ʼ��Ĭ�ϴ��᲻����
	ps_debugout("vm_motion_init2\r\n");
	return 0;
}


int vm_efunc_reset(void)
{
	int i;

	SYS_ErrNo=_ERR_NO_ERR;
	EH.EHCount=0;
	SYS_State=VM_IDLE;
	memset(ParamTemp,0x00,sizeof(ParamTemp));

	memset((void*)MPDataBuf,0x00,sizeof(MP)*MaxMParaPart);
	MotionQueue.QNode=MPDataBuf;
	pMQ=&MotionQueue;
	pMQ->MaxCount=MaxMParaPart;
	InitQueue(pMQ);

	memset((void*)EPDataBuf,0x00,sizeof(EPDataBuf));
	EPQueue.QNode=EPDataBuf;
	pEPQ=&EPQueue;
	pEPQ->MaxCount=MaxMParaPart;
	InitQueue(pEPQ);

	memset(mdata,0x00,sizeof(mdata));

	for(i=Ax;i<(MAXAXIS+MAXINPGROUP);i++)
		mdata[i].Info.bDataTag=3;
	MPCount=0;
	MPCOUNTOFFSET=0;
	return 0;
}


	/*********************************************
	������:       	vm_inp_move
	��Ҫ����:    	������
	�������:
					index---------- ����ֵ,�����1��ʼ�������ⲻ��������������ͬ��index��
									���߾�����Ҫ��һ�������˶��������������ظ���index
			 		maxis----------����
			 		pos1,pos2------���λ��
					pos3,pos4------���λ��
					speed----------���������ٶ�
	�������:       ��
	����ֵ:         ��
	*********************************************/

int vm_inp_move(int index,int maxis,int pos1,int pos2,int pos3,int pos4,int speed)  //δ��
{
	static 	float 	ftemp,ftemp2;
	static 	int 	i,j;

	float 	relpos[4];
	MP 		Data;

	ps_debugout("rel %d %d %d %d %d speed:%d\r\n",maxis,pos1,pos2,pos3,pos4,speed);

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	if(maxis<0)
	{
		goto INPASUC;
	}

	relpos[0]=pos1/CALRATE;
	relpos[1]=pos2/CALRATE;
	relpos[2]=pos3/CALRATE;
	relpos[3]=pos4/CALRATE;

	ftemp=0;
	j=0;

	for(i=Ax;i<MAXAXIS;i++)
	{
		ftemp2=fabsf(relpos[i]);

		if(ftemp<ftemp2)
		{	j=i;
			ftemp=ftemp2;
		}
	}

	if(fabsf(ftemp)<0.001)
		goto INPASUC;

	if(fabsf(relpos[maxis])<0.001)
	{
		maxis=j;
	}

	memset(&Data,0x00,sizeof(Data));
	Data.maxis=maxis;
	Data.Info.index.Index=index;
	MPCount++;
	MPCount&=MAXMPCOUNT;
	Data.Info.index.QueueIndex=MPCount?MPCount:++MPCount;		//MPCount ��Զ����Ϊ0��0�����⴦��ı��

	Data.vmax=speed/CALRATE;
	Data.Info.bDataTag=0;

	for(i=Ax;i<MAXAXIS;i++)
	{
		SYS_RMacPos[i]+=relpos[i];
		Data.axis[i].pos=SYS_RMacPos[i];
		Data.axis[i].relpos=relpos[i];
	}


	Data.pos=Data.axis[Data.maxis].pos;
	Data.relpos=Data.axis[Data.maxis].relpos;

	if(!SpeedPretreatment(&Data,pMQ))
	{
		EnQueue(sizeof(MP),&Data,pMQ);
	}
	else
		goto INPAERR;

INPASUC:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;

INPAERR:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return FAIL;
}




	/*********************************************
	������:       	vm_abs_inp_move
	��Ҫ����:    	������
	�������:
					index---------- ����ֵ,�����1��ʼ�������ⲻ��������������ͬ��index��
									���߾�����Ҫ��һ�������˶��������������ظ���index
			 		axismap--------ӳ�����
			 		maxis----------����
			 		pos1,pos2------����λ��
					pos3,pos4------����λ��
					pos5,pos6------����λ��

					speed------�����ٶ�
	�������:       ��
	����ֵ:         ��
	*********************************************/
int vm_abs_inp_move(int index,int axismap,int maxis,int pos1,int pos2,int pos3,int pos4,int pos5,int pos6,int pos7,int pos8,int pos9,int pos10,int pos11,int pos12,int pos13,int pos14,int pos15,int pos16,int speed)
{
	static 	float 	ftemp,ftemp2;
	static 	int 	i,j;
	float 	pos[MAXAXIS]={0};
	MP 		Data;

//ps_debugout("abs %d %d %d %d %d speed:%d\t",maxis,pos1,pos2,pos3,pos4,speed);

//ps_debugout("qq=%d\r\n",pos1);
	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	if(maxis<0)
	{
		goto INPASUC;
	}

	if(pos1>0)
		pos[Ax]=0.0005+pos1/CALRATE;

//	ps_debugout("%f\r\n",pos[Ax]);

	if(pos1<0)
		pos[Ax]=-0.0005+pos1/CALRATE;

	if(pos2>0)
		pos[Ay]=0.0005+pos2/CALRATE;

	if(pos2<0)
		pos[Ay]=-0.0005+pos2/CALRATE;

	if(pos3>0)
		pos[Az]=0.0005+pos3/CALRATE;

	if(pos3<0)
		pos[Az]=-0.0005+pos3/CALRATE;

	if(pos4>0)
		pos[Aa]=0.0005+pos4/CALRATE;

	if(pos4<0)
		pos[Aa]=-0.0005+pos4/CALRATE;

	if(pos5>0)
		pos[Ab]=0.0005+pos5/CALRATE;

	if(pos5<0)
		pos[Ab]=-0.0005+pos5/CALRATE;

	if(pos6>0)
		pos[Ac]=0.0005+pos6/CALRATE;

	if(pos6<0)
		pos[Ac]=-0.0005+pos6/CALRATE;


	if(pos7>0)
		pos[A7]=0.0005+pos7/CALRATE;
	if(pos7<0)
		pos[A7]=-0.0005+pos7/CALRATE;

	if(pos8>0)
		pos[A8]=0.0005+pos8/CALRATE;
	if(pos8<0)
		pos[A8]=-0.0005+pos8/CALRATE;

	if(pos9>0)
		pos[A9]=0.0005+pos9/CALRATE;
	if(pos9<0)
		pos[A9]=-0.0005+pos9/CALRATE;

	if(pos10>0)
		pos[A10]=0.0005+pos10/CALRATE;
	if(pos10<0)
		pos[A10]=-0.0005+pos10/CALRATE;

	if(pos11>0)
		pos[A11]=0.0005+pos11/CALRATE;
	if(pos11<0)
		pos[A11]=-0.0005+pos11/CALRATE;

	if(pos12>0)
		pos[A12]=0.0005+pos12/CALRATE;
	if(pos12<0)
		pos[A12]=-0.0005+pos12/CALRATE;

	if(pos13>0)
		pos[A13]=0.0005+pos13/CALRATE;
	if(pos13<0)
		pos[A13]=-0.0005+pos13/CALRATE;

	if(pos14>0)
		pos[A14]=0.0005+pos14/CALRATE;
	if(pos14<0)
		pos[A14]=-0.0005+pos14/CALRATE;

	if(pos15>0)
		pos[A15]=0.0005+pos15/CALRATE;
	if(pos15<0)
		pos[A15]=-0.0005+pos15/CALRATE;

	if(pos16>0)
		pos[A16]=0.0005+pos16/CALRATE;
	if(pos16<0)
		pos[A16]=-0.0005+pos16/CALRATE;


	memset(&Data,0x00,sizeof(Data));
	Data.maxis=maxis;											//�öλ���岹�������ʾ
	Data.vmax=speed/CALRATE;									//�öλ���岹���ٶȵ�λ1000����
	Data.Info.bDataTag=0;


	for(i=Ax;i<MAXAXIS;i++)
	{
		if(axismap & (0x01<<i))									//�ƶ���Ӧ��λ�����Ϊ1��ʾ��Ӧ���Ὺ����Ϊ0��ʾ�ر�
		{
			Data.axis[i].pos=pos[i];							//��Ÿöλ���岹����Ŀ��λ��
			Data.axis[i].relpos=pos[i]-SYS_RMacPos[i];			//��ŵ�ǰ���굽����岹����Ŀ��λ�õľ���
			//if(i==1)
				//ps_debugout("pos[1]=%f SYS_RMacPos[1]=%f \n",pos[i],SYS_RMacPos[i]);
			SYS_RMacPos[i]=pos[i];
		}
		else
		{
			Data.axis[i].pos=SYS_RMacPos[i];
			Data.axis[i].relpos=0;
		}
	}

	ftemp=0;
	j=0;

	for(i=Ax;i<MAXAXIS;i++)										//��öλ���岹����ƶ������������ƶ��������
	{
		ftemp2=fabsf(Data.axis[i].relpos);

		if(ftemp2>ftemp)
		{
			j=i;
			ftemp=ftemp2;
		}
	}

	if(fabsf(ftemp)<0.001)
		goto INPASUC;

	if(fabsf(Data.axis[maxis].relpos)<0.001)					//�����ƶ��������Ϊ0�������Ÿ�ֵ��������
	{
		maxis=j;
		Data.maxis=maxis;
	}

//	ps_debugout("%5.3f %5.3f %5.3f \r\n",Data.axis[Ax].relpos,Data.axis[Ay].relpos,Data.axis[Az].relpos);
	Data.pos=Data.axis[Data.maxis].pos;
	Data.relpos=Data.axis[Data.maxis].relpos;
	Data.Info.index.Index=index;

	MPCount++;
	MPCount&=MAXMPCOUNT;
	Data.Info.index.QueueIndex=MPCount?MPCount:++MPCount;		//MPCount ��Զ����Ϊ0��0�����⴦��ı��

	if(!SpeedPretreatment(&Data,pMQ))							//�ٶ�ǰհ��������ٲ������
	{
		EnQueue(sizeof(MP),&Data,pMQ);

		if(MethodFlag==0)
		{
			FollowSpeedPretreatment(pMQ);
		}
	}
	else
		goto INPAERR;

INPASUC:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;

INPAERR:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return FAIL;
}


int vm_start(void)
{

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	if(SYS_State!=VM_IDLE)
		goto Err;


	OSSemPost(&VM_START,OS_OPT_POST_1,&os_err);

Err:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;
}


int vm_stop(int mode)
{
	int i=0;

	CPU_SR_ALLOC();

	OS_CRITICAL_ENTER();

	g_iStopMode=mode;


	if(SYS_State==VM_RUN  || SYS_State==VM_TAGSTOPCMD)
	{
		SYS_State=VM_STOP;
		OS_CRITICAL_EXIT();

		for(;;)
		{
			DelaySec(1);

			i++;

			if(i>1000 )
			{
				ps_debugout("vm_stop timeout!!\n ");
				SYS_State=VM_IDLE;
				return FAIL;
			}


			if(SYS_State==VM_IDLE)break;

		}
	}
	else
		OS_CRITICAL_EXIT();

	return 0;
}


int vm_tag_stop(void)
{
	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	ps_debugout("vm_tag_stop\r\n");

	if(SYS_State==VM_RUN)
	{
		SYS_State=VM_TAGSTOPCMD;
	}

	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);

	return SUCCESS;
}


int vm_reset(void)
{
	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	vm_efunc_reset();
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);

	return SUCCESS;
}


int vm_set_io(int port,int level)
{
	EP d;
	MP *pMP;

//	ps_debugout("port=%d,level=%d\n",port,level);

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	pMP = GetMPDataIn(0,pMQ);    								//��ȡ����β����
	memset(&d,0x00,sizeof(d));
	d.Type=_T_GPIO;
	d.Data.Gpio.ioport =port;
	d.Data.Gpio.iolevel=level;
	if(!pMP)
		d.QueueIndex=0;
	else
		d.QueueIndex=pMP->Info.index.QueueIndex;

	EnQueue(sizeof(EP),&d,pEPQ);
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);

	return SUCCESS;
}


int vm_set_probe(int pro,int port)
{
	EP d;
	MP *pMP;

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	pMP = GetMPDataIn(0,pMQ);    								//��ȡ����β����
	memset(&d,0x00,sizeof(d));
	d.Type=_T_PROBE;
	d.Data.Probe.pro =pro;
	d.Data.Probe.port=port;

	if(!pMP)
		d.QueueIndex=0;
	else
		d.QueueIndex=pMP->Info.index.QueueIndex;

	EnQueue(sizeof(EP),&d,pEPQ);
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);

	return SUCCESS;

}


int vm_set_position(int axis,int pos)
{
	EP d;
	MP *pMP;

	if(axis<Ax || axis>A16)return FAIL;

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	pMP = GetMPDataIn(0,pMQ);    								//��ȡ����β����
	memset(&d,0x00,sizeof(d));
	d.Type=_T_POSSET;
	d.Data.Posset.axis =axis;
	d.Data.Posset.pos=pos;

	if(!pMP)
	{
		d.QueueIndex=0;

		BaseSetCommandPos(axis+1,pos);

		BaseGetCommandPos(axis+1,&lastpulse[axis]);
		SYS_RMacPos[axis]=lastpulse[axis]/1000.0;
	//	ps_debugout("v_s_p MPCot=%d\n",MPCount);

		goto VM_SET_POS_END;
	}
	else
		d.QueueIndex=pMP->Info.index.QueueIndex;

	EnQueue(sizeof(EP),&d,pEPQ);
//	ps_debugout("@%d %d\r\n",axis,pos);
	SYS_RMacPos[axis]=pos/CALRATE;

VM_SET_POS_END:

	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);

	return SUCCESS;
}


int vm_set_tag(int tag)
{
	EP d;
	MP *pMP;

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	pMP = GetMPDataIn(0,pMQ);    								//��ȡ����β����
	memset(&d,0x00,sizeof(d));
	d.Type=_T_TAGSET;
	d.Data.Tagset.tag=tag;

	if(!pMP)
	{
		d.QueueIndex=0;
		CurTag=tag;
		goto VM_SET_TAG_ENG;
	}
	else
		d.QueueIndex=pMP->Info.index.QueueIndex;

	EnQueue(sizeof(EP),&d,pEPQ);

VM_SET_TAG_ENG:
//	ps_debugout("vm_set_tag =%ld\n",tag);
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;

}


int vm_get_status(void)
{
	int 	status=-1;

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	//ps_debugout("pMQ->QueueCount=%d,icurSetindex-icurRunindex=%d  status=%d\n",pMQ->QueueCount,icurSetindex-icurRunindex,status);
	if (pMQ->QueueCount==0 && get_left_index()==0 && IsInpEnd(1))	//ִ�����
	{
		status= 0;
	}
	else															//δִ�����
		status= 1;

	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return status;
}


int vm_getindex(void)
{
	INT16U 	count;
	int 	sta;
	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
	count=get_left_index();

	if(count==0)
	{
		adt_get_pos_reach_status(1,INPA_AXIS,&sta);

		if(sta==0 )
		{
			FIFOCOUNT=0;
		//	ps_debugout("vm_getindex2\n");
			goto Err;
		}
	//	ps_debugout("vm_getindex1\n");
		count=0;
	}

	CurIndex=FIFOBUF[(FIFOCOUNT-count-1)&0x7ff].Index;

Err:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return CurIndex;
}


int vm_getqueueindex(void)
{

	INT16U 	count;
	//INT8U 	err;
	int 	sta;

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	count=get_left_index();
	if(count==0)
	{
		adt_get_pos_reach_status(1,INPA_AXIS,&sta);

		if(sta==0)
		{
			FIFOCOUNT=0;
			CurQueueIndex=0;
			goto Err;
		}

		count=1;
	}

	CurQueueIndex=FIFOBUF[(FIFOCOUNT-count)&0x7ff].QueueIndex;
Err:
	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return CurQueueIndex;

}


int vm_setindex(TIT index)
{
	int count;

	OSSemPend(&VM_PV,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

	FIFOCOUNT=FIFOCOUNT+1;										//����岹һ��ָ��Ļ���ռ���6��������岹��8��

	for(count=1;count>0;count--)
	{
//		ps_debugout("\t%d\n",(FIFOCOUNT-count+1)&0x7ff);
		FIFOBUF[(FIFOCOUNT-count)&0x7ff]=index;
	}

	OSSemPost(&VM_PV,OS_OPT_POST_1,&os_err);
	return SUCCESS;
}


int vm_setrange1(float rate)
{
	RANGERATE1=rate;
	FPGARATE=RANGERATE1*RANGERATE2;
	SET_RANGE(FPGARATE);

	return 0;
}


int vm_setrange2(int rate)
{
	RANGERATE2=rate/8000.;
	FPGARATE=RANGERATE1*RANGERATE2;
	SET_RANGE(FPGARATE);

	//ps_debugout("%f\n",FPGARATE);
	return 0;
}

int get_left_index()
{
	adt_get_inp_index(INPA_AXIS,&icurRunindex);

	if(icurSetindex-icurRunindex<0)
	{
		ps_debugout("%d %d  %d index ERR\n ",icurSetindex,icurRunindex,icurSetindex-icurRunindex);
	}

	return 	(icurSetindex-icurRunindex);
}


int fifo_inp_move16_axis(int InpAxis,int index,long pos1,long pos2,long pos3,long pos4,long pos5,long pos6,long pos7,long pos8,long pos9,long pos10,long pos11,long pos12,long pos13,long pos14,long pos15,long pos16,long speed)
{
	FP32 fspeedpulse[16]={0.0};
	long lpulse1[16]={pos1,pos2,pos3,pos4,pos5,pos6,pos7,pos8,pos9,pos10,pos11,pos12,pos13,pos14,pos15,pos16};
	long lpulse[16]={labs(pos1),labs(pos2),labs(pos3),labs(pos4),labs(pos5),labs(pos6),labs(pos7),labs(pos8),labs(pos9),labs(pos10),labs(pos11),labs(pos12),labs(pos13),labs(pos14),labs(pos15),labs(pos16)};
	long pulseMax=0;
	FP32 finptotalspeed=0.0;
	INT32S i=0,j=0;

	//static int k=0;

	pulseMax=lpulse[0];

	for(i=0;i<15;i++)
	{
		pulseMax=(pulseMax<lpulse[i+1])?lpulse[i+1]:pulseMax;
	}

	if(pulseMax==0)
	{
		printf("speed Zero!!!\n");
		return 1;
	}

	for(j=0;j<16;j++)
	{
		fspeedpulse[j]=(FP32)lpulse[j]*speed/pulseMax;
		//ps_debugout("ipospulse=%d,ispeedpulse=%d,pospulseMax=%d\n",ipospulse[j],ispeedpulse[j],pospulseMax);
		//ps_debugout("speed[%d]=%d\n",j,(long)((FP32)ipospulse[j]*speed/pospulseMax+0.5));
		finptotalspeed=finptotalspeed+fspeedpulse[j]*fspeedpulse[j];
	}

	finptotalspeed=sqrt(finptotalspeed)+0.5;

	g_pos += lpulse1[1];  //���������ڲ���

	return adt_fifo_inp_move16_pulse(InpAxis,index,lpulse1,(long)finptotalspeed);

}

