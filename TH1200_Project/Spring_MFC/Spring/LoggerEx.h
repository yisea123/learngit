#pragma once
#include "logger.h"
#include <string>

class LoggerEx :
	public Logger
{
public:
	LoggerEx(void);
	~LoggerEx(void);

	static LoggerEx* getInstance(); // 只允许一个静态实例运行
	
	const char* getLogFileName(){return filename;}

	// 日志等级
	enum{
		INFO,
		ERROR1,
		ERROR2,
		SETTING_CHANGED,
	};

protected:
	std::string getDateFilename();
	char filename[100];

private:
	static LoggerEx* m_Instance; // 只允许一个静态实例运行
};

