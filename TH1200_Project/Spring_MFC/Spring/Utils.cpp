#include "StdAfx.h"
#include "Utils.h"
#include "Definations.h"
#include "Data.h"
#include <stdarg.h> // va_start


#include "SysText.h"

bool isModbusPending(int counterModbusPending, CDialogEx* context)
{
	if(counterModbusPending>0){
		context->MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_BUSY),
			g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR),
			MB_OK);
		
		return true;
	}else
		return false;
}

bool CStringToChars(CString cstr,char *chr, size_t max_size)
{
	int len=cstr.GetLength();
	int nbytes=WideCharToMultiByte(CP_ACP,0,cstr,len,NULL,0,NULL,NULL);

	// 确保不越界
	if((size_t)nbytes+1 > max_size)
		return false;

	memset(chr,0,nbytes+1);
	WideCharToMultiByte(CP_ACP,0,cstr,len,chr,nbytes,NULL,NULL);
	chr[nbytes]=0;

	return true;
}

CString CharsToCString(const char *chr)
{
	return CString(chr);
}

std::string CStringToString(CString src)
{
	USES_CONVERSION;

	return std::string(W2A(src));
}

CString StringToCString(const std::string& str)
{
	return CString(str.c_str());
}

bool CStringToWchar(CString cstring, wchar_t* wchar, size_t max_size)
{
	std::wstring s1(cstring);
	const wchar_t* wchar_str = s1.c_str();
	size_t nbytes=wcslen(wchar_str);

	// 确保不越界
	if((size_t)nbytes+1 > max_size)
		return false;

	wcscpy_s(wchar,max_size,wchar_str);
	return true;
}

CString WcharToCString(wchar_t* wchar)
{
	std::wstring std_wstring(wchar);
	CString cstring(std_wstring.c_str());

	return cstring;
}

bool CharsToWchars(const char* src, wchar_t* dest, size_t max_size){
	int nbytes=MultiByteToWideChar(CP_ACP,0,src,-1,NULL,0);
	
	// 确保不越界
	if((size_t)nbytes+1 > max_size)
		return false;

	MultiByteToWideChar(CP_ACP,0,src,-1,dest,nbytes);
	return true;
}

bool WcharsToChars(const wchar_t* src, char* dest, size_t max_size){
	int nbytes=WideCharToMultiByte(CP_ACP,NULL,src,-1,NULL,0,NULL,FALSE);
	
	// 确保不越界
	if((size_t)nbytes+1 > max_size)
		return false;

	WideCharToMultiByte(CP_ACP,NULL,src,-1,dest,nbytes,NULL,FALSE);
	return true;
}

CString GetAppRunPath()
{
	// return current Program dir
	char curPath[MAX_PATH]={0};
	CString strCurPath;

	::GetModuleFileNameW(NULL,(LPWSTR)curPath,MAX_PATH);
	strCurPath.Format(_T("%s"),curPath);
	int n=strCurPath.ReverseFind('\\');
	strCurPath=strCurPath.Left(n+1);
	return strCurPath;
}

CString GetLocalAppDataPath()
{
	CString appath;
	TCHAR wzLacalAppData[MAX_PATH]={0};

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, wzLacalAppData)))
		appath = wzLacalAppData;
	else
		return CString(); // error

	auto createDir=[](const CString& dir)->BOOL{
		CFileStatus fileStatus;
		BOOL isOk=FALSE;
		if(!CFile::GetStatus(dir, fileStatus)) {
			isOk=::CreateDirectory(dir, NULL);
		}
		return isOk;
	};

	//default DIR: /ADTECH/Spring
	appath+=_T("\\ADTECH");
	createDir(appath);
	appath+=_T("\\Spring");
	createDir(appath);
	appath+=_T("\\");

	return appath;
}

