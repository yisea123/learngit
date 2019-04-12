/*
 * LogggerEx.cpp
 *
 * 日志派生类：分等级写入
 *
 * Created on: 2017年12月13日下午9:04:09
 * Author: lixingcong
 */
#include "StdAfx.h"
#include "LoggerEx.h"
#include <ctime>
#include "Utils.h"

LoggerEx* LoggerEx::m_Instance=nullptr;

LoggerEx::LoggerEx(void)
	:Logger(getDateFilename())
{
	setLogLevelMsg(INFO,"提示信息");
	setLogLevelMsg(ERROR1,"一般报警");
	setLogLevelMsg(ERROR2,"紧急报警");
	setLogLevelMsg(SETTING_CHANGED,"参数改变");
}

LoggerEx* LoggerEx::getInstance()
{
	if(m_Instance==nullptr)
		m_Instance=new LoggerEx();
	return m_Instance;
}

LoggerEx::~LoggerEx(void)
{
}

std::string LoggerEx::getDateFilename()
{
	//创建Logs文件夹
	CString strCurPath;
	CFileStatus status;
	strCurPath = GetLocalAppDataPath() + _T("Logs");
	if(!CFile::GetStatus(strCurPath, status)) {
		::CreateDirectory(strCurPath, NULL);
	}

	std::time_t t = std::time(nullptr);
	struct tm Tm;

	localtime_s(&Tm,&t);
	std::strftime(filename, sizeof(filename), "Log_%Y-%m-%d.csv", &Tm);

	USES_CONVERSION;
	CString path=GetLocalAppDataPath();
	std::string result=std::string(W2A(path));
	result+="Logs\\"; // 子目录
	result+=filename;
	
	return result;
}
