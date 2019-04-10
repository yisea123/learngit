/*
 * Loggger.cpp
 *
 * ��־ʵ��
 *
 * Created on: 2017��12��13������9:04:09
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Logger.h"
#include <ctime>
// #include "Utils.h"

#define DEFAULT_SPLIT_CHAR ','

Logger::Logger(std::string filename)
{
	isEnableLogger=true; // Ĭ�Ͽ�����־
	m_filename=filename;
	InitializeCriticalSection(&m_Mutex);
}

Logger::~Logger(void)
{
	DeleteCriticalSection(&m_Mutex);
}

void Logger::writeLine(std::string& levelMsg, std::string& msg)
{
	EnterCriticalSection(&m_Mutex);

	bool _isFileExisted=isFileExisted(m_filename);

	std::ofstream oFile;
	oFile.open(m_filename.c_str(), std::ios::out|std::ios::app);

	if(false==_isFileExisted){
		oFile << "ʱ��" << DEFAULT_SPLIT_CHAR << "��־����" << DEFAULT_SPLIT_CHAR << "��־��Ϣ" << std::endl;
	}

	oFile << getCurrentTime() << DEFAULT_SPLIT_CHAR << levelMsg << DEFAULT_SPLIT_CHAR << msg << std::endl;

	oFile.flush();
	oFile.close();

	LeaveCriticalSection(&m_Mutex);
}

void Logger::log(int logLevel, std::string msg)
{
	if(isEnableLogger)
		writeLine(getLogLevelMsg(logLevel),msg);
}

std::string Logger::getCurrentTime()
{
	std::time_t t = std::time(nullptr);
	struct tm Tm;
	char mbstr[100];

	localtime_s(&Tm,&t);
	std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", &Tm);

	return std::string(mbstr);
}

void Logger::setLogLevelMsg(int logLevel, std::string msg)
{
	m_logLevelMessage[logLevel]=msg;
}

std::string Logger::getLogLevelMsg(int logLevel)
{
	std::map<int,std::string>::iterator it;
	const std::string unknownLevel="?";

	it = m_logLevelMessage.find(logLevel);
	if (it != m_logLevelMessage.end())
		return it->second;
	else
		return unknownLevel;
}

void Logger::setEnable(bool isEnable)
{
#if 0
	std::string levelMsg="��־״̬";
	std::string hint_enable="��־���ܿ���";
	std::string hint_disable="��־���ܹر�";
	writeLine(levelMsg,isEnable?hint_enable:hint_disable); // isEnable is the param
#endif
	isEnableLogger=isEnable;
}

bool Logger::isFileExisted(const std::string& filename)
{
	std::ifstream ssin;

	try{
		ssin.open(filename.c_str(), std::ios::ate | std::ios::binary);
	}catch(...){
		return false;
#if 0
		m_filename=DEFAULT_LOG_FILENAME;
		ssin.open(m_filename.c_str(), std::ios::ate | std::ios::binary);
#endif
	}

	// is a new(empty) file?
	int filesize=static_cast<int>(ssin.tellg());
	ssin.close();

	return filesize>0;
}