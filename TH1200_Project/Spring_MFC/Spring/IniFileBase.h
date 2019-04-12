/*
 * IniFileBase.h
 *
 * 文件ini读写的基类，支持键值对索引、区块注释
 *
 * Created on: 2017年12月25日下午2:55:07
 * Author: lixingcong
 */

#ifndef CINIFILEBASE_H_
#define CINIFILEBASE_H_

#include <string>
#include <map>
#include <vector>

#ifdef LINUX_USE_PTHREAD
#include <pthread.h>
#else
#include <Windows.h>
#endif

class CIniFileBase
{
protected:
	struct SECTION_T{
		std::string comment;
		std::map<std::string, std::string> map_items;
	};

	typedef std::pair<std::string,SECTION_T> PAIR_SECTION_T;
	typedef std::pair<std::string,std::string> PAIR_KEYVALUE_T;

public:
	CIniFileBase(std::string filename);
	~CIniFileBase(void);

	bool save();
	bool load();

	bool is_section_exist(std::string section);
	bool add_section(std::string section, std::string comment=std::string());
	bool set_section_comment(std::string section, std::string comment);
	std::string get_section_comment(std::string section);

	bool set_string(std::string key, std::string value, std::string section);
	std::string get_string(std::string key, std::string section);

protected: // members
#ifdef LINUX_USE_PTHREAD
	typedef thread_mutex_t pthread_mutex_t;
	bool thread_lock(thread_mutex_t* mutex, bool isBlocking=true)
	{
		int ret=-1;
		if(isBlocking)
			ret=pthread_mutex_lock(mutex);
		else
			ret=pthread_mutex_trylock(mutex);
		return 0==ret;
	}
	bool thread_unlock(thread_mutex_t* mutex)
	{
		return 0==pthread_mutex_unlock(mutex);
	}
#else
	typedef CRITICAL_SECTION thread_mutex_t;
	bool thread_lock(LPCRITICAL_SECTION cs, bool isBlocking=true)
	{
		if(isBlocking)
			EnterCriticalSection(cs);
		else
			TryEnterCriticalSection(cs);
		return true;
	}
	bool thread_unlock(LPCRITICAL_SECTION cs)
	{
		LeaveCriticalSection(cs);
		return true;
	}
#endif
	std::map<std::string, SECTION_T> map_sections;
	std::string filename;
	thread_mutex_t cs_write;
	thread_mutex_t cs_map;

protected: // member functions
	bool set_value(std::string key, std::string value, std::string section);
	std::string get_value(std::string key, std::string section);

	// Trim empty space
	static inline void trim(std::string& s);
	static bool is_not_space(int ch);

	std::string get_full_comment(std::string comment);

protected: // templates
	template<class T>
	bool starts_with(const T& input, const T& match)
	{
		return input.size() >= match.size()	&& equal(match.begin(), match.end(), input.begin());
	}

};

#endif
