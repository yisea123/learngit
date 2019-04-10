/*
 * IniFileBase.cpp
 *
 * 文件ini读写的基类，支持键值对索引、区块注释
 *
 * Created on: 2017年12月25日下午2:55:07
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "IniFileBase.h"
#include <iostream>
#include <fstream>
#include <algorithm> // std::remove_if find_if

const std::string comment_prefix="# ";
const std::string newline="\n";

CIniFileBase::CIniFileBase(std::string filename)
{
	this->filename=filename;
#ifndef LINUX_USE_PTHREAD
	InitializeCriticalSection(&cs_write);
	InitializeCriticalSection(&cs_map);
#endif
}

CIniFileBase::~CIniFileBase()
{
#ifndef LINUX_USE_PTHREAD
	DeleteCriticalSection(&cs_write);
	DeleteCriticalSection(&cs_map);
#endif
}

bool CIniFileBase::is_section_exist(std::string section)
{
	return map_sections.find(section)!=map_sections.end();
}

bool CIniFileBase::add_section(std::string section, std::string comment)
{
	SECTION_T s;
	s.comment=comment;

	std::pair<std::map<std::string,SECTION_T>::iterator, bool> ret;
	
	thread_lock(&cs_map);
	ret=map_sections.insert(PAIR_SECTION_T(section,s));
	thread_unlock(&cs_map);

	return ret.second;
}

bool CIniFileBase::set_section_comment(std::string section, std::string comment)
{
	if(false==is_section_exist(section)){
		return add_section(section,comment);
	}else{
		thread_lock(&cs_map);
		map_sections[section].comment=comment;
		thread_unlock(&cs_map);
	}

	return true;
}

std::string CIniFileBase::get_section_comment(std::string section)
{
	if(true==is_section_exist(section)){
		return map_sections[section].comment;
	}

	return std::string(); // NULL string
}

bool CIniFileBase::set_value(std::string key, std::string value, std::string section)
{
	if(false==is_section_exist(section)){
		add_section(section);
	}

	thread_lock(&cs_map);
	map_sections[section].map_items[key]=value;
	thread_unlock(&cs_map);

	return true;
}

bool CIniFileBase::set_string(std::string key, std::string value, std::string section)
{
	return set_value(key,value,section);
}

std::string CIniFileBase::get_string(std::string key, std::string section)
{
	return get_value(key,section);
}

std::string CIniFileBase::get_value(std::string key, std::string section)
{
	if(false==is_section_exist(section)){
		return std::string(); // NULL string
	}

	std::map<std::string,std::string>* p_items=&map_sections[section].map_items;

	if(p_items->find(key)==p_items->end()){
		return std::string(); // NULL string
	}

	return p_items->at(key);
}

bool CIniFileBase::save()
{
	thread_lock(&cs_write);

	std::ofstream out_file (filename.c_str());
	if (!out_file.is_open())
		return false;

	std::map<std::string,SECTION_T>::iterator it_section;
	std::map<std::string,std::string>::iterator it_item;

	for(it_section=map_sections.begin();it_section!=map_sections.end();/*++it_section*/){
		if(it_section->second.comment!=std::string()){
			out_file << get_full_comment(it_section->second.comment) << std::endl;
		}

		out_file << "[" << it_section->first << "]" << std::endl;


		for(it_item=it_section->second.map_items.begin();
				it_item!=it_section->second.map_items.end();
				++it_item)
		{
			out_file << it_item->first << "=" << it_item->second << std::endl;
		}

		if(++it_section!=map_sections.end()) /*++it_section*/
			out_file << std::endl;
	}

	out_file.close();
	thread_unlock(&cs_write);
	return true;
}

bool CIniFileBase::load()
{
	std::string s;
	std::ifstream in_file (filename.c_str());
	if (!in_file.is_open())
		return false;

	thread_lock(&cs_map);

	map_sections.clear();
	std::string comment=std::string();
	std::string section;
	std::string key,value;
	bool is_found_comment=false;

	while(!std::getline(in_file, s).eof()){
		trim(s);

		if(s.empty())
			continue;

		if(starts_with(s,comment_prefix)){
			s.erase(0,comment_prefix.length());

			if(true==is_found_comment){
				comment+=(s+newline);
			}else{
				comment=(s+newline);
				is_found_comment=true;
			}

		}else if(*s.begin()=='[' && *s.rbegin()==']'){
			section=s.substr(1,s.length()-2);

			if(true==is_found_comment){ // comment is not empty
				comment=comment.substr(0,comment.length()-newline.length()); // remove last '\n'
				is_found_comment=false;
			}

			add_section(section,comment);
			comment=std::string(); // NULL string

		}else if(s.find('=')!=std::string::npos){
			key=s.substr(0,s.find('='));
			value=s.substr(s.find('=')+1);

			trim(key);
			trim(value);

			set_value(key,value,section);
		}
	}

	in_file.close();
	thread_unlock(&cs_map);
	return true;
}

inline void CIniFileBase::trim(std::string& s)
{
	// Trim example: https://gist.github.com/springmeyer/4236211
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_space));
	s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
}

bool CIniFileBase::is_not_space(int ch)
{
	if (ch == 32 || ch == 160 || ch == '\n' || ch == '\r' || ch == '\t')
		return false;
    return true;
}

std::string CIniFileBase::get_full_comment(std::string comment)
{
	// C++ replace example: https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c

	if(std::string()==comment)
		return comment;

	std::string comment_full=comment_prefix+comment;
	std::string replace_by=(newline+comment_prefix);
	int len_comment_prefix=static_cast<int>(comment_prefix.length());
	int len_newline=static_cast<int>(newline.length());
	size_t index = len_comment_prefix;

	while (true) {
		 /* Locate the substring to replace. */
		 index = comment_full.find(newline, index);
		 if (index == std::string::npos)
			 break;

		 /* Make the replacement. */
		 comment_full.replace(index, len_newline, replace_by);

		 /* Advance index forward so the next iteration doesn't pick it up as well. */
		 index += (len_newline+len_comment_prefix);
	}

	return comment_full;
}
