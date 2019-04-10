#pragma once

#include <conio.h>
#include "ListCtrlEx_Settings.h"
#include "Definations.h"
#include <GdiPlus.h>
#include <string>

// �����ã������windows��Ļ�ն�
#ifdef _DEBUG
#define debug_printf(fmt, ...) \
		do{ \
				_cprintf(fmt, __VA_ARGS__); \
		}while(0)
#else
#define debug_printf(fmt, ...) do{}while(0)
#endif

bool isModbusPending(int counterModbusPending, CDialogEx* context);

bool CStringToChars(CString cstr,char *chr, size_t max_size);
CString CharsToCString(const char *chr);

std::string CStringToString(CString src);
CString StringToCString(const std::string& str);

bool CStringToWchar(CString cstring, wchar_t* wchar, size_t max_size);
CString WcharToCString(wchar_t* wchar);

bool CharsToWchars(const char* src, wchar_t* dest, size_t max_size);
bool WcharsToChars(const wchar_t* src, char* dest, size_t max_size);

// ��ȡAppLocalĿ¼������ֵ�磺'C:\Users\USERNAME\AppData\Local\ADTECH\Spring\'
CString GetLocalAppDataPath();
// ��ȡApp����Ŀ¼������ֵ�磺'C:\Program Files(x86)\ADTECH\Spring\'
CString GetAppRunPath();

// �Ƚϴ�Сģ�庯��
template<class T>
bool checkDataRange(T data, T value_min, T value_max){
	if(data>value_max || data<value_min)
		return false;
	return true;
}

template<class T>
bool checkDataRangeAndSet(T* data, T value_min, T value_max, T value_default){
	if(*data>value_max || *data<value_min){
		*data=value_default;
		return false;
	}
	return true;
}

// �Լ���װ���̺߳��� lixingcong@20171022
void threadLoopTask_start(threadLoopTask_param_t* p);
bool threadLoopTask_stopCheckRequest(threadLoopTask_param_t* p);
void threadLoopTask_stopResponse(threadLoopTask_param_t* p);
bool threadLoopTask_stop(threadLoopTask_param_t* p, int timeout);
bool threadLoopTask_wait(threadLoopTask_param_t* p, int timeout);

// �������嵱��
#if 0
void SetUnitPulse(short ch,double mm,int pulse);
float	GetMMFromPulse(short ch,int pulse);
int	GetPulseFromMM(short ch,float mm);
float	GetAngleFromPulse(short ch,int pulse);
int	GetPulseFromAngle(short ch,float angle);
float	GetMMFromAngle(short ch,float angle);
float	GetAngleFromMM(short ch,float mm);
#endif

// �����ڴ�ģ�庯����ʹ��ָ����������޸�ָ��ΪNULL
// https://stackoverflow.com/questions/41225738/c-template-function-to-delete-and-reset-pointers
template<class T>
void delete_array(T*& ptr)
{
	if(nullptr!=ptr){
		delete[] ptr;
		ptr=nullptr;
	}
}

template<class T>
void delete_pointer(T*& ptr)
{
	if(nullptr!=ptr){
		delete ptr;
		ptr=nullptr;
	}
}

DWORD GetTickCountDelta(DWORD t1,DWORD t2);

std::string getIniFileName(const std::string& subdir, const std::string& filename);

// ������ʽ ��дmodbus
bool readWriteModbus(void* data, unsigned short len, unsigned short addr, bool isRead=true, unsigned int timeout=0);

// �Ƿ�ΪVista���ϵĲ���ϵͳ
BOOL isWindowsVistaOrGreater();

// ���Ȩ��
bool checkPrivilege(CDialogEx* context, int min_level);

// �ļ��Ƿ����
bool isFileExists(CString file);

// ���������Ի��������
void setCurrentDialogFont(CWnd* dlg, CFont* m_pFontDefault);

// ��ĳ������������ַ��� index_axisΪ0��ʼ���±꣬�����һ����
CString getStringFromCoord(int index_axis, float pos, char delimiter='.');

// �����Ƿ�Ϊ���С���ʼ�ǽ����ǡ��Ļ��ͣ���ʼ����ӳ������
void initMapColumnToTeach();
