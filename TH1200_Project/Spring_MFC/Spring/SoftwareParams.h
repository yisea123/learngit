#pragma once

#include "Definations.h"
#include "TestIo_public.h"
#include <string>
#include "IniSettings.h"
#include "StaticImageAxis.h"

typedef struct{
	// modbus目标ip
	char destIPAddr[MAX_LEN_IO_NAME];
	unsigned int modbus_timeout;
	unsigned int modbus_retry_times;

	bool isEnableLogger; // 是否启用日志
	bool isNeedPasswordExiting; // 退出前是否需要密码
	bool isAlwaysOnTop; // 总在最上
	bool isBootWithMaximized; // 最大化启动

	// 功能打开关闭
	bool isEnableCutwire;
	bool isEnableBackknife;
	bool isEnableModel;
	bool isEnableSetorigin;

	std::string languageFile;
	std::string windowsTitle;

	int ImageAxisMapping[IMAGE_AXIS_MAX_COUNT];
}CtrlSysConfig_Params_T_0;

class SoftwareParams
{
public:
	CtrlSysConfig_Params_T_0 params;

public:
	SoftwareParams();
	virtual ~SoftwareParams();
	
	bool saveAllParamsToIni();
	bool loadAllParamsFromIni();

protected:
	void loadSaveImageAxisMapping(bool isLoad);
	CIniSettings* m_iniSettings; // INI配置文件
};

