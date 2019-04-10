#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <Windows.h>
#include <process.h>

class Logger
{
public:
	Logger(std::string filename);
	~Logger(void);
	void log(int logLevel, std::string msg);
	void setLogLevelMsg(int logLevel, std::string msg);
	std::string getLogLevelMsg(int logLevel);
	void setEnable(bool isEnable);

protected: // 成员
	std::string m_filename;
	CRITICAL_SECTION m_Mutex;
	bool isEnableLogger;

protected: // 成员函数
	void writeLine(std::string& levelMsg, std::string& msg);
	std::string getCurrentTime();
	std::map<int,std::string> m_logLevelMessage;
	bool isFileExisted(const std::string& filename);
};

