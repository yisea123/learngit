
// Spring.cpp : 定义应用程序的类行为。
//
//

#include "stdafx.h"
#include <locale.h>                   // setlocale函数的头文件  
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

// 防止程序多开
HANDLE h_single_instance;

// CSpringApp 构造

CSpringApp::CSpringApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

#ifdef _DEBUG_OFFLINE
	g_bIsDebugOffline=true; // 强制modbus离线（单机模式），用于调试界面，不从下位机读取参数
	//g_Sysparam.iMachineCraftType=2; // 带有结束角
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
	
	delete_pointer(g_logger); // 关闭日志
	delete_pointer(g_mbq); // 这个modbus查询队列应该是最后删除，否则出现死线程在查询modbus。
}

// 唯一的一个 CSpringApp 对象

CSpringApp theApp;


// CSpringApp 初始化

BOOL CSpringApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	setlocale(LC_ALL, "chs"); 

	CWinApp::InitInstance();

#ifdef _DEBUG
	if(!AllocConsole()) // 创建调试console
		AfxMessageBox(_T("Fail to create the console!"),MB_ICONEXCLAMATION);
	debug_printf("Hello, I am the debug console!\n");
#endif

	// Windows版本检测
	if(FALSE==isWindowsVistaOrGreater()){
		AfxMessageBox(_T("请使用Windows Vista以上的操作系统"));
		return FALSE;
	}

	// 防止程序多开
	h_single_instance = CreateMutex(NULL, TRUE, _T("这是一个单实例应用"));
	if(ERROR_ALREADY_EXISTS == GetLastError()){
		AfxMessageBox(_T("本程序已经运行"));
		return FALSE;
	}

	// 初始化全局对象
	g_logger=LoggerEx::getInstance(); // 开启日志
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

	// 是否启用日志
	g_logger->setEnable(g_softParams->params.isEnableLogger);

	if(IDOK==nResponse){
		g_logger->log(LoggerEx::INFO,"打开软件");
		CSpringDlg dlg;
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// ?
		}
		g_logger->log(LoggerEx::INFO,"退出软件");
	}

	// 保存参数
	g_softParams->saveAllParamsToIni();

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。

	return FALSE;
}

