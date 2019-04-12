/*
 * IniLanguage.h
 *
 * INI语言文件
 *
 * Created on: 2018年3月2日下午4:24:27
 * Author: lixingcong
 */

#ifndef INILANGUAGE_H_
#define INILANGUAGE_H_

#include <IniFileBase.h>

class CIniLanguage: public CIniFileBase
{
protected:
	struct ID_STRING_TEXT_T{
		std::string id_str;
		std::string default_value;
	};

	typedef std::map<int, ID_STRING_TEXT_T> ID_SECTION_T; // 从ID到ID_string的映射

public:
	CIniLanguage(std::string filename, bool isReadOnly);
	virtual ~CIniLanguage();

	/*
	 *   func: set_language_string_id
	 *  brief: 资源ID与字符串的映射
	 *  param: id        资源ID
	 *         id_str    资源ID的字符串
	 *         section   所在ini内的段落
	 *         default_value   默认value值，若资源ID不存在于ini文件，该ID被赋值为defualt_value，否则以ini文件为准
	 * return: void
	 */
	void set_language_string_id(int id, std::string id_str, std::string section, const std::string& default_value);

	/*
	 *   func: get_language_string_by_id
	 *  brief: 获取语言文本(by id)
	 * params: id       资源ID
	 *         section  所在INI内的段落
	 * return: 语言字符串，若不存在则返回string()
	 */
	std::string get_language_string_by_id(int id, std::string section);

	// 获取语言文本，若找不到该翻译文件，default_value则被写入到map_sections，手动调用save来保存
	std::string get_language_string(std::string key, std::string section, const std::string& default_value);

	std::string get_language_string(std::string key, std::string section);

	void set_language_string(std::string key, std::string section, const std::string& value);

	// 强制立即写入到ini文件
	bool save_now();

protected: // members
	std::map<std::string, ID_SECTION_T> map_id_sections; // 这个映射集合，与基类IniFileBase的map_sections不一样
	bool isNeedSave; // 是否需要写入到磁盘，依赖于isReadOnly=false
	bool isReadOnly; // 是否只读，只读：不读取/保存ini文件。
};

#endif /* INILANGUAGE_H_ */
