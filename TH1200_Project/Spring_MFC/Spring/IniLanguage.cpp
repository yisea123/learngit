/*
 * IniLanguage.cpp
 *
 * INI�����ļ�
 *
 * Created on: 2018��3��2������4:24:27
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

	// ����INI�ļ��У�key-value�����ڣ���д��Ĭ��ֵ��INI�ļ�
	if(std::string() == get_value(id_str,section)){
		set_language_string(id_str,section,default_value);
	}
}

std::string CIniLanguage::get_language_string_by_id(int id, std::string section)
{
	// ����section->section_T���Ƿ��м�¼
	if(map_id_sections.find(section)==map_id_sections.end()){
		return std::string(); // ��section��¼
	}

	// ����id->id_string���Ƿ��м�¼
	ID_SECTION_T* item=&map_id_sections[section];
	if(item->find(id)==item->end()){
		return std::string(); // ��id��¼
	}

	return get_language_string(item->at(id).id_str,section,item->at(id).default_value);
}

std::string CIniLanguage::get_language_string(std::string key, std::string section, const std::string& default_value)
{
	std::string value=get_value(key,section);

	// INI�ļ����м�¼
	if(std::string() != value){
		return value;
	}

	// INI�ļ����޼�¼��д�뵽INI��
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