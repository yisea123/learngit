/*
 * IniSettings.cpp
 *
 * INI����
 *
 * Created on: 2017��12��25������2:55:07
 * Author: lixingcong
 */

#include "StdAfx.h"
#include <IniSettings.h>

const std::string strTrue=std::string("True");
const std::string strFalse=std::string("False");

bool CIniSettings::set_boolean(std::string key, bool value, std::string section)
{
	return set_value(key,value?strTrue:strFalse,section);
}

bool CIniSettings::get_boolean(std::string key, std::string section, bool default_value)
{
	std::string str_value=get_value(key,section);

	if(str_value != std::string()){
		return (strTrue == str_value);
	}

	return default_value;
}
