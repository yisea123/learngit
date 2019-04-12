#pragma once

#include "Definations.h"
#include "TestIo_public.h"
#include <string>
#include "IniSettings.h"
#include "StaticImageAxis.h"

typedef struct{
	// modbusĿ��ip
	char destIPAddr[MAX_LEN_IO_NAME];
	unsigned int modbus_timeout;
	unsigned int modbus_retry_times;

	bool isEnableLogger; // �Ƿ�������־
	bool isNeedPasswordExiting; // �˳�ǰ�Ƿ���Ҫ����
	bool isAlwaysOnTop; // ��������
	bool isBootWithMaximized; // �������

	// ���ܴ򿪹ر�
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
	CIniSettings* m_iniSettings; // INI�����ļ�
};

