/*
 * Language.h
 *
 * 封装IniLanguage后的API接口函数
 *
 * Created on: 2018年4月17日上午10:08:14
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
	// 其他Diaglog初始化refreshLanguageUI()函数用到该结构体
	typedef struct{
		int id;
		std::string id_str;
		int section_index; // section索引号
	}LANG_ITEM_T;

	// INI文件的段落名
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

	// 通過getStringById()获取的文字ID
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

	// 设置对话框元素的函数，注意传入的lang_array的最后一个元素必须为{0,string(),0}，否则数组越界
	void setDialogText(CWnd* context, LANG_ITEM_T* lang_array);

	// 设置可见菜单的函数 lixingcong@20180315，注意传入的lang_array的最后一个元素必须为{0,string(),0}，否则数组越界
	void setMenuText(CMenu* context, LANG_ITEM_T* lang_array);

	// 设置popup菜单的函数 lixingcong@20180316，注意传入的max_count用于限制数组越界
	void setMenuPopupText(CMenu* menu, LANG_ITEM_T* lang_array, int max_count);

	// 根据报警号获取报警文本
	CString getAlarmMsg(int alarmNumber);

	// 系统设置的参数文字
	CString getParamName(int index); // index参数为ParaTable中的下标
	CString getParamDescription(int index);
	CString getParamTitle(int title_index);

	// 公共文字
	CString getCommonText(int id);

	// IO文字
	CString getIOName(int index, bool isInput=true);

	// 立马保存文件
	bool saveNow(){return iniLang->save_now();}

protected:
	typedef struct{
		int section_index; // section索引号
		std::string key;
		std::string value;
	}LANG_KEY_VAL_T;

	typedef struct{
		int section_index; // section索引号
		std::string section;
	}LANG_SECT_ARRAY_T;

	typedef struct{
		int section_index; // section索引号
		int id; // ID号
		std::string id_str; // ID号的字符串
		std::string value;
	}LANG_MAPKEY_VAL_T;

protected: // mapping init
	void _initSectionArray();
	void _initDiaglogTexts();
	void _initMapTexts();
	void _initAlarmMsg();
	void _initParamHints();
	void _initIONames();

	// 设置id字符串的映射 lixingcong@20180319，注意传入的lang_array的最后一个元素必须为{0,string(),0}，否则数组越界
	void mapIDs(const LANG_MAPKEY_VAL_T* lang_array);
	void mapID(int id, std::string id_str, int section_index, const std::string& default_value);

protected:
	CIniLanguage* iniLang;
	
	const std::string* _section_indexes[SECT_MAX_INDEX];

	static const LANG_KEY_VAL_T DEFAULT_TEXTS[];
	static const LANG_SECT_ARRAY_T DEFAULT_SECTIONS[];
	static const LANG_MAPKEY_VAL_T DEFAULT_MAPPING_TEXTS[];

	// 映射
	std::map<int,CString> map_param_names;
	std::map<int,CString> map_param_descriptions;
	std::map<int,CString> map_param_titles;

	std::map<int,int> map_alarm_num_idx_with_axis; // 含有轴号的报警信息映射到offset

	// 动态申请的内存
	std::vector<CString*> menuStrings;
};

