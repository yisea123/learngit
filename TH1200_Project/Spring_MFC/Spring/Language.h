/*
 * Language.h
 *
 * ��װIniLanguage���API�ӿں���
 *
 * Created on: 2018��4��17������10:08:14
 * Author: lixingcong
 */

#pragma once

#include "IniLanguage.h"
#include <vector>
#include <map>

#define LANGUAGE_MAP_ID(id) id,#id
#define LANGUAGE_STR(id) #id

class CLanguage
{
public:
	// ����Diaglog��ʼ��refreshLanguageUI()�����õ��ýṹ��
	typedef struct{
		int id;
		std::string id_str;
		int section_index; // section������
	}LANG_ITEM_T;

	// INI�ļ��Ķ�����
	enum{
		SECT_COMMON_TEXTS,
		SECT_DIALOG_ABOUT,
		SECT_DIALOG_AXIS_IMAGE,
		SECT_DIALOG_AXIS_RENAME,
		SECT_DIALOG_BACKZERO,
		SECT_DIALOG_CUTWIRE,
		SECT_DIALOG_DIAGNOSE,
		SECT_DIALOG_DOGDECRYPT,
		SECT_DIALOG_DOGENCRYPT,
		SECT_DIALOG_MANUFACTURE,
		SECT_DIALOG_OPENFILE,
		SECT_DIALOG_SET1600EIP,
		SECT_DIALOG_SETDESTIP,
		SECT_DIALOG_SETLANG,
		SECT_DIALOG_SETTINGIO,
		SECT_DIALOG_SETTINGIP,
		SECT_DIALOG_SETTINGS,
		SECT_DIALOG_SETTINGSOFTWARE,
		SECT_DIALOG_SPRING,
		SECT_DIALOG_STARTUP,
		SECT_DIALOG_USERS,
		SECT_ERRORS_TEACHING_1,
		SECT_ERRORS_TEACHING_2,
		SECT_LIST_AXIS_RENAME,
		SECT_LIST_DOGPASSWORD,
		SECT_LIST_MODEL,
		SECT_LIST_SETTINGIO,
		SECT_LIST_SETTINGS,
		SECT_LIST_TEACH,
		SECT_PARAM_DESCRIPTION,
		SECT_PARAM_NAMES,
		SECT_USER_NAMES,
		SECT_WARNING_MESSAGE,
		SECT_IO_NAMES_IN,
		SECT_IO_NAMES_OUT,
		SECT_MAX_INDEX
	};

	// ͨ�^getStringById()��ȡ������ID
	enum{
		LANG_TIME_YEAR,
		LANG_TIME_MONTH,
		LANG_TIME_DAY,
		LANG_TIME_HOUR,
		LANG_TIME_MINUTE,
		LANG_TIME_SECOND,
		LANG_RESULT_SUCCESS,
		LANG_RESULT_FAIL,
		LANG_RESULT_ERROR,
		LANG_INVALID_RANGE,
		LANG_INVALID_PASSWORD,
		LANG_MODBUS_BUSY,
		LANG_MODBUS_FAIL,
		LANG_MODBUS_FAIL_LOADWF,
		LANG_BUTTON_OK,
		LANG_BUTTON_CANCEL,
		LANG_BUTTON_SAVE,
		LANG_USER_MINIMAL_LEVEL,
		LANG_USER_CURRENT_LEVEL,
		LANG_SWITCH_ENABLED,
		LANG_SWITCH_DISABLED,
		LANG_SWITCH_ON,
		LANG_SWITCH_OFF,
		LANG_TITLE_HINT,
	};

public:
	CLanguage(std::string filename, bool isReadOnly=false);
	~CLanguage(void);

	CString getStringById(int id, int section_index);
	CString getString(std::string key, int section_index, const std::string& default_value);
	CString getString(std::string key, int section_index);

	// ���öԻ���Ԫ�صĺ�����ע�⴫���lang_array�����һ��Ԫ�ر���Ϊ{0,string(),0}����������Խ��
	void setDialogText(CWnd* context, LANG_ITEM_T* lang_array);

	// ���ÿɼ��˵��ĺ��� lixingcong@20180315��ע�⴫���lang_array�����һ��Ԫ�ر���Ϊ{0,string(),0}����������Խ��
	void setMenuText(CMenu* context, LANG_ITEM_T* lang_array);

	// ����popup�˵��ĺ��� lixingcong@20180316��ע�⴫���max_count������������Խ��
	void setMenuPopupText(CMenu* menu, LANG_ITEM_T* lang_array, int max_count);

	// ���ݱ����Ż�ȡ�����ı�
	CString getAlarmMsg(int alarmNumber);

	// ϵͳ���õĲ�������
	CString getParamName(int index); // index����ΪParaTable�е��±�
	CString getParamDescription(int index);
	CString getParamTitle(int title_index);

	// ��������
	CString getCommonText(int id);

	// IO����
	CString getIOName(int index, bool isInput=true);

	// �������ļ�
	bool saveNow(){return iniLang->save_now();}

protected:
	typedef struct{
		int section_index; // section������
		std::string key;
		std::string value;
	}LANG_KEY_VAL_T;

	typedef struct{
		int section_index; // section������
		std::string section;
	}LANG_SECT_ARRAY_T;

	typedef struct{
		int section_index; // section������
		int id; // ID��
		std::string id_str; // ID�ŵ��ַ���
		std::string value;
	}LANG_MAPKEY_VAL_T;

protected: // mapping init
	void _initSectionArray();
	void _initDiaglogTexts();
	void _initMapTexts();
	void _initAlarmMsg();
	void _initParamHints();
	void _initIONames();

	// ����id�ַ�����ӳ�� lixingcong@20180319��ע�⴫���lang_array�����һ��Ԫ�ر���Ϊ{0,string(),0}����������Խ��
	void mapIDs(const LANG_MAPKEY_VAL_T* lang_array);
	void mapID(int id, std::string id_str, int section_index, const std::string& default_value);

protected:
	CIniLanguage* iniLang;
	
	const std::string* _section_indexes[SECT_MAX_INDEX];

	static const LANG_KEY_VAL_T DEFAULT_TEXTS[];
	static const LANG_SECT_ARRAY_T DEFAULT_SECTIONS[];
	static const LANG_MAPKEY_VAL_T DEFAULT_MAPPING_TEXTS[];

	// ӳ��
	std::map<int,CString> map_param_names;
	std::map<int,CString> map_param_descriptions;
	std::map<int,CString> map_param_titles;

	std::map<int,int> map_alarm_num_idx_with_axis; // ������ŵı�����Ϣӳ�䵽offset

	// ��̬������ڴ�
	std::vector<CString*> menuStrings;
};