void threadLoopTask_start(threadLoopTask_param_t* p)
{
#ifdef _DEBUG
	VERIFY(nullptr != p->context);
	VERIFY(nullptr != p->threadLoopTaskRun);
#else
	if(nullptr == p->context || nullptr == p->threadLoopTaskRun)
		return;
#endif
	if(true==p->isNeedAsynchronousKill){
		p->hExitRequest=CreateEvent(NULL, FALSE, FALSE, NULL);
		p->hExitResponse=CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	p->returnCode=0; // 默认返回值0，表示正常返回
	AfxBeginThread(p->threadLoopTaskRun, (LPVOID)p ); // <<== START THE THREAD
}

bool threadLoopTask_stopCheckRequest(threadLoopTask_param_t* p)
{
#ifdef _DEBUG
	VERIFY(nullptr != p->context);
	VERIFY(nullptr != p->threadLoopTaskRun);
#else
	if(nullptr == p->context || nullptr == p->threadLoopTaskRun)
		return false;
#endif
	if(false==p->isNeedAsynchronousKill)
		return false;

	if(WaitForSingleObject(p->hExitRequest, 0) == WAIT_OBJECT_0) // 收到了终止线程请求
		return true;
	else
		return false;
}

void threadLoopTask_stopResponse(threadLoopTask_param_t* p)
{
#ifdef _DEBUG
	VERIFY(nullptr != p->context);
	VERIFY(nullptr != p->threadLoopTaskRun);
#else
	if(nullptr == p->context || nullptr == p->threadLoopTaskRun)
		return;
#endif
	if(false==p->isNeedAsynchronousKill)
		return;

	SetEvent(p->hExitResponse);
}

bool threadLoopTask_stop(threadLoopTask_param_t* p, int timeout)
{
#ifdef _DEBUG
	VERIFY(nullptr != p->context);
	VERIFY(nullptr != p->threadLoopTaskRun);
#else
	if(nullptr == p->context || nullptr == p->threadLoopTaskRun)
		return false;
#endif
	if(false==p->isNeedAsynchronousKill)
		return false;

	SetEvent(p->hExitRequest);

	if(WAIT_TIMEOUT == WaitForSingleObject(p->hExitResponse, timeout)){
		return false; // You shouldn't go here!
	}else{
		CloseHandle(p->hExitRequest);
		CloseHandle(p->hExitResponse);
		return true;
	}
}

bool threadLoopTask_wait(threadLoopTask_param_t* p, int timeout)
{
#ifdef _DEBUG
	VERIFY(nullptr != p->context);
	VERIFY(nullptr != p->threadLoopTaskRun);
#else
	if(nullptr == p->context || nullptr == p->threadLoopTaskRun)
		return false;
#endif
	if(false==p->isNeedAsynchronousKill)
		return false;

	bool isOk;

	if(timeout>0){ // 超时等待
		if(WAIT_TIMEOUT == WaitForSingleObject(p->hExitResponse, timeout))
			isOk=false;
		else
			isOk=true;
	}else{ // 无限等待
		if(WAIT_OBJECT_0 != WaitForSingleObject(p->hExitResponse, INFINITE))
			isOk=false;
		else
			isOk=true;
	}

	CloseHandle(p->hExitRequest);
	CloseHandle(p->hExitResponse);
	return isOk;
}

#if 0
void SetUnitPulse(short ch,double mm,int pulse)
{
	if( mm < 0.001 ) 
		mm = 1.0;
		
	if( pulse < 200) 
		pulse = 200;
		
	g_fPulseOfMM[ch-1] = (double)(pulse)/mm;
	g_fPulseOfAngle [ch-1] = (double)(pulse)/360.0;
	g_lPulseOfCircle[ch-1] = pulse;
}

float	GetMMFromPulse(short ch,int pulse)
{
	if(ch > AXIS_NUM)
		return 0.0f;
		
	return  (float)(pulse)/g_fPulseOfMM[ch-1];
}

int	GetPulseFromMM(short ch,float mm)
{
	if(ch > AXIS_NUM)
		return 0;
		
	if(mm*g_fPulseOfMM[ch-1]>0.000001)
		return (int)(mm*g_fPulseOfMM[ch-1]+0.5);
	else
		return (int)(mm*g_fPulseOfMM[ch-1]-0.5);
}

float	GetAngleFromPulse(short ch,int pulse)
{
	if(ch > AXIS_NUM)
		return 0.0f;
	
	pulse = (pulse)%g_lPulseOfCircle[ch-1];

	return (float)(pulse)/g_fPulseOfAngle[ch-1];
}

int	GetPulseFromAngle(short ch,float angle)
{
	if(ch > AXIS_NUM)
		return 0;
		
	if(angle*g_fPulseOfAngle[ch-1]>0.000001)
		return (int)(angle*g_fPulseOfAngle[ch-1]+0.5);
	else
		return (int)(angle*g_fPulseOfAngle[ch-1]-0.5);
}

float	GetMMFromAngle(short ch,float angle)
{
	if(ch > AXIS_NUM)
		return 0.0f;
		
	return g_fPulseOfAngle[ch-1]*angle/g_fPulseOfMM[ch-1];
}

float	GetAngleFromMM(short ch,float mm)
{
	if(ch > AXIS_NUM)
		return 0.0f;
		
	return g_fPulseOfMM[ch-1]*mm/g_fPulseOfAngle[ch-1];
}
#endif

DWORD GetTickCountDelta(DWORD t1,DWORD t2)
{
	if(t1>t2)
		return t1-t2;
	else
		return t2-t1;
}

std::string getIniFileName(const std::string& subdir, const std::string& filename)
{
	std::string str;
	std::string appPath=CStringToString(GetLocalAppDataPath());
	
	if(subdir.length()>0){
		CFileStatus status;
		CString _subdir = GetLocalAppDataPath() + StringToCString(subdir);
		
		if(!CFile::GetStatus(_subdir, status)) {
			::CreateDirectory(_subdir, NULL);
		}

		str = appPath + subdir + std::string("\\") + filename;
	}else{
		str = appPath + filename;
	}

	return str;
}

bool readWriteModbus(void* data, unsigned short len, unsigned short addr, bool isRead, unsigned int timeout)
{
	modbusQ_push_t mbq;

	mbq.data=data;
	mbq.len=len;
	mbq.operation=isRead?MODBUSQ_OP_READ_DATA:MODBUSQ_OP_WRITE_DATA;
	mbq.start_addr=addr;
	
	return g_mbq->sendQuery(mbq,false,true,timeout);
}

BOOL isWindowsVistaOrGreater()
{
	// 检查当前windows版本是否为XP及以上demo：https://msdn.microsoft.com/en-us/library/windows/desktop/ms725491(v=vs.85).aspx
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op=VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 0;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

	// Initialize the condition mask.
	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

	// Perform the test.
	return VerifyVersionInfo(
	  &osvi,
	  VER_MAJORVERSION | VER_MINORVERSION |
	  VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
	  dwlConditionMask);
}

bool checkPrivilege(CDialogEx* context, int min_level)
{
	if(g_currentUser<min_level){
		context->MessageBox(g_lang->getString("HINT_CURRENT_USER",CLanguage::SECT_USER_NAMES)+
			g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES)+
			_T("\n")+
			g_lang->getString("HINT_PLEASE_CHANGE_USER",CLanguage::SECT_USER_NAMES)+
			g_lang->getStringById(min_level, CLanguage::SECT_USER_NAMES));
		return false;
	}

	return true;
}

