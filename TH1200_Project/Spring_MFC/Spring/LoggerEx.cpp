/*
 * LogggerEx.cpp
 *
 * ��־�����ࣺ�ֵȼ�д��
 *
 * Created on: 2017��12��13������9:04:09
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
	setLogLevelMsg(INFO,"��ʾ��Ϣ");
	setLogLevelMsg(ERROR1,"һ�㱨��");
	setLogLevelMsg(ERROR2,"��������");
	setLogLevelMsg(SETTING_CHANGED,"�����ı�");
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
	//����Logs�ļ���
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
	result+="Logs\\"; // ��Ŀ¼
	result+=filename;
	
	return result;
}
