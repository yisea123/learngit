
// Spring.cpp : ����Ӧ�ó��������Ϊ��
//
//

#include "stdafx.h"
#include <locale.h>                   // setlocale������ͷ�ļ�  
#include "Spring.h"
#include "SpringDlg.h"
#include "Utils.h"
#include "Data.h"
#include "StartupDlg.h"
#include "LoggerEx.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpringApp

BEGIN_MESSAGE_MAP(CSpringApp, CWinApp)
	// ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// ��ֹ����࿪
HANDLE h_single_instance;

// CSpringApp ����

CSpringApp::CSpringApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

#ifdef _DEBUG_OFFLINE
	g_bIsDebugOffline=true; // ǿ��modbus���ߣ�����ģʽ�������ڵ��Խ��棬������λ����ȡ����
	//g_Sysparam.iMachineCraftType=2; // ���н�����
#endif
}

CSpringApp::~CSpringApp()
{
	try{
		ReleaseMutex(h_single_instance); // Explicitly release mutex
		CloseHandle(h_single_instance); // close handle before terminating
	}catch(...){
		debug_printf("h_single_instance: Exception was catched\n");
	}

	delete_pointer(g_modelArray);
	delete_pointer(g_lang);
	delete_pointer(g_dog);
	delete_pointer(g_paramTable);
	delete_pointer(g_softParams);
	
	delete_pointer(g_logger); // �ر���־
	delete_pointer(g_mbq); // ���modbus��ѯ����Ӧ�������ɾ��������������߳��ڲ�ѯmodbus��
}

// Ψһ��һ�� CSpringApp ����

CSpringApp theApp;


// CSpringApp ��ʼ��

BOOL CSpringApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	setlocale(LC_ALL, "chs"); 

	CWinApp::InitInstance();

#ifdef _DEBUG
	if(!AllocConsole()) // ��������console
		AfxMessageBox(_T("Fail to create the console!"),MB_ICONEXCLAMATION);
	debug_printf("Hello, I am the debug console!\n");
#endif

	// Windows�汾���
	if(FALSE==isWindowsVistaOrGreater()){
		AfxMessageBox(_T("��ʹ��Windows Vista���ϵĲ���ϵͳ"));
		return FALSE;
	}

	// ��ֹ����࿪
	h_single_instance = CreateMutex(NULL, TRUE, _T("����һ����ʵ��Ӧ��"));
	if(ERROR_ALREADY_EXISTS == GetLastError()){
		AfxMessageBox(_T("�������Ѿ�����"));
		return FALSE;
	}

	// ��ʼ��ȫ�ֶ���
	g_logger=LoggerEx::getInstance(); // ������־
	g_softParams = new SoftwareParams();
	g_paramTable=new ParamTable;
	g_dog=new CDog;
	g_mbq = new ModbusQueue();
	g_modelArray=new CModelArray;

	g_paramTable->init();

	CShellManager *pShellManager = new CShellManager;

	INT_PTR nResponse=IDOK;

	if(false==g_bIsDebugOffline){
		CStartupDlg dlg_startup;
		nResponse=dlg_startup.DoModal();
	}else{
		g_softParams->loadAllParamsFromIni();
		
		g_lang = new CLanguage(getIniFileName("Languages",g_softParams->params.languageFile),
			g_softParams->params.languageFile == std::string("default"));

		g_mbq->init(g_softParams->params.destIPAddr,200,3,MODBUS_UDP_ID);
			
		g_Sysparam.TotalAxisNum=3;
		g_Sysparam.AxisParam[0].iAxisSwitch=1;
		g_Sysparam.AxisParam[1].iAxisSwitch=1;
		g_Sysparam.AxisParam[2].iAxisSwitch=1;
		g_Sysparam.AxisParam[3].iAxisSwitch=1;
		sprintf_s(g_axisName[0],sizeof(g_axisName[0]),"Axis1");
		sprintf_s(g_axisName[1],sizeof(g_axisName[1]),"Axis2");
		sprintf_s(g_axisName[2],sizeof(g_axisName[2]),"Axis3");
		g_bIsModbusFail=true;
		g_currentUser=REG_SUPER;

		g_modelArray->loadDefault();
	}

	// �Ƿ�������־
	g_logger->setEnable(g_softParams->params.isEnableLogger);

	if(IDOK==nResponse){
		g_logger->log(LoggerEx::INFO,"�����");
		CSpringDlg dlg;
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// ?
		}
		g_logger->log(LoggerEx::INFO,"�˳����");
	}

	// �������
	g_softParams->saveAllParamsToIni();

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�

	return FALSE;
}

