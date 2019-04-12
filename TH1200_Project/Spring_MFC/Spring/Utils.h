#pragma once

#include <conio.h>
#include "ListCtrlEx_Settings.h"
#include "Definations.h"
#include <GdiPlus.h>
#include <string>

// 调试用，输出到windows屏幕终端
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

// 获取AppLocal目录，返回值如：'C:\Users\USERNAME\AppData\Local\ADTECH\Spring\'
CString GetLocalAppDataPath();
// 获取App运行目录，返回值如：'C:\Program Files(x86)\ADTECH\Spring\'
CString GetAppRunPath();

// 比较大小模板函数
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

// 自己封装的线程函数 lixingcong@20171022
void threadLoopTask_start(threadLoopTask_param_t* p);
bool threadLoopTask_stopCheckRequest(threadLoopTask_param_t* p);
void threadLoopTask_stopResponse(threadLoopTask_param_t* p);
bool threadLoopTask_stop(threadLoopTask_param_t* p, int timeout);
bool threadLoopTask_wait(threadLoopTask_param_t* p, int timeout);

// 设置脉冲当量
#if 0
void SetUnitPulse(short ch,double mm,int pulse);
float	GetMMFromPulse(short ch,int pulse);
int	GetPulseFromMM(short ch,float mm);
float	GetAngleFromPulse(short ch,int pulse);
int	GetPulseFromAngle(short ch,float angle);
float	GetMMFromAngle(short ch,float angle);
float	GetAngleFromMM(short ch,float mm);
#endif

// 回收内存模板函数，使用指针的引用来修改指针为NULL
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

// 阻塞方式 读写modbus
bool readWriteModbus(void* data, unsigned short len, unsigned short addr, bool isRead=true, unsigned int timeout=0);

// 是否为Vista以上的操作系统
BOOL isWindowsVistaOrGreater();

// 检查权限
bool checkPrivilege(CDialogEx* context, int min_level);

// 文件是否存在
bool isFileExists(CString file);

// 设置整个对话框的字体
void setCurrentDialogFont(CWnd* dlg, CFont* m_pFontDefault);

// 从某个坐标输出到字符串 index_axis为0开始的下标，代表第一个轴
CString getStringFromCoord(int index_axis, float pos, char delimiter='.');

// 根据是否为带有“起始角结束角”的机型，初始化轴映射数组
void initMapColumnToTeach();
