/*
 * IniLanguage.h
 *
 * INI�����ļ�
 *
 * Created on: 2018��3��2������4:24:27
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

	typedef std::map<int, ID_STRING_TEXT_T> ID_SECTION_T; // ��ID��ID_string��ӳ��

public:
	CIniLanguage(std::string filename, bool isReadOnly);
	virtual ~CIniLanguage();

	/*
	 *   func: set_language_string_id
	 *  brief: ��ԴID���ַ�����ӳ��
	 *  param: id        ��ԴID
	 *         id_str    ��ԴID���ַ���
	 *         section   ����ini�ڵĶ���
	 *         default_value   Ĭ��valueֵ������ԴID��������ini�ļ�����ID����ֵΪdefualt_value��������ini�ļ�Ϊ׼
	 * return: void
	 */
	void set_language_string_id(int id, std::string id_str, std::string section, const std::string& default_value);

	/*
	 *   func: get_language_string_by_id
	 *  brief: ��ȡ�����ı�(by id)
	 * params: id       ��ԴID
	 *         section  ����INI�ڵĶ���
	 * return: �����ַ��������������򷵻�string()
	 */
	std::string get_language_string_by_id(int id, std::string section);

	// ��ȡ�����ı������Ҳ����÷����ļ���default_value��д�뵽map_sections���ֶ�����save������
	std::string get_language_string(std::string key, std::string section, const std::string& default_value);

	std::string get_language_string(std::string key, std::string section);

	void set_language_string(std::string key, std::string section, const std::string& value);

	// ǿ������д�뵽ini�ļ�
	bool save_now();

protected: // members
	std::map<std::string, ID_SECTION_T> map_id_sections; // ���ӳ�伯�ϣ������IniFileBase��map_sections��һ��
	bool isNeedSave; // �Ƿ���Ҫд�뵽���̣�������isReadOnly=false
	bool isReadOnly; // �Ƿ�ֻ����ֻ��������ȡ/����ini�ļ���
};

#endif /* INILANGUAGE_H_ */