bool isFileExists(CString file)
{
	DWORD dwAttrib = GetFileAttributes(file);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void setCurrentDialogFont(CWnd* dlg, CFont* m_pFontDefault)
{
	if(nullptr==m_pFontDefault)
		return;

	CWnd* pWnd=dlg->GetTopWindow();

	while(pWnd!=NULL){
		dlg->GetDlgItem(pWnd->GetDlgCtrlID())->SetFont(m_pFontDefault);
		pWnd=pWnd->GetNextWindow();
	}
}

CString getStringFromCoord(int index_axis, float pos, char delimiter)
{
	CString str=_T("");
	int circle,angle; // 多圈显示：整圈数

	switch(g_Sysparam.AxisParam[index_axis].iAxisRunMode){ // 轴运行模式
	case 2: // 丝杆
	case 3: // 送线
		if(0 == g_Sysparam.UnpackMode)
			str.Format(_T("%d"),static_cast<int>(pos));
		else
			str.Format(_T("%.2f"),static_cast<double>(pos) / 100.0);
		break;

	case 0: // 单圈
		if(0 == g_Sysparam.UnpackMode)
			str.Format(_T("%d"),static_cast<int>(pos));
		else
			str.Format(_T("%.2f"),static_cast<double>(pos) / 10.0);
				
		break;

	case 1: // 多圈
		circle=static_cast<int>(pos);
		angle =abs(static_cast<int>((pos - circle) * 10000));

		if(pos>-0.00001)
			str.Format(_T("%d%c%04d"),circle,delimiter,angle);
		else
			str.Format(_T("-%d%c%04d"),-circle,delimiter,angle);

		break;

	default:
		break;
	}

	return str;
}

void initMapColumnToTeach()
{
	int mapColumnToAxisNum;
	int mapColumnToTeachIndex;

	switch(g_Sysparam.iMachineCraftType){
	case 2: // 带 起始角+结束角
		for(int col=0;col<MAX_COLUMN_LISTCTRL;++col){
			// 相互映射（列号 <-> 轴号）
			if(2 == col){ // 第1轴 开始角
				g_mapAxisNumToColumn[0]=col;
				mapColumnToAxisNum=1;
			}else if(3==col) // 第1轴 结束角
				mapColumnToAxisNum=1;
			else if(col>3 && col<AXIS_NUM+3){ // 第2轴~实际轴数
				g_mapAxisNumToColumn[col-3]=col; // g_mapAxisNumToColumn数组下标0表示第一个轴
				mapColumnToAxisNum=col-2;
			}else
				mapColumnToAxisNum=0;

			// 相互映射（列号 <-> 教导结构体TEACH_COMMAND）
			if(col>3) // 第2轴 到 延时
				mapColumnToTeachIndex=col-2;
			else if(3==col) // 第1轴 结束角
				mapColumnToTeachIndex=21;
			else if(2==col) // 第1轴 开始角
				mapColumnToTeachIndex=1;
			else if(1==col) // 指令
				mapColumnToTeachIndex=0;
			else // 行号
				mapColumnToTeachIndex=-1;

			g_mapColumnToAxisNum[col]=mapColumnToAxisNum;
			g_mapColumnToTeachIndex[col]=mapColumnToTeachIndex;

			if(mapColumnToTeachIndex >= 0 && mapColumnToTeachIndex < MAXVALUECOL)
				g_mapTeachIndexToColumn[mapColumnToTeachIndex]=col;
		}
		break;

	case 0: // 无起始角结束角
	case 1:
	default:
		for(int col=0;col<MAX_COLUMN_LISTCTRL;++col){
			// 相互映射（列号 <-> 轴号）
			if(col>1 && col<AXIS_NUM+2){
				mapColumnToAxisNum=col-1;
				g_mapAxisNumToColumn[col-2]=col; // g_mapAxisNumToColumn数组下标0表示第一个轴
			}else
				mapColumnToAxisNum=0;

			// 相互映射（列号 <-> 教导结构体TEACH_COMMAND）
			mapColumnToTeachIndex=col-1;
			
			g_mapColumnToAxisNum[col]=mapColumnToAxisNum;
			g_mapColumnToTeachIndex[col]=mapColumnToTeachIndex;

			if(mapColumnToTeachIndex >= 0 && mapColumnToTeachIndex < MAXVALUECOL)
				g_mapTeachIndexToColumn[mapColumnToTeachIndex]=col;
		}
		break;
	}
}