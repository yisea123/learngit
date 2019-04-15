/*
 * Loggger.cpp
 *
 * 日志实现
 *
 * Created on: 2017年12月13日下午9:04:09
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Logger.h"
#include <ctime>
// #include "Utils.h"

#define DEFAULT_SPLIT_CHAR ','

Logger::Logger(std::string filename)
{
	isEnableLogger=true; // 默认开启日志
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
		oFile << "时间" << DEFAULT_SPLIT_CHAR << "日志级别" << DEFAULT_SPLIT_CHAR << "日志信息" << std::endl;
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
	std::string levelMsg="日志状态";
	std::string hint_enable="日志功能开启";
	std::string hint_disable="日志功能关闭";
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