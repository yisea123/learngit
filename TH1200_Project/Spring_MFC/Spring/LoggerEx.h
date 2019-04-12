#pragma once
#include "logger.h"
#include <string>

class LoggerEx :
	public Logger
{
public:
	LoggerEx(void);
	~LoggerEx(void);

	static LoggerEx* getInstance(); // ֻ����һ����̬ʵ������
	
	const char* getLogFileName(){return filename;}

	// ��־�ȼ�
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
	static LoggerEx* m_Instance; // ֻ����һ����̬ʵ������
};

