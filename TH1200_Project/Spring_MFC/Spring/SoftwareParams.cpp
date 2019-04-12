/*
 * SoftwareParams.cpp
 *
 * 软件配置
 *
 * Created on: 2017年12月13日下午8:58:03
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "SoftwareParams.h"
#include "Definations.h"
#include "Utils.h"
#include "Data.h"
#include <string>
#include <sstream>

#define INI_SECTION_DESTIP "MODBUS_DEST"
#define INI_SECTION_SOFTWARE "SOFTWARE_CONFIG"
#define INI_SECTION_AXIS_IMAGE_MAP "MAPPING_AXIS_IMAGE"

// 参数默认值
#define MODBUS_DEST_HOST "192.168.9.120"
#define MODBUS_TIMEOUT 200
#define MODBUS_RETRY 3

//char axisName[AXIS_NUM][24]={"送线","转芯","转线","X1","X2","X3","X4","X5","X6","X7","X8","J1","J2","A","B","C"};//中文

SoftwareParams::SoftwareParams():
	m_iniSettings(new CIniSettings(getIniFileName("","Settings.ini")))
{
}

SoftwareParams::~SoftwareParams()
{
	delete_pointer(m_iniSettings);
}

bool SoftwareParams::saveAllParamsToIni()
{
	bool isOk=true;

	// Modbus默认参数
	m_iniSettings->add_section(INI_SECTION_DESTIP);
	m_iniSettings->set_string("address",params.destIPAddr,INI_SECTION_DESTIP);
	m_iniSettings->set_number("timeout",params.modbus_timeout,INI_SECTION_DESTIP);
	m_iniSettings->set_number("retry",params.modbus_retry_times,INI_SECTION_DESTIP);

	// 软件参数
	m_iniSettings->add_section(INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isEnableLogger", params.isEnableLogger, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isNeedPasswordExiting", params.isNeedPasswordExiting, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isAlwaysOnTop", params.isAlwaysOnTop, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isBootWithMaximized", params.isBootWithMaximized, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isEnableCutwire", params.isEnableCutwire, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isEnableBackknife", params.isEnableBackknife, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isEnableModel", params.isEnableModel, INI_SECTION_SOFTWARE);
	m_iniSettings->set_boolean("isEnableSetorigin", params.isEnableSetorigin, INI_SECTION_SOFTWARE);

	// language
	m_iniSettings->set_string("languageFile", params.languageFile, INI_SECTION_SOFTWARE);

	// 公司名
	m_iniSettings->set_string("windowsTitle", params.windowsTitle, INI_SECTION_SOFTWARE);

	// 动图的映射
	loadSaveImageAxisMapping(false);

	m_iniSettings->save();
	return isOk;
}

bool SoftwareParams::loadAllParamsFromIni()
{
	bool isOk=true;
	std::string got_str;

	m_iniSettings->load();

	// IP
	got_str=m_iniSettings->get_string("address",INI_SECTION_DESTIP);
	if(std::string()!=got_str){
		sprintf_s(params.destIPAddr,MAX_LEN_IO_NAME,got_str.c_str());
	}else{
		sprintf_s(params.destIPAddr,MAX_LEN_IO_NAME,MODBUS_DEST_HOST);
	}

	// timeout & retry
	params.modbus_timeout=m_iniSettings->get_number("timeout",INI_SECTION_DESTIP,(unsigned int)MODBUS_TIMEOUT);
	params.modbus_retry_times=m_iniSettings->get_number("retry",INI_SECTION_DESTIP,(unsigned int)MODBUS_RETRY);

	// software configurations
	params.isEnableLogger=m_iniSettings->get_boolean("isEnableLogger",INI_SECTION_SOFTWARE,false);
	params.isNeedPasswordExiting=m_iniSettings->get_boolean("isNeedPasswordExiting",INI_SECTION_SOFTWARE,true);
	params.isAlwaysOnTop=m_iniSettings->get_boolean("isAlwaysOnTop",INI_SECTION_SOFTWARE,false);
	params.isBootWithMaximized=m_iniSettings->get_boolean("isBootWithMaximized",INI_SECTION_SOFTWARE,true);
	params.isEnableCutwire=m_iniSettings->get_boolean("isEnableCutwire", INI_SECTION_SOFTWARE, true);
	params.isEnableBackknife=m_iniSettings->get_boolean("isEnableBackknife", INI_SECTION_SOFTWARE, true);
	params.isEnableModel=m_iniSettings->get_boolean("isEnableModel", INI_SECTION_SOFTWARE, true);
	params.isEnableSetorigin=m_iniSettings->get_boolean("isEnableSetorigin", INI_SECTION_SOFTWARE, false);

	// IO
	params.languageFile=m_iniSettings->get_string("languageFile",INI_SECTION_SOFTWARE);
	if(std::string() == params.languageFile)
		params.languageFile="default";

	// 动图的映射
	loadSaveImageAxisMapping(true);

	// 公司名
	params.windowsTitle=m_iniSettings->get_string("windowsTitle",INI_SECTION_SOFTWARE);
	if(std::string() == params.windowsTitle)
		params.windowsTitle="总线控制系统";

	return isOk;
}

void SoftwareParams::loadSaveImageAxisMapping(bool isLoad)
{
	std::stringstream ss_io_param;
	char formattedStr[10];

	for(int i=0;i<IMAGE_AXIS_MAX_COUNT;++i){
		ss_io_param.str(std::string()); // flush all data
		sprintf_s(formattedStr,10,"%03u",i+1);
		ss_io_param << "angle_" << formattedStr;

		if(true == isLoad){
			// 加载
			params.ImageAxisMapping[i]=m_iniSettings->get_number(ss_io_param.str(),INI_SECTION_AXIS_IMAGE_MAP,(int)0);
		}else{
			// 写入
			m_iniSettings->set_number(ss_io_param.str(),params.ImageAxisMapping[i],INI_SECTION_AXIS_IMAGE_MAP);
		}
	}
}