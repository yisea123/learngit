/*
 * IniLanguage.cpp
 *
 * INI语言文件
 *
 * Created on: 2018年3月2日下午4:24:27
 * Author: lixingcong
 */

#include <stdafx.h>
#include <IniLanguage.h>

CIniLanguage::CIniLanguage(std::string filename, bool isReadOnly)
	:CIniFileBase(filename)
{
	isNeedSave=false;
	this->isReadOnly=isReadOnly;

	if(false == isReadOnly)
		load();
}

CIniLanguage::~CIniLanguage()
{
	if(false==isReadOnly &&  true==isNeedSave){
		save();
	}
}

void CIniLanguage::set_language_string_id(int id, std::string id_str, std::string section, const std::string& default_value)
{
	ID_STRING_TEXT_T item={id_str,default_value};
	map_id_sections[section][id]=item;

	// 若该INI文件中，key-value不存在，则写入默认值到INI文件
	if(std::string() == get_value(id_str,section)){
		set_language_string(id_str,section,default_value);
	}
}

std::string CIniLanguage::get_language_string_by_id(int id, std::string section)
{
	// 索引section->section_T中是否有记录
	if(map_id_sections.find(section)==map_id_sections.end()){
		return std::string(); // 无section记录
	}

	// 索引id->id_string中是否有记录
	ID_SECTION_T* item=&map_id_sections[section];
	if(item->find(id)==item->end()){
		return std::string(); // 无id记录
	}

	return get_language_string(item->at(id).id_str,section,item->at(id).default_value);
}

std::string CIniLanguage::get_language_string(std::string key, std::string section, const std::string& default_value)
{
	std::string value=get_value(key,section);

	// INI文件中有记录
	if(std::string() != value){
		return value;
	}

	// INI文件中无记录，写入到INI中
	set_language_string(key,section,default_value);
	return default_value;
}

std::string CIniLanguage::get_language_string(std::string key, std::string section)
{
	return get_value(key,section);
}

void CIniLanguage::set_language_string(std::string key, std::string section, const std::string& value)
{
	isNeedSave=true;
	set_value(key,value,section);
}

bool CIniLanguage::save_now()
{
	bool isOk=save();
	
	if(true == isOk)
		isNeedSave=false;

	return isOk;
}