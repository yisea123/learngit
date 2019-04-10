/*
 * IniSettings.h
 *
 * INI配置
 *
 * Created on: 2017年12月25日下午2:55:07
 * Author: lixingcong
 */

#ifndef INISETTINGS_H_
#define INISETTINGS_H_

#include "IniFileBase.h"
#include <sstream>

class CIniSettings : public CIniFileBase
{
public:
	CIniSettings(std::string filename) : CIniFileBase(filename)
	{
	};

	virtual ~CIniSettings()
	{
	};

	bool set_boolean(std::string key, bool value, std::string section);
	bool get_boolean(std::string key, std::string section, bool default_value);

protected: // templates
	template <typename T>
	std::string get_string_from_value(T value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	template <typename T>
	T get_value_from_string(const std::string& str, T default_value)
	{
		std::stringstream ss(str);
		T result;
		return ss >> result ? result: default_value;
	}

public:
	template <typename T>
	bool set_number(std::string key, T value, std::string section)
	{
		return set_value(key,get_string_from_value(value),section);
	}

	template <typename T>
	T get_number(std::string key, std::string section ,T default_value)
	{
		std::string str_value=get_value(key,section);
		return (T)get_value_from_string(str_value,default_value);
	}

};

#endif /* INISETTINGS_H_ */
