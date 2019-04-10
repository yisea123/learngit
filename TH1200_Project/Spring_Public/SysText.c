/*
 * SysText.c
 *
 *  Created on: 2017��8��10��
 *      Author: yzg
 */

#include "Parameter_public.h"
#include "SysText.h"

#ifndef WIN32
#include "ParaManage.h"
#else
#include "stdio.h"
#include "string.h"
#endif

INT32U REGNULL;

// ��λ������Ҫ�õ���Щ����
#ifdef WIN32
#define IOFilterParamProc NULL
#define GearParamProc NULL
#define SwitchParamProc NULL
#define LimtParamProc NULL
#define AxisParamProc NULL
#define MageParamProc NULL
#define StartOffsetProc NULL
#define SlaveZeroSet NULL
#define BackModeProc NULL
#define AxisRunModeProc NULL
#define AxisMulRunModeProc NULL
#define BackOrderParamProc NULL
#define UnpackParamProc NULL
#endif

TParamTable ParaRegTab[] = {
//�ۺϲ���******************************************************************************************
	{RO_USER, DT_TITLE 	, 0, 	STR("�ۺϲ���"), NULL},	//��������,��������������ͬ���Ͳ���

	{RW_PROGRAMER, DT_INT8U , 0, 	STR("IO�˲��ȼ�(0~15)")			, 	ADDR(g_Sysparam.FilterTime)			, {0, 15}			,IOFilterParamProc	,0	,0,NULL		,0,STRRANGE("0��ʾ���˲�,����Ϊn�����˲�ʱ��Ϊ��2��n�η�us(΢��)")},
	{RW_SUPER, DT_INT32U	, 0, 	STR("ͨ�ŷ�ʽѡ��<��>")			    , 	ADDR(g_Sysparam.CommMode)			    , {0, 4}				,NULL			,0	,0,NULL		,1,STRRANGE("0-���ڣ�1-TCP��2-UDP")},
	{RW_SUPER, DT_INT32U	, 0, 	STR("ϵͳ������<��>")				, 	ADDR(g_Sysparam.BaudRate)			, {1, 1000000}		,NULL			,0	,0,NULL			,1,STRRANGE("��")},
	{RW_SUPER, DT_INT8U 	, 0, 	STR("������ID��<��>")				, 	ADDR(g_Sysparam.ControlerID)		, {1, 250}			,NULL			,0	,0,NULL			,0,STRRANGE("��")},
	{RW_USER, DT_INT16S , 0, 	STR("����ѡ��")				    , 	ADDR(g_Sysparam.iScrProtect)		, {0, 1}			,NULL			,0	,0,NULL			,0,STRRANGE("��")},
	{RW_USER, DT_INT16S , 0, 	STR("����ʱ��")				    , 	ADDR(g_Sysparam.iScrProtectTime)		, {0, 32767}			,NULL			,0	,0,NULL	,0,STRRANGE("��")},
	{RW_USER, DT_INT16S , 0, 	STR("����ͼƬ")				    , 	ADDR(g_Sysparam.iScrProtectBmp)		, {0, 255}			,NULL			,0	,0,NULL			,0,STRRANGE("��")},
	//{RW_USER, DT_INT16S , 0, 	STR("ָʾ����״̬")				, 	ADDR(g_Sysparam.iHandStatus)		, {0, 255}			,NULL			,0	,0,NULL				,0},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("��ҡ�ٶ�")				    , 	ADDR(g_Sysparam.iHandRunSpeed)		, {1, 10}			,NULL			,0	,0,NULL		,0,STRRANGE("��ҡ�ֵļ���ٶ�,Ĭ��2.��λ:��.��Χ(1-10).")},
	{RW_PROGRAMER, DT_FLOAT  , 0, 	STR("��ҡ����")				    , 	ADDR(g_Sysparam.fHandRunRate)		, {1, 1000}			,NULL			,0	,0,NULL		,0,STRRANGE("��ҡ���ٶȷŴ���,Ĭ��75.��λ:��.��Χ(1-1000).")},
	{RW_PROGRAMER, DT_FLOAT , 0, 	STR("��ҡ���ٱ���")				    , 	ADDR(g_Sysparam.fHandRateL)		    , {0.1, 1}			,NULL			,0	,0,NULL		,0,STRRANGE("�ֶ�����Ϊ(��)ʱ���ֶ��ٶȵ�Уֵ��λ:��.��Χ(0.1-1).")},
	{RW_PROGRAMER, DT_FLOAT , 0, 	STR("��ҡ���ٱ���")				    , 	ADDR(g_Sysparam.fHandRateM)		    , {0.1, 2}			,NULL			,0	,0,NULL		,0,STRRANGE("�ֶ�����Ϊ(��)ʱ���ֶ��ٶȵ�Уֵ��λ:��.��Χ(0.1-2).")},
	{RW_PROGRAMER, DT_FLOAT , 0, 	STR("����")				        , 	ADDR(g_Sysparam.fHandRateH)		    , {0.1, 4}			,NULL			,0	,0,NULL		,0,STRRANGE("�ֶ�����Ϊ(��)ʱ���ֶ��ٶȵ�Уֵ��λ:��.��Χ(0.1-4).")},
	{RW_SUPER,DT_INT16S , 0, 	STR("���ݱ༭ģʽ "),		             ADDR(g_Sysparam.UnpackMode)		, {0, 1}			,UnpackParamProc	,0	,0,NULL ,1,STRRANGE("���ݱ༭ģʽ  0-�Ŵ�ģʽ  1-ԭֵģʽ")},
	{RW_SUPER,DT_INT16S , 0, 	STR("�������ģʽ����"),		             ADDR(g_Sysparam.iBackZeroMode)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("�������ģʽ����  0-����˳�򵥶�����ģʽ  1-����˳�������ͬʱ����ģʽ(��������˳����ֵ��Сһ��������Щ��ͬʱ����)")},
	{RW_SUPER,DT_INT16S , 0, 	STR("�̵�ģʽ"),		                 ADDR(g_Sysparam.iTeachMode)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("�̵�ģʽ����  0-���н̵�ģʽ  1-����̵�ģʽ")},
	{RW_SUPER,DT_INT16S , 0, 	STR("�����˶�����"),		                 ADDR(g_Sysparam.iHandWhellDrect)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("�����˶���������  0-���� 1-����")},

	{RO_USER, DT_NONE  	, 0, ("")},	//�ۺϲ�������
//�������**************************************************************************************************************************
    {RO_USER, DT_TITLE 	, 0, 	STR("�������"), NULL},
	//�����޸Ĳ��������ַ���!!!!!!!!!!!!!!!!!
	//{RW_SUPER,DT_INT32U	, 0, 	STR("��ʼ���û�����Ϊ����ֵ (����������ϵͳ) "),	 ADDR(REGNULL)				, {0, 999999}		    ,MageParamProc	,1	,3,NULL	,1,STRRANGE("������ 255 ��ʼ���û�����Ϊ����ֵ (������ϵͳ)")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("��ʼ��IO����(����������ϵͳ)"),		 ADDR(REGNULL)				, {0, 999999}		,MageParamProc	,2	,4,NULL	,1,STRRANGE("������ 255 ��ʼ��IO����(������ϵͳ)")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("��ʼ��ȫ������(����������ϵͳ)"),		 ADDR(REGNULL)				, {0, 999999}		,MageParamProc	,3	,5,NULL	,1,STRRANGE("������ 255 ��ʼ��ȫ������(������ϵͳ)")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("��������"),				         ADDR(REGNULL)						, {0, 999999}			,MageParamProc	,4	,6,NULL	,1,STRRANGE("������ 255 �����ļ�  ����·�����ļ�����0:/sysconf.bak")},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("�����ָ�(����������ϵͳ)"),		     ADDR(REGNULL)						, {0, 999999}		,MageParamProc	,5	,7,NULL	,1,STRRANGE("������ 255 �ָ����� �ָ�·�����ļ�����0:/sysconf.bak(������ϵͳ)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("��ʼ���û�����Ϊ����ֵ (����������ϵͳ)"),	 "InitUserPara",(INT8U*)(&(REGNULL))				, {0, 999999}		    ,MageParamProc	,1	,3,NULL	,1,STRRANGE("������ 255 ��ʼ���û�����Ϊ����ֵ (������ϵͳ)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("��ʼ��IO����(����������ϵͳ)"),		 "InitIOPara",(INT8U*)(&(REGNULL))				, {0, 999999}		,MageParamProc	,2	,4,NULL	,1,STRRANGE("������ 255 ��ʼ��IO����(������ϵͳ)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("��ʼ��ȫ������(����������ϵͳ)"),		 "InitAllPara",(INT8U*)(&(REGNULL))				, {0, 999999}		,MageParamProc	,3	,5,NULL	,1,STRRANGE("������ 255 ��ʼ��ȫ������(������ϵͳ)")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("��������"),				         "BakPara",(INT8U*)(&(REGNULL))						, {0, 999999}			,MageParamProc	,4	,6,NULL	,1,STRRANGE("������ 255 �����ļ�  ����·�����ļ�����0:/sysconf.bak")},
    {RW_SUPER,DT_INT32U	, 0, 	STR("�����ָ�(����������ϵͳ)"),		     "RecoverPara",(INT8U*)(&(REGNULL))						, {0, 999999}		,MageParamProc	,5	,7,NULL	,1,STRRANGE("������ 255 �ָ����� �ָ�·�����ļ�����0:/sysconf.bak(������ϵͳ)")},
	{RW_SUPER,DT_INT16U , 0, 	STR("ϵͳ������ (����������ϵͳ)"),		     ADDR(g_Sysparam.TotalAxisNum)		, {1, 16}			,NULL			,0	,0,NULL ,1,STRRANGE("��ϵͳʹ�õ�����,ϵͳ��������Ч.��λ:��.��Χ(1-16).")},
	{RW_SUPER,DT_INT8U 	, 0, 	STR("ϵͳ������Ϣ���ʹ��"),			     ADDR(g_Sysparam.DebugEnable)		, {0, 1}			,NULL			,0	,0,NULL ,1,STRRANGE("ϵͳ������Ϣ���ʹ��:0-�ر�,1-ʹ��")},
	{RW_USER, DT_INT16S , 0, 	STR("����ѡ��"),              	         ADDR(g_Sysparam.iLanguage)	  	    , {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("����ѡ��0:��������,1:Ӣ��.")},
	{RW_SUPER, DT_INT16S , 0, 	STR("������������"),              	     ADDR(g_Sysparam.iMachineCraftType)	, {0, 2}	        ,NULL	        ,0	,0,NULL	,0,STRRANGE("�����������ͣ�0:12���׼���ɻ�,1:ѹ�ɻ�,2:6�����ܻ�")},

	{RO_USER, DT_NONE  	, 0, ("")},	//�����������

//���ղ���****************************************************************************************************
	{RO_USER, DT_TITLE 	, 0, 	STR("���ղ���"), NULL},

	//{RW_SUPER,DT_INT32U	, 0, 	STR("�ӹ�����"),		ADDR(g_Sysparam.lRunCount)		    , {0, 999999}			,NULL          	,0	,0,NULL	,0},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("Ŀ�����"),		ADDR(g_Sysparam.lDestCount)		    , {0, 999999}			,NULL          	,0	,0,NULL	,0},
	//{RW_SUPER,DT_INT32U	, 0, 	STR("̽�����"),		ADDR(g_Sysparam.lProbeCount)		, {0, 999999}			,NULL          	,0	,0,NULL	,0},
	{RW_USER, DT_FLOAT 	, 0, 	STR("X������ٶ�"), 	ADDR(g_Sysparam.fYMaxSpeed)			, {1, 500}			,NULL			,0	,0,NULL	,0,STRRANGE("Sָ��X������ٶ�.��λ:ת/��.��Χ(1-500).")},
	{RW_USER, DT_INT16S , 0, 	STR("���߼��ʹ��"), 	ADDR(g_Sysparam.iPaoXian)	  		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("���߼��ʹ��:0-�ر�;1-ʹ��")},
	{RW_USER, DT_INT16S , 0, 	STR("�������ʹ��"), 	ADDR(g_Sysparam.iWarningOut)	  	, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("�������ʹ��:0-�ر�;1-ʹ��")},
	{RW_USER, DT_INT16S , 0, 	STR("����ʹ��"),  	ADDR(g_Sysparam.iYDaoXian)	  	    , {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("����ʹ��:0-�ر�;1-ʹ��")},
	{RW_USER, DT_INT16S , 0, 	STR("�˿ط�ʽ"), 	ADDR(g_Sysparam.iSecondAxisStyle)	    , {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("�˿ط�ʽ:0-����,1-�Ӽ���.��λ:��.��Χ(0-1).")},
	{RW_USER, DT_INT16S , 0, 	STR("�߼�����ʹ��"), 	ADDR(g_Sysparam.iXianjiaDrive)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("�߼�����ʹ��:0-�ر�;1-ʹ��")},
	{RW_USER, DT_INT16S , 0, 	STR("����ʹ��"), 		ADDR(g_Sysparam.iIOSendWire)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("����ʹ��:0-�ر�;1-ʹ��")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("̽��1��Ӧ�������(1-8��Ч)"), ADDR(g_Sysparam.Out_Probe0)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("̽��1��Ӧ������ã�1-6��Ӧ6���������ŷ�����˿ڣ�7��8�ֱ��Ӧͨ�����22,23�Ŷ˿�")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("̽��2��Ӧ�������(1-8��Ч)"), ADDR(g_Sysparam.Out_Probe1)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("̽��2��Ӧ������ã�1-6��Ӧ6���������ŷ�����˿ڣ�7��8�ֱ��Ӧͨ�����22,23�Ŷ˿�")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("̽��3��Ӧ�������(1-8��Ч)"), ADDR(g_Sysparam.Out_Probe2)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("̽��3��Ӧ������ã�1-6��Ӧ6���������ŷ�����˿ڣ�7��8�ֱ��Ӧͨ�����22,23�Ŷ˿�")},
	{RW_PROGRAMER, DT_INT16S , 0, 	STR("̽��4��Ӧ�������(1-8��Ч)"), ADDR(g_Sysparam.Out_Probe3)	, {1, 8}			,NULL			,0	,0,NULL	,0,STRRANGE("̽��4��Ӧ������ã�1-6��Ӧ6���������ŷ�����˿ڣ�7��8�ֱ��Ӧͨ�����22,23�Ŷ˿�")},
	{RW_USER,DT_INT16S , 0, 	STR("�������(3-4)"),		        ADDR(g_Sysparam.iReviseAxis)		, {3, 4}			,NULL			,0	,0,NULL ,1,STRRANGE("�������  ����ѹ�ɻ���ţ��쳤ʱ��")},
	{RW_USER, DT_FLOAT , 0, 	STR("�쳤������0")	, 	 			ADDR(g_Sysparam.fReviseValue)	, {-0.001, 10}			,NULL			,0	,0,NULL		,0,STRRANGE("�쳤������0(��λ������/��)(IO:5+,6-)")},
	{RW_USER, DT_FLOAT , 0, 	STR("�쳤������1")	, 	 			ADDR(g_Sysparam.fReviseValue1)	, {-0.001, 10}			,NULL			,0	,0,NULL		,0,STRRANGE("�쳤������1(��λ������/��)(IO:7+,8-)")},

	{RW_USER, DT_FLOAT , 0, 	STR("ת�������߲�������")	, 	 		ADDR(g_Sysparam.fReviseFTLMolecular)	, {0.01, 100}			,NULL			,0	,0,NULL		,0,STRRANGE("ת��ʱͬʱ���ߵĲ�����������")},
	{RW_USER, DT_FLOAT , 0, 	STR("ת�������߲�����ĸ")	, 	 		ADDR(g_Sysparam.fReviseFTLDenominator)	, {0.01, 100}			,NULL			,0	,0,NULL		,0,STRRANGE("ת��ʱͬʱ���ߵĲ���������ĸ")},
	{RW_USER, DT_INT16U , 0, 	STR("����ת�����"), 				ADDR(g_Sysparam.iReviseFTLAxisNo)		, {0, 16}			,NULL			,0	,0,NULL	,0,STRRANGE("����ת�����:0-�ر�;1-16 ���")},
	{RW_USER, DT_INT16S , 0, 	STR("ת��ʱ���߲���ʹ��"), 			ADDR(g_Sysparam.iReviseFTLEnable)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("ת��ʱ���߲���ʹ��:0-�ر�;1-ʹ��")},
	{RW_USER, DT_INT16U , 0, 	STR("����������ʹ��"), 				ADDR(g_Sysparam.iTapMachineCraft)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("����������ʹ��:0-�ر�;1-ʹ��")},
	{RW_USER, DT_INT16U , 0, 	STR("���������㱣��ʹ��"), 			ADDR(g_Sysparam.iGozeroProtectEn)		, {0, 1}			,NULL			,0	,0,NULL	,0,STRRANGE("���������㱣��ʹ��:0-�ر�;1-ʹ��")},

	{RO_USER, DT_NONE  	, 0, ("")},	//���ղ�������

//�����ò���********************************************************************************************************
	{RO_USER, DT_TITLE 	, 0,	STR("�����ò���"), NULL},
	//001
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")	,	ADDR(AxParam.ElectronicValue)		, {2000, 300000}		,GearParamProc			,0x00	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AyParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x10	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AzParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x20	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��Χ(2000-300000).��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AaParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x30	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AbParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x40	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AcParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x50	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AdParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x60	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(AeParam.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x70	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Ax1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x80	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Ay1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0x90	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Az1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xa0	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Aa1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xb0	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Ab1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xc0	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Ac1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xd0	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Ad1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xe0	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("ÿȦ������")		, 	ADDR(Ae1Param.ElectronicValue)			, {2000, 300000}		,GearParamProc			,0xf0	,0,NULL	,0,STRRANGE("����תһȦ��������.��λ:��.��ֵ�����ܹ���������ٱ�����.")},

	//002
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AxParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x01	,0,NULL	,0,STRRANGE("��������תһȦ����.��λ:����.��Χ(0.5-2000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AyParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x11	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AzParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x21	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AaParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x31	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AbParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x41	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AcParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x51	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AdParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x61	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(AeParam.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x71	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Ax1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x81	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Ay1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0x91	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Az1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xa1	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Aa1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xb1	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Ab1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xc1	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Ac1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xd1	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Ad1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xe1	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("ÿȦ����")    		, 	ADDR(Ae1Param.ScrewPitch)		, {0.5,2000.0}			,GearParamProc			,0xf1	,0,NULL	,0,STRRANGE("˿���ݾ��С.��λ:����.��Χ(0.5-2000);����תһȦ�Ƕ�.��λ:��.��Χ(0.5-2000)")},

	//003
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���"), 	ADDR(AxParam.CoderValue)        , {2500,99999999}		,GearParamProc			,0x02	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AyParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x12	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AzParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x22	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AaParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x32	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AbParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x42	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AcParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x52	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AdParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x62	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(AeParam.CoderValue)			, {2500,99999999}		,GearParamProc			,0x72	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Ax1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0x82	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Ay1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0x92	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Az1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xa2	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Aa1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xb2	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Ab1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xc2	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Ac1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xd2	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Ad1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xe2	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�������ֱ���")		, 	ADDR(Ae1Param.CoderValue)			, {2500,99999999}		,GearParamProc			,0xf2	,0,NULL	,0,STRRANGE("�������ֱ���.��Χ(2500-9999999).")},

	//004  //��λ�������¼��ؼӹ��ļ���Ҫ���¶�ȡԭ��ƫ��������

	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AxParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc	,0x00		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AyParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x10		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AzParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x20		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AaParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x30		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AbParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x40		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AcParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x50		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AdParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x60		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(AeParam.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x70		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Ax1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x80		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Ay1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0x90		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Az1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xa0		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Aa1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xb0		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Ab1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xc0		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Ac1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xd0		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Ad1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xe0		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},
	{RW_USER, DT_FLOAT	, 0, 	STR("ԭ��ƫ����")	, 	ADDR(Ae1Param.fStartOffset)		, {-99999.0,99999.0}		,StartOffsetProc			,0xf0		,0,NULL				,0,STRRANGE("ԭ��ƫ��:��С��λ0.1�Ȼ�0.01���ף�1����1�Ȼ�1����")},

	//005��  ����ʼ�������
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�"), 	ADDR(g_Sysparam.AxisParam[Ax0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL		,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ay0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Az0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Aa0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ab0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ac0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")   	, 	ADDR(g_Sysparam.AxisParam[Ad0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ae0].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ax1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ay1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Az1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Aa1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ab1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ac1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")   	, 	ADDR(g_Sysparam.AxisParam[Ad1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},
	{RW_USER, DT_INT32S	, 0, 	STR("����ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ae1].lMaxSpeed)		, {100,8000}			,NULL			,0	,0,NULL				,0,STRRANGE("������90%�ת��.��λ:ת/����.��Χ(100-8000).")},


	//006��  ���ֶ�����
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ax0].lHandSpeed)		, {1,9999}			,NULL	,0	,0,NULL		,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ay0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Az0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Aa0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ab0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ac0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ad0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ae0].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ax1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Ay1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Az1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")		, 	ADDR(g_Sysparam.AxisParam[Aa1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ab1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ac1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ad1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},
	{RW_USER, DT_INT32S	, 0, 	STR("�ֶ��ٶ�")	    , 	ADDR(g_Sysparam.AxisParam[Ae1].lHandSpeed)		, {1,9999}			,NULL			,0	,0,NULL				,0,STRRANGE("������ҡ�Ļ����ٶ�.��λ:����/��|��/��.��Χ(1-9999).")},

	//007��  ��ӹ����ٶ�
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")	 , 	ADDR(g_Sysparam.AxisParam[Ax0].lChAddSpeed)				, {50,10000}	,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ay0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    ,	ADDR(g_Sysparam.AxisParam[Az0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Aa0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ab0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ac0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ad0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ae0].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")	   , 	ADDR(g_Sysparam.AxisParam[Ax1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ay1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    ,	ADDR(g_Sysparam.AxisParam[Az1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Aa1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ab1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ac1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ad1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},
	{RW_SUPER, DT_INT32S	, 0, 	STR("�ӹ����ٶ�")    , 	ADDR(g_Sysparam.AxisParam[Ae1].lChAddSpeed)				, {50,10000}				,NULL	,0	,0,NULL	,0,STRRANGE("�ӹ�ʱ���Ӽ��ٵļ��ٶ�.��λ:KPP/��.��Χ(50-10000).")},

	//008�� ��ԭ���ٶ�
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")	  		, 	ADDR(g_Sysparam.AxisParam[Ax0].fBackSpeed), {0.1,1000.0}	            ,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ay0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		,	ADDR(g_Sysparam.AxisParam[Az0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")   		, 	ADDR(g_Sysparam.AxisParam[Aa0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ab0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ac0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ad0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")  		, 	ADDR(g_Sysparam.AxisParam[Ae0].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")	  		, 	ADDR(g_Sysparam.AxisParam[Ax1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ay1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		,	ADDR(g_Sysparam.AxisParam[Az1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")   		, 	ADDR(g_Sysparam.AxisParam[Aa1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ab1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ac1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")    		, 	ADDR(g_Sysparam.AxisParam[Ad1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("�����ٶ�")  		, 	ADDR(g_Sysparam.AxisParam[Ae1].fBackSpeed)				, {0.1,1000.0}				,NULL	,0	,0,NULL	,0,STRRANGE("������ٶ�.��λ:��/��(����/��).��Χ(0.1-1000).")},

	//009��  ����ٱ�
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�"), 	ADDR(g_Sysparam.AxisParam[Ax0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x03	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�") 	, 	ADDR(g_Sysparam.AxisParam[Ay0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x13	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")    	,	ADDR(g_Sysparam.AxisParam[Az0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x23	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")   	, 	ADDR(g_Sysparam.AxisParam[Aa0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x33	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")    	, 	ADDR(g_Sysparam.AxisParam[Ab0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x43	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")    	, 	ADDR(g_Sysparam.AxisParam[Ac0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x53	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")  	, 	ADDR(g_Sysparam.AxisParam[Ad0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x63	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")   	, 	ADDR(g_Sysparam.AxisParam[Ae0].fGearRate)				, {1,1000.0}				,GearParamProc	,0x73	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")	  	, 	ADDR(g_Sysparam.AxisParam[Ax1].fGearRate)				, {1,1000.0}				,GearParamProc	,0x83	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�") 	, 	ADDR(g_Sysparam.AxisParam[Ay1].fGearRate)				, {1,1000.0}				,GearParamProc	,0x93	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")    	,	ADDR(g_Sysparam.AxisParam[Az1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xa3	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")   	, 	ADDR(g_Sysparam.AxisParam[Aa1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xb3	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")    	, 	ADDR(g_Sysparam.AxisParam[Ab1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xc3	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")    	, 	ADDR(g_Sysparam.AxisParam[Ac1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xd3	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")  	, 	ADDR(g_Sysparam.AxisParam[Ad1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xe3	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("���ٱ�")   	, 	ADDR(g_Sysparam.AxisParam[Ae1].fGearRate)				, {1,1000.0}				,GearParamProc	,0xf3	,0,NULL	,0,STRRANGE("�������ٱ�.��λ:��ֵ.��Χ(1-1000).")},

	//010��  �����з���
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")	 , 	ADDR(AxParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x20, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")   		, 	ADDR(AyParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x21, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		,	ADDR(AzParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x22, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")   		, 	ADDR(AaParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x23, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		, 	ADDR(AbParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x24, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")   		, 	ADDR(AcParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x25, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		, 	ADDR(AdParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x26, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		, 	ADDR(AeParam.iRunDir)			, {0, 1}				,AxisParamProc	,0x27, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")	  		, 	ADDR(Ax1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x28, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")   		, 	ADDR(Ay1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x29, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		,	ADDR(Az1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2a, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")   		, 	ADDR(Aa1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2b, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		, 	ADDR(Ab1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2c, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")   		, 	ADDR(Ac1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2d, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		, 	ADDR(Ad1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2e, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���з���")    		, 	ADDR(Ae1Param.iRunDir)			, {0, 1}				,AxisParamProc	,0x2f, 0,NULL,0,STRRANGE("�����߼�.(0:����,1:����).")},

	//011��  ���ԭ�㷽��
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")	  		, 	ADDR(AxParam.iBackDir)	, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")   		, 	ADDR(AyParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		,	ADDR(AzParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")   		, 	ADDR(AaParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		, 	ADDR(AbParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")   		, 	ADDR(AcParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		, 	ADDR(AdParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		, 	ADDR(AeParam.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")	  		, 	ADDR(Ax1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")   		, 	ADDR(Ay1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		,	ADDR(Az1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")   		, 	ADDR(Aa1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		, 	ADDR(Ab1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")   		, 	ADDR(Ac1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		, 	ADDR(Ad1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("���㷽��")    		, 	ADDR(Ae1Param.iBackDir)			, {0, 1}				,NULL	,0	,0,NULL	,0,STRRANGE("���㷽������.(0:����,1:����).")},

	//012��  �Ὺ��
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��"), 	ADDR(AxParam.iAxisSwitch)           , {0, 1}			,SwitchParamProc	,0x00	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")   		, 	ADDR(AyParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x10	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		,	ADDR(AzParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x20	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")   		, 	ADDR(AaParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x30	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		, 	ADDR(AbParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x40	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")   		, 	ADDR(AcParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x50	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		, 	ADDR(AdParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x60	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		, 	ADDR(AeParam.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x70	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")	  		, 	ADDR(Ax1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x80	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")   		, 	ADDR(Ay1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0x90	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		,	ADDR(Az1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xa0	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")   		, 	ADDR(Aa1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xb0	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		, 	ADDR(Ab1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xc0	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")   		, 	ADDR(Ac1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xd0	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		, 	ADDR(Ad1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xe0	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("�Ὺ��")    		, 	ADDR(Ae1Param.iAxisSwitch)		, {0, 1}			,SwitchParamProc	,0xf0	,0,NULL	,0,STRRANGE("�Ὺ��:0-�ر�,1-����")},

	//013��  ��������㷽ʽ
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")	  		, 	ADDR(AxParam.iSearchZero), {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")   		, 	ADDR(AyParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		,	ADDR(AzParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")   		, 	ADDR(AaParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		, 	ADDR(AbParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")   		, 	ADDR(AcParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		, 	ADDR(AdParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		, 	ADDR(AeParam.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")	  		, 	ADDR(Ax1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")   		, 	ADDR(Ay1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		,	ADDR(Az1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")   		, 	ADDR(Aa1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		, 	ADDR(Ab1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")   		, 	ADDR(Ac1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		, 	ADDR(Ad1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},
	{RW_SUPER, DT_INT8U	, 0, 	STR("������㷽ʽ")    		, 	ADDR(Ae1Param.iSearchZero)			, {0, 2}				,NULL	,0	,0,NULL	,0,STRRANGE("0:������,1:���еԭ��,2:��Ȧ���Ա���������")},

	//014��  ���˶����� 0-��Ȧ 1-��Ȧ2-˿�� 3-����
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")	, 	ADDR(AxParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")   		, 	ADDR(AyParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		,	ADDR(AzParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")   		, 	ADDR(AaParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		, 	ADDR(AbParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")   		, 	ADDR(AcParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		, 	ADDR(AdParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		, 	ADDR(AeParam.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")	  		, 	ADDR(Ax1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")   		, 	ADDR(Ay1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		,	ADDR(Az1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")   		, 	ADDR(Aa1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		, 	ADDR(Ab1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")   		, 	ADDR(Ac1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		, 	ADDR(Ad1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("�˶�����")    		, 	ADDR(Ae1Param.iAxisRunMode)		, {0, 3}		,AxisRunModeProc	,0	,0,NULL	,0,STRRANGE("�˶�����:0-��Ȧ��ʽ,1-��Ȧ��ʽ,2-˿�˷�ʽ,3-���߷�ʽ.")},

	//015��  ��ӹ������ģʽ
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")	  		, 	ADDR(AxParam.iBackMode)	, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")   		, 	ADDR(AyParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		,	ADDR(AzParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")   		, 	ADDR(AaParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		, 	ADDR(AbParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")   		, 	ADDR(AcParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		, 	ADDR(AdParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		, 	ADDR(AeParam.iBackMode)				, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")	  		, 	ADDR(Ax1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")   		, 	ADDR(Ay1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		,	ADDR(Az1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")   		, 	ADDR(Aa1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		, 	ADDR(Ab1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")   		, 	ADDR(Ac1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		, 	ADDR(Ad1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},
	{RW_USER, DT_INT16S	, 0, 	STR("�ӹ������ģʽ")    		, 	ADDR(Ae1Param.iBackMode)			, {0, 4}				,BackModeProc	,0	,0,NULL	,0,STRRANGE("�ӹ������ķ�ʽ.(0:�ͽ�,1:����,2:����,3:����,4:������).")},

	//016�� �綯����
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")	  		, 	ADDR(AxParam.iStepDis)	, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")   		, 	ADDR(AyParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		,	ADDR(AzParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")   		, 	ADDR(AaParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		, 	ADDR(AbParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")   		, 	ADDR(AcParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		, 	ADDR(AdParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		, 	ADDR(AeParam.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")	  		, 	ADDR(Ax1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")   		, 	ADDR(Ay1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		,	ADDR(Az1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")   		, 	ADDR(Aa1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		, 	ADDR(Ab1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")   		, 	ADDR(Ac1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		, 	ADDR(Ad1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},
	{RW_SUPER, DT_FLOAT	, 0, 	STR("��綯����")    		, 	ADDR(Ae1Param.iStepDis)			, {0.01,5000}				,NULL	,0	,0,NULL	,0,STRRANGE("�綯����:��λ���׻��,˿��/���� �Ŵ�ģʽ:1����0.01���� ԭֵģʽ:1����1���ף���Ȧ/��Ȧ�Ŵ�ģʽ:1����0.1��  ԭֵģʽ:1����1��")},

	//017�� ��ҡ����
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")	  		, 	ADDR(AxParam.iStepRate)		, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")   		, 	ADDR(AyParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		,	ADDR(AzParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")   		, 	ADDR(AaParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		, 	ADDR(AbParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")   		, 	ADDR(AcParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		, 	ADDR(AdParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		, 	ADDR(AeParam.iStepRate)				, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")	  		, 	ADDR(Ax1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")   		, 	ADDR(Ay1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		,	ADDR(Az1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")   		, 	ADDR(Aa1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		, 	ADDR(Ab1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")   		, 	ADDR(Ac1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		, 	ADDR(Ad1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},
	{RW_SUPER, DT_INT16S	, 0, 	STR("��ҡ����")    		, 	ADDR(Ae1Param.iStepRate)			, {1,99}			,NULL	,0	,0,NULL	,0,STRRANGE("��ҡ����:����Ϊ��ʱ�ı�����1-99")},


	//018��  ������������λ
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AxParam.fMaxLimit)	, {-999999,999999}		,LimtParamProc			,0x00	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AxParam.fMinLimit)	, {-999999,999999}		,LimtParamProc			,0x01	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AyParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x10	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AyParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x11	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AzParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x20	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AzParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x21	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AaParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x30	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AaParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x31	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AbParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x40	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AbParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x41	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AcParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x50	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AcParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x51	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AdParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x60	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AdParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x61	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AeParam.fMaxLimit)				, {-999999,999999}		,LimtParamProc			,0x70	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(AeParam.fMinLimit)				, {-999999,999999}		,LimtParamProc			,0x71	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ax1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0x80	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ax1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0x81	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ay1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0x90	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ay1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0x91	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Az1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xa0	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Az1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xa1	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Aa1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xb0	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Aa1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xb1	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ab1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xc0	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ab1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xc1	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ac1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xd0	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ac1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xd1	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ad1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xe0	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ad1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xe1	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ae1Param.fMaxLimit)			, {-999999,999999}		,LimtParamProc			,0xf0	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},
	{RW_USER, DT_FLOAT   	, 0, 	STR("��������λ")		, 	ADDR(Ae1Param.fMinLimit)			, {-999999,999999}		,LimtParamProc			,0xf1	,0,NULL	,0,STRRANGE("��λ:0.01����(˿��/����);0.1��(��Ȧ/��Ȧ).��Χ(-999999~999999).")},

	//019�� ��������Ծ��Ա�﷽ʽ(0���.��1����)
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")		    , 	ADDR(AxParam.iAxisMulRunMode)		, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AyParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AzParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AaParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AbParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AcParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AdParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(AeParam.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")		    , 	ADDR(Ax1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Ay1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Az1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Aa1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Ab1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Ac1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Ad1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},
	{RW_USER, DT_INT16S 	, 0, 	STR("���ݱ�﷽ʽ")			, 	ADDR(Ae1Param.iAxisMulRunMode)					, {0, 1}	,AxisMulRunModeProc	,0	,0,NULL	,0,STRRANGE("������Ϊ��Ȧ,��Ȧ,˿��ģʽʱ,����λ�ñ�﷽ʽ.0:���,1:����.")},

	//020�� �ŷ����ͣ�0 �᲻����;1������������;10-19����Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39������A5B�����ŷ�
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����"), 	ADDR(AxParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AyParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AzParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AaParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AbParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AcParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AdParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(AeParam.AxisNodeType)   , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Ax1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Ay1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Az1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Aa1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Ab1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Ac1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Ad1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},
	{RW_USER, DT_INT32S 	, 0, 	STR("�ŷ�����")		, 	ADDR(Ae1Param.AxisNodeType)  , {0, 4096}	,NULL	,0,0,NULL,0,STRRANGE("(0 �᲻����;1������������;10-19:��Ϊ��QXE�ŷ�;20-29:��ʱ�������ŷ�;30-39:����A5B�����ŷ�;40-49:ɽ�������ŷ�;50-59:�㴨�����ŷ�,60-69:E5ϵ�����߲���,70-79:֮ɽ�����ŷ�)")},

	//021�� �����˳��
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")	, 	ADDR(AxParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x00	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).������������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AyParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x10	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AzParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x20	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AaParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x30	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AbParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x40	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AcParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x50	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AdParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x60	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(AeParam.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x70	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")		    , 	ADDR(Ax1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x80	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Ay1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0x90	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Az1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xa0	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Aa1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xb0	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Ab1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xc0	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Ac1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xd0	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Ad1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xe0	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�����˳��")			, 	ADDR(Ae1Param.iBackOrder)					, {1,15}				,BackOrderParamProc			,0xf0	,0,NULL				,0,STRRANGE("�����˳��.��Χ(1-15).˳�����ģʽ�²�����������������ͬ��˳��(���������)!")},

	//022��  ��Ӳ��λʹ�����õ�ƽ����
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")	, 	ADDR(AxParam.LIMITConfPEna)   					, {0, 1}				,AxisParamProc	,0x10,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")	, 	ADDR(AxParam.LIMITConfMEna)   					, {0, 1}				,AxisParamProc	,0x40,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")	        ,  	ADDR(AyParam.LIMITConfPEna)   					    , {0, 1}				,AxisParamProc	,0x11,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")	        ,  	ADDR(AyParam.LIMITConfMEna)   					    , {0, 1}				,AxisParamProc	,0x41,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AzParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x12,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AzParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x42,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AaParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x13,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AaParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x43,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AbParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x14,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AbParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x44,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AcParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x15,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AcParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x45,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AdParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x16,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AdParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x46,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AeParam.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x17,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(AeParam.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x47,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ax1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x18,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ax1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x48,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			,  	ADDR(Ay1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x19,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			,  	ADDR(Ay1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x49,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Az1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1a,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Az1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4a,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Aa1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1b,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Aa1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4b,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ab1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1c,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ab1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4c,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ac1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1d,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ac1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4d,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ad1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1e,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ad1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4e,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ae1Param.LIMITConfPEna)   						, {0, 1}				,AxisParamProc	,0x1f,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},
	{RW_USER, DT_INT8U 	, 0, 	STR("��Ӳ��λʹ��")			, 	ADDR(Ae1Param.LIMITConfMEna)   						, {0, 1}				,AxisParamProc	,0x4f,0,NULL		,0,STRRANGE("�Ƿ����ø����Ӳ����λ��⹦��.1:����,0:������.")},

	// ���ŷ����λ��  ��λ������������������ֵ
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AxParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x00	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AyParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x10	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AzParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x20	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AaParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x30	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AbParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x40	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AcParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x50	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AdParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x60	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(AeParam.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x70	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Ax1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x80	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Ay1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0x90	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Az1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xa0	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Aa1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xb0	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Ab1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xc0	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Ac1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xd0	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Ad1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xe0	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},
	{RW_SUPER, DT_INT32S , 0, 	STR("�ŷ����λ��")	, 	ADDR(Ae1Param.lSlaveZero)	, {0, 199999999}	,SlaveZeroSet	,0xf0	,0,NULL	,0,STRRANGE("��Ȧ���Ա��������λ��(Pulse)")},

	//�ŷ���������Ƿ�ʹ��
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AxParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AyParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AzParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AaParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AbParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AcParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AdParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(AeParam.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Ax1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Ay1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Az1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Aa1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Ab1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Ac1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Ad1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},
	{RW_USER, DT_INT8U 	, 0, 	STR("�ŷ��������ʹ��")		,   ADDR(Ae1Param.ServoAlarmValid), {0,1}		   ,NULL	,0	,0,NULL	,0,STRRANGE("0-�ر� 1-ʹ��")},


	{RO_USER, DT_NONE  	, 0, ("")},	//�����ò�������

	{RO_USER, DT_END   , 0, STR("������־"), NULL},	//����
};

/******************************************************************************
                               	   ������ַ����
******************************************************************************/

void ParaManage_Init(void)
{
	int i, j;
	INT16U addr;
	TParamTable *p;

	p = (TParamTable *)ParaRegTab;

	addr = rPARA_BASEADDR;
	for(i=0,j=0; p[i].DataType!=DT_END; i++)
	{
		if(p[i].DataType == DT_TITLE)
		{
			addr = rPARA_BASEADDR + 600*j;
			j++;
		}

		switch(p[i].DataType)
		{
			case DT_INT8U:
			case DT_INT8S:
			case DT_INT16U:
			case DT_INT16S:

				p[i].PlcAddr = addr;
				addr += 1;
				break;
			case DT_INT32U:
			case DT_INT32S:
			case DT_FLOAT:
			case DT_PPS:
				p[i].PlcAddr = addr;
				addr += 2;
				break;
			default:
				break;
		}
	}
}


