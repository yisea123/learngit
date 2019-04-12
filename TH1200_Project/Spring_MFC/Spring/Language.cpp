/*
 * Language.cpp
 *
 * 封装IniLanguage后的API接口函数
 *
 * Created on: 2018年4月17日上午10:08:14
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "Language.h"
#include "Utils.h"
#include "SysText.h"
#include "SysAlarm.h"
#include <sstream>
#include "Data.h"

const CLanguage::LANG_SECT_ARRAY_T CLanguage::DEFAULT_SECTIONS[]={
	{SECT_COMMON_TEXTS,"COMMON_TEXTS"},
	{SECT_DIALOG_ABOUT,"DIALOG_ABOUT"},
	{SECT_DIALOG_AXIS_IMAGE,"DIALOG_AXIS_IMAGE"},
	{SECT_DIALOG_AXIS_RENAME,"DIALOG_AXIS_RENAME"},
	{SECT_DIALOG_BACKZERO,"DIALOG_BACKZERO"},
	{SECT_DIALOG_CUTWIRE,"DIALOG_CUTWIRE"},
	{SECT_DIALOG_DIAGNOSE,"DIALOG_DIAGNOSE"},
	{SECT_DIALOG_DOGDECRYPT,"DIALOG_DOGDECRYPT"},
	{SECT_DIALOG_DOGENCRYPT,"DIALOG_DOGENCRYPT"},
	{SECT_DIALOG_MANUFACTURE,"DIALOG_MANUFACTURE"},
	{SECT_DIALOG_OPENFILE,"DIALOG_OPENFILE"},
	{SECT_DIALOG_SET1600EIP,"DIALOG_SET1600EIP"},
	{SECT_DIALOG_SETDESTIP,"DIALOG_SETDESTIP"},
	{SECT_DIALOG_SETLANG,"DIALOG_SETLANG"},
	{SECT_DIALOG_SETTINGIO,"DIALOG_SETTINGIO"},
	{SECT_DIALOG_SETTINGIP,"DIALOG_SETTINGIP"},
	{SECT_DIALOG_SETTINGS,"DIALOG_SETTINGS"},
	{SECT_DIALOG_SETTINGSOFTWARE,"DIALOG_SETTINGSOFTWARE"},
	{SECT_DIALOG_SPRING,"DIALOG_SPRING"},
	{SECT_DIALOG_STARTUP,"DIALOG_STARTUP"},
	{SECT_DIALOG_USERS,"DIALOG_USERS"},
	{SECT_ERRORS_TEACHING_1,"ERRORS_TEACHING_1"},
	{SECT_ERRORS_TEACHING_2,"ERRORS_TEACHING_2"},
	{SECT_LIST_AXIS_RENAME,"LIST_AXIS_RENAME"},
	{SECT_LIST_DOGPASSWORD,"LIST_DOGPASSWORD"},
	{SECT_LIST_MODEL,"LIST_MODEL"},
	{SECT_LIST_SETTINGIO,"LIST_SETTINGIO"},
	{SECT_LIST_SETTINGS,"LIST_SETTINGS"},
	{SECT_LIST_TEACH,"LIST_TEACH"},
	{SECT_PARAM_DESCRIPTION,"PARAM_DESCRIPTION"},
	{SECT_PARAM_NAMES,"PARAM_NAMES"},
	{SECT_USER_NAMES,"USER_NAMES"},
	{SECT_WARNING_MESSAGE,"WARNING_MESSAGE"},
	{SECT_IO_NAMES_IN, "IO_NAMES_IN"},
	{SECT_IO_NAMES_OUT, "IO_NAMES_OUT"},
	{-1} // end
};

const CLanguage::LANG_KEY_VAL_T CLanguage::DEFAULT_TEXTS[]={
	// 主对话框按钮元素
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_BACKZERO), "归零(F2)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_CHECKIO), "诊断(F10)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_CUT), "剪线(F9)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_ADD_LINE), "添加行(Z)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_ALL_LINES), "全删除(V)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_LINE), "删除行(C)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_INSERT_LINE), "插入行(X)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_REDO), "恢复(N)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_UNDO), "撤销(B)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_HAND), "手轮(F7)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_KNIFE), "退刀(F8)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_PROBE), "设定"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_PRODUCTION), "设定"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_RESETSONGXIAN), "送线清零"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_SPEEDRATE), "倍率"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_OPENFILE), "打开(F1)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_PROBE_FAIL_COUNTER_CLEAR2), "清零"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_PRODUCT_COUNT_CLEAR), "清零"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_RESET_ALARM), "清错(F11)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_RUN), "启动(F4)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MODELS), "模型"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_SET_ORIGIN), "设原点"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_SINGLE), "单条(F3)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_STOP), "急停(F5)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_TEACH_FROM_1600E), "教导(M)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_TEST), "测试(F6)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_DANBU), "单步(O)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_WORKSPEED_RATE), "速度(F12)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_NOW_PROG), "状态"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_PC_MOVE), "轴动"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_STATISTIC), "统计"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_TEST), "测试加工"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_CUR_AXISNAME), "选轴"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_CUR_HANDRATE), "倍率"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_DESTCOUNT), "产量设定"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_FILENAME), "程序名称"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_POSITION), "当前坐标"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_PROBE), "探针设定"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_PROBEFAIL), "探针失败"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_PRODUCTION), "实际生产"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_REMAIN_TIME), "剩余时间"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_RUNSPEED), "加工速度"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_RUNSPEED_UNIT), "个/分钟"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_SPEEDRATE), "速度百分比"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_WORKSTATUS), "运行状态"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_CYLINDER_OUT), "气缸"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_PROBE_IN), "探针"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_STOP_IN), "急停"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_XIANJIA_IN), "线架"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_O_1), "液剪开(Q)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_O_2), "液剪关(W)"},

	// 主界面的菜单
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32776), "轴重命名/开关"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32777), "系统参数"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32778), "软件设置"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32782), "查看今天日志"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32783), "打开日志文件夹"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32785), "用户手册"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32786), "软件版本"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32860), "IP地址设置"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32864), "联系设备厂商"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32865), "语言"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32866), "轴动图设置"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32862), "解密"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32863), "加密"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_1","参数设置"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_2","日志"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_3","售后服务"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_4","软件加密狗"},

	// 主界面的其他提示信息
	{SECT_DIALOG_SPRING, "LOADING_WORKFILE_HINT", "读取加工文件中，请稍候"},
	{SECT_DIALOG_SPRING, "AXIS_SELECTED_UNSPECIFIED", "未选中"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_STOP", "停止"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_RUN", "正在加工"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_PAUSE", "暂停"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_SINGLE", "单条"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_ZERO", "回零"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_HANDMOVE", "手摇轴动"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_PCMOVE", "PC轴动"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_USBLINK", "USB连接中"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_CUTWIRE", "剪线中"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_TUIDAO", "退刀中"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_FTPLINK", "FTP开启"},
	{SECT_DIALOG_SPRING, "SET_PRODUCTION_DLG_TITLE", "输入数字，范围0-9999999"},
	{SECT_DIALOG_SPRING, "SET_PROBE_DLG_TITLE", "输入数字，范围0-999"},
	{SECT_DIALOG_SPRING, "PROD_CLEAR_CONFIRM", "是否产量清零？"},
	{SECT_DIALOG_SPRING, "HANDBOX_RATIO_LOW", "低"},
	{SECT_DIALOG_SPRING, "HANDBOX_RATIO_MID", "中"},
	{SECT_DIALOG_SPRING, "HANDBOX_RATIO_HIGH", "高"},
	{SECT_DIALOG_SPRING, "SET_ORIGIN_INVALID_AXIS", "请选中正确的轴号"},
	{SECT_DIALOG_SPRING, "SET_ORIGIN_CURRENT_AXIS", "当前选中轴："},
	{SECT_DIALOG_SPRING, "SET_ORIGIN_CONFIRM", "确实要设置该轴原点？"},
	{SECT_DIALOG_SPRING, "TEACH_ERR_HINT", "教导无效，可能是某个轴是多圈模式，且为相对模式"},
	{SECT_DIALOG_SPRING, "INVALID_MACHINE_BACKKNIFE", "退刀功能仅适用于12轴以上弹簧机"},
	{SECT_DIALOG_SPRING, "SET_BACKKNIFE_DLG_TITLE", "退刀速度百分比，范围0-500"},
	{SECT_DIALOG_SPRING, "SET_WORKSPEED_DLG_TITLE", "总加工速度百分比，范围0-150"},
	{SECT_DIALOG_SPRING, "DOG_VALIDATION_HINT_SERIAL", "机器序列号:"},
	{SECT_DIALOG_SPRING, "DOG_VALIDATION_HINT_SERIAL2", "厂商序列号:"},
	{SECT_DIALOG_SPRING, "DOG_VALIDATION_HINT_INPUT_PWD", "请输入加密狗管理密码"},
	{SECT_DIALOG_SPRING, "DEST_COUNT_REACH_HINT", "加工完成，已达到目标产量："},
	{SECT_DIALOG_SPRING, "INVALID_MACHINE_MODEL", "模型功能仅适用于6轴压簧机"},
	{SECT_DIALOG_SPRING, "INVALID_MACHINE_CUTWIRE", "剪线功能仅适用于12轴以上弹簧机"},
	{SECT_DIALOG_SPRING, "ON_CANCEL_PWD_TITLE", "请输入四位退出密码"},
	{SECT_DIALOG_SPRING, "LOG_WAS_DISABLE", "日志已关闭，请在软件设置中启用它"},

	// 打开对话框
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_STATIC_OPENFILE_TOTAL_TEXT), "文件总数"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_BUTTON_OPENFILE_COPY), "复制"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_BUTTON_OPENFILE_NEWFILE), "新建"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_BUTTON_OPENFILE_DELETE), "删除"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDOK), "打开"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDCANCEL), "取消"},

	// 归零对话框
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_SINGLE), "单轴归零"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_IS_MACHINE), "机械零点"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_IS_PROG), "程序零点"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_MULTIPLE), "全部归零"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDOK), "所有轴归零"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_FORCE_MACHINE), "强制机械归零"},

	// 系统版本对话框
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_REGISTER), "产品注册状态"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_PRODUCTID), "控制器产品ID"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_MOTION_LIB), "控制器运动库"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_ZYNQ), "控制器LIB版本"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_FPGA1), "控制器FPGA1版本"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_FPGA2), "控制器FPGA2版本"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_MFC), "上位机软件版本"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_CONTROLLER), "控制器软件版本"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_BUILDDATE_MFC), "上位机编译日期"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_BUILDDATE_CONTROLLER), "控制器编译日期"},

	// 剪线对话框
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_MAIN), "主刀"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_MAIN_AXIS), "轴选择"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_MAIN_ANGLE), "角度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_MAIN_SPEED), "速度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_OPT), "辅助刀"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_OPT_AXIS), "轴选择"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_OPT_ANGLE), "角度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_OPT_SPEED), "速度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_SCISSOR), "液压剪"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_CHECK_CUT_WIRE_USE_SISSOR), "使用液压剪"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_DELAY), "延时(秒)"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_OTHERS), "其他"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_ZHUANXIN_ANGLE), "转芯角度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_ZHUANXIN_SPEED), "转芯速度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_SONGXIAN_POS), "送线位置"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_SONGXIAN_SPEED), "送线速度"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDOK), "执行剪线"},

	// 加密狗：解密对话框
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_TIME), "控制器时间"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_REMAIN), "剩余天数"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_RANDOM), "随机索引"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_UNLOCK), "解锁码"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_ISUNLOCK), "解锁状态"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_SERIAL2), "厂商序列号"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDOK), "确定"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_SERIAL), "机器序列号"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_CURRENT), "当前期数"},

	// 加密狗：加密对话框
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_STATUS), "加密狗状态设置"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_REMAIN), "剩余天数(最长四位)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_RANDOM), "随机索引"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_SERIAL), "机器序列号(最长六位)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_SERIAL2), "厂商序列号(最长六位)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_PWD), "管理密码(最长十位)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_TIME), "控制器时间"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_CHECK_DOGENCRYPTDLG_SETTIME), "改控制器时间"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_BUTTON_DOGENCRYPTDLG_INIT), "初始化"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_BUTTON_DOGENCRYPTDLG_SAVE), "应用"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDOK), "确定"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_CURRENT), "当前期数"},

	// 设置1600的IP对话框
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_IP), "控制器IP"},
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_MASK), "控制器子网掩码"},
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_GW), "控制器默认网关"},
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_MAC), "控制器MAC地址"},

	// 设置上位机目标IP对话框
	{SECT_DIALOG_SETDESTIP, LANGUAGE_STR(IDC_STATIC_SETIP_DEST_HINT_IP), "地址"},
	{SECT_DIALOG_SETDESTIP, LANGUAGE_STR(IDC_STATIC_SETIP_DEST_HINT_TIMEOUT), "超时(ms)"},
	{SECT_DIALOG_SETDESTIP, LANGUAGE_STR(IDC_STATIC_SETIP_DEST_HINT_RETRY), "重发次数"},

	// 系统设置对话框
	{SECT_DIALOG_SETTINGS, LANGUAGE_STR(IDC_STATIC_SETTINGSDLG_RANGE), "输入范围"},
	{SECT_DIALOG_SETTINGS, "CHANGE_USER_HINT", "要修改，请切换用户："},
	
	// 软件配置
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_STATIC_SETTING_SW_HINT), "本页配置上位机的行为"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_LOG), "开启日志功能"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_EXITHINT), "退出前需要密码"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_TOPMOST), "窗口总在最前"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_MAXIMIZED), "窗口启动时最大化"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_STATIC_WINDOWSTITLE_HINT), "窗口标题"},

	// 启动对话框
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_BUTTON_REFRESH_SERIAL), "刷新"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_BUTTON_STARTUP_CONTINUE), "继续"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_BUTTON_STARTUP_STOP), "中断连接"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_RADIO_CONN_TYPE_IP), "IP"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_RADIO_CONN_TYPE_SERIAL), "串口"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_1), "超时"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_2), "重试"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_OPT1), "通讯方式"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_OPT2), "高级选项"},

	// 权限管理对话框
	{SECT_DIALOG_USERS, LANGUAGE_STR(IDC_STATIC_USERS_HINT_CUR), "当前用户"},
	{SECT_DIALOG_USERS, LANGUAGE_STR(IDC_STATIC_USERS_HINT_SWITCH), "切换用户"},
	{SECT_DIALOG_USERS, LANGUAGE_STR(IDC_STATIC_USERS_HINT_PWD), "修改密码"},

	// 轴命名对话框
	{SECT_DIALOG_AXIS_RENAME,"TITLE","轴命名"},

	// 归零对话框
	{SECT_DIALOG_BACKZERO,"TITLE","归零"},

	// 关于本软件
	{SECT_DIALOG_ABOUT,"TITLE","关于本软件"},

	// 剪线对话框
	{SECT_DIALOG_CUTWIRE, "SUFFIX_DISABLED", "已停用"},
	{SECT_DIALOG_CUTWIRE, "ERR_NO_SAME_AXIS", "辅助刀与主刀不能选择相同的轴"},
	{SECT_DIALOG_CUTWIRE, "TITLE", "剪线"},

	// 诊断对话框
	{SECT_DIALOG_DIAGNOSE, "TITLE", "IO诊断：勾选代表高电平"},
	{SECT_DIALOG_DIAGNOSE, "TAB_TITLE_INPUT", "输入"},
	{SECT_DIALOG_DIAGNOSE, "TAB_TITLE_OUTPUT", "输出"},
	{SECT_DIALOG_DIAGNOSE, "TAB_TITLE_OTHERS", "系统操作"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_UPGRADE", "更新固件"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_REBOOT", "重启控制器"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADE", "选择A9Rom.bin升级"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_REBOOT", "重启后请关闭软件"},
	{SECT_DIALOG_DIAGNOSE, "CONFIRM_UPGRADE", "确实要升级？"},
	{SECT_DIALOG_DIAGNOSE, "CONFIRM_REBOOT", "确实要重启？"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADING", "升级中，请勿断电"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADED", "升级完毕，请点击重启"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADE_FAIL", "升级失败"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADE_FAIL2", "请在停机状态下重试"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_USB_TO_DISCONNECT", "关闭USB"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_USB_STATUS_ON", "USB已开启"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_USB_TO_CONNECT", "连接USB"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_USB_STATUS_OFF", "USB已关闭"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_FTP_TO_DISCONNECT", "关闭FTP"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_FTP_STATUS_ON", "FTP已开启"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_FTP_TO_CONNECT", "连接FTP"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_FTP_STATUS_OFF", "FTP已关闭"},

	// 加密狗解密对话框
	{SECT_DIALOG_DOGDECRYPT, "UNLOCK_STATUS_FREE", "已解锁"},
	{SECT_DIALOG_DOGDECRYPT, "UNLOCK_STATUS_LOCKED", "已锁定"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_NO_RESTRICTED", "无限制"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_UNLOCK_DONE", "谢谢，已解锁所有使用时长"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_UNLOCKED_COUNT", "已解锁期数"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_UNLOCKED_DAYS", "可使用天数"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_WRITE_ERR", "写入加密狗失败"},
	{SECT_DIALOG_DOGDECRYPT, "TITLE", "软件加密狗"},

	// 加密狗加密
	{SECT_DIALOG_DOGENCRYPT, "HINT_NO_RESTRICTED", "禁用加密狗"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_ENABLE_DOG1", "启用加密狗1(多期)"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_ENABLE_DOG2", "启用加密狗2(单期)"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_NO_EMPTY_PWD", "密码不能为空"},
	{SECT_DIALOG_DOGENCRYPT, "CONFIRM_RESET", "确定重置加密狗？"},
	{SECT_DIALOG_DOGENCRYPT, "RESET_FAIL", "初始化失败"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_WRITE_ERR", "写入加密狗失败"},
	{SECT_DIALOG_DOGENCRYPT, "TITLE", "加密狗管理"},
	
	// 厂商联系方式图片
	{SECT_DIALOG_MANUFACTURE, "TITLE", "设备厂商"},
	{SECT_DIALOG_MANUFACTURE, "FILE_NOT_FOUND", "找不到文件"},

	// 打开文件对话框
	{SECT_DIALOG_OPENFILE, "HEADER_FILENAME", "文件名"},
	{SECT_DIALOG_OPENFILE, "HEADER_SIZE", "大小"},
	{SECT_DIALOG_OPENFILE, "HEADER_DATE", "修改日期"},
	{SECT_DIALOG_OPENFILE, "HEADER_TIME", "修改时间"},
	{SECT_DIALOG_OPENFILE, "READ_FILELIST_ERR", "modbus读取文件列表失败"},
	{SECT_DIALOG_OPENFILE, "OPEN_NOT_THJ_ERR", "无法打开文件："},
	{SECT_DIALOG_OPENFILE, "HINT_SRC_FILENAME", "源文件为"},
	{SECT_DIALOG_OPENFILE, "HINT_DUPLICATE_SRC_DST", "目标文件名与源文件相同"},
	{SECT_DIALOG_OPENFILE, "CONFIRM_DELETE", "确实需要删除吗？"},
	{SECT_DIALOG_OPENFILE, "HINT_INPUT_FILENAME", "输入新文件名"},
	{SECT_DIALOG_OPENFILE, "HINT_INVALID_LENGTH", "文件名长度不合法"},
	{SECT_DIALOG_OPENFILE, "CONFIRM_OVERWRITE", "已存在，确实需要覆盖吗？"},
	{SECT_DIALOG_OPENFILE, "TITLE", "文件"},

	// 设置IP
	{SECT_DIALOG_SET1600EIP, "ERR_INVALID_MAC", "MAC地址格式不合法"},
	{SECT_DIALOG_SETTINGIP, "TITLE", "IP设置"},
	{SECT_DIALOG_SETTINGIP, "TAB_DEST", "Modbus连接设置"},
	{SECT_DIALOG_SETTINGIP, "TAB_CONTROLLER_IP", "控制器IP"},

	// 轴动图映射
	{SECT_DIALOG_AXIS_IMAGE, "TITLE", "设定轴动图的实际轴"},

	// 设定IO
	{SECT_DIALOG_SETTINGIO, "TITLE", "IO设置"},
	{SECT_DIALOG_SETTINGIO, "TAB_INPUT", "输入"},
	{SECT_DIALOG_SETTINGIO, "TAB_OUTPUT", "输出"},

	// 设置语言对话框
	{SECT_DIALOG_SETLANG, "HINT", "更改语言：重启生效"},
	{SECT_DIALOG_SETLANG, "TITLE", "设置语言"},
	{SECT_DIALOG_SETLANG, LANGUAGE_STR(IDC_BUTTON_SETLANG_GENERATE), "导出模板"},
	{SECT_DIALOG_SETLANG, LANGUAGE_STR(IDC_BUTTON_SETLANG_IMPORT), "导入语言"},
	{SECT_DIALOG_SETLANG, LANGUAGE_STR(IDC_BUTTON_SETLANG_OPENFILES), "浏览语言"},

	// 设置系统参数
	{SECT_DIALOG_SETTINGS, "TITLE", "系统设置"},

	// 设置软件参数
	{SECT_DIALOG_SETTINGSOFTWARE, "TITLE", "软件设置"},

	// 启动对话框
	{SECT_DIALOG_STARTUP, "ERR_EXPIRE_SOON", "使用期限还剩1天，请联系设备厂商"},
	{SECT_DIALOG_STARTUP, "ERR_WRONG_COM", "该串口号无效"},
	{SECT_DIALOG_STARTUP, "WS_READ_REG_STATUS", "加载系统注册状态"},
	{SECT_DIALOG_STARTUP, "WS_READ_REG_STATUS_FAIL", "系统注册失败，将无法正常加工，若仍需进入，请点击继续"},
	{SECT_DIALOG_STARTUP, "WS_READ_DOG", "验证加密狗状态"},
	{SECT_DIALOG_STARTUP, "WS_READ_IO_CONFIG_IN", "加载IO端口配置"},
	{SECT_DIALOG_STARTUP, "WS_READ_SETTINGS", "加载系统参数"},
	{SECT_DIALOG_STARTUP, "WS_INIT_PARAM_PREFIXES", "初始化参数名字"},
	{SECT_DIALOG_STARTUP, "WS_READ_SERVO_SET_RESULT", "设置伺服驱动器"},
	{SECT_DIALOG_STARTUP, "WS_READ_SERVO_SET_RESULT_FAIL", "伺服驱动器初始化失败，点击继续。请配置好轴参数并检查伺服连线，然后重启软件"},
	{SECT_DIALOG_STARTUP, "WS_EXITING", "正在退出软件"},
	{SECT_DIALOG_STARTUP, "WS_ABORTING", "中断连接"},
	{SECT_DIALOG_STARTUP, "TITLE", "启动中"},

	// 用户管理对话框
	{SECT_DIALOG_USERS, "KEEP_UNCHANGED", "保持不变"},
	{SECT_DIALOG_USERS, "CUR_PWD", "当前密码"},
	{SECT_DIALOG_USERS, "NEW_PWD", "新密码"},
	{SECT_DIALOG_USERS, "INVALID_INPUT", "输入范围不合法，请输入0-99999999数字"},
	{SECT_DIALOG_USERS, "TITLE", "权限与用户"},

	// 轴重命名对话框列
	{SECT_LIST_AXIS_RENAME, "HEADER_INDEX", "轴序号"},
	{SECT_LIST_AXIS_RENAME, "HEADER_NAME_OLD", "原名称"},
	{SECT_LIST_AXIS_RENAME, "HEADER_NAME_NEW", "新名称"},
	{SECT_LIST_AXIS_RENAME, "HEADER_NAME_SWITCH", "轴开关"},

	// 加密狗加密对话框列
	{SECT_LIST_DOGPASSWORD, "HEADER_INDEX", "第几期"},
	{SECT_LIST_DOGPASSWORD, "HEADER_PASSWORD", "密码"},
	{SECT_LIST_DOGPASSWORD, "HEADER_DAYS", "天数"},

	// 模型生成对话框列
	{SECT_LIST_MODEL, "HEADER_INDEX", "序号"},
	{SECT_LIST_MODEL, "HEADER_NAME", "名称"},
	{SECT_LIST_MODEL, "HEADER_VALUE", "值"},
	{SECT_LIST_MODEL, "HEADER_HINT", "说明"},

	// 设置IO对话框列
	{SECT_LIST_SETTINGIO, "HEADER_INDEX", "序号"},
	{SECT_LIST_SETTINGIO, "HEADER_NAME", "名称"},
	{SECT_LIST_SETTINGIO, "HEADER_MAP", "映射IO"},
	{SECT_LIST_SETTINGIO, "HEADER_LOGIC", "有效电平"},

	// 系统设置列
	{SECT_LIST_SETTINGS, "HEADER_INDEX", "序号"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_NAME", "参数"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_VALUE", "值"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_RANGE", "范围"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_USER", "修改权"},
	{SECT_LIST_SETTINGS, "PERMISSION_READ_ONLY", "只读"},

	// 教导指令编辑错误信息
	{SECT_ERRORS_TEACHING_1, "ERR_DELETING_S", "错误: 第一行S指令不能删除"},
	{SECT_ERRORS_TEACHING_1, "ERR_CONFIRM_DEL_ALL", "该操作不能撤销，确实删除所有行？"},
	{SECT_ERRORS_TEACHING_1, "ERR_UNDO_FAIL", "错误：无法撤销到更早的修改"},
	{SECT_ERRORS_TEACHING_1, "ERR_REDO_FAIL", "错误：无法恢复到最近的修改"},
	{SECT_ERRORS_TEACHING_1, "ERR_UNKNOWN_CMD", "未知的教导指令"},
	{SECT_ERRORS_TEACHING_1, "ERR_MOVE_SAME_LINE_S_PLUS", "错误：\"+\",\"-\",\"S\"指令和探针不能同时存在于MOVE指令行"},
	{SECT_ERRORS_TEACHING_1, "HINT_SINGLE", "单圈模式：输入范围"},
	{SECT_ERRORS_TEACHING_1, "HINT_MULTI", "多圈模式：输入范围-100~100(圈)，四位小数点"},
	{SECT_ERRORS_TEACHING_1, "HINT_SCREW", "丝杆模式：输入范围-999999~1999999"},
	{SECT_ERRORS_TEACHING_1, "HINT_FEED", "送线模式：输入范围-999999~1999999"},
	{SECT_ERRORS_TEACHING_1, "HINT_CMD_TYPE", "M:运动, E:结束, S:速度, J/G:跳转, L:循环, N:标志"},
	{SECT_ERRORS_TEACHING_1, "HINT_SPEED", "速度比例：输入范围0.01~5.0"},
	{SECT_ERRORS_TEACHING_1, "HINT_PROBE", "探针(1~4)，输入(5~24)，检长(255)"},
	{SECT_ERRORS_TEACHING_1, "HINT_CYLINDER", "气缸：0关闭所有,负号关闭单个,逗号隔开各个气缸"},
	{SECT_ERRORS_TEACHING_1, "HINT_DELAY", "延时：输入范围0.01~500，单位为秒"},

	// 指令编辑单元格检验错误信息
	{SECT_ERRORS_TEACHING_2, "ERR_EXCEED_FEEDDING", "错误：送线轴速度设定大于送线最大值"},
	{SECT_ERRORS_TEACHING_2, "ERR_SPEED_NEG", "错误：速度设定小于0"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_SPEED_COLUNM", "错误：速度指令行中，数据只能键入到轴所在列"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_FEED", "错误：送线长度合法范围为-999999~1999999"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_SINGLE", "错误：单圈模式合法范围为"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_MULTI_1", "错误：多圈模式小数点后最大范围为0.3600"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_MULTI_2", "错误：多圈模式圈数有效范围为-100~100"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RUNMODE", "错误：不支持的轴运行模式"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_SPEED", "错误：速度比例有效范围为0.01~5.0"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_PROBE", "错误：探针有效范围为1~255"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_CYLINDER", "错误：汽缸合法输入范围0~20"},
	{SECT_ERRORS_TEACHING_2, "ERR_NOT_NUMBER", "错误：输入数据不是数字"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_JUMP", "错误：跳转行数合法范围1~200(MAXLINE)"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_JUMP_COLUMN", "错误：跳转指令行中，数据只能键入到第一轴"},
	{SECT_ERRORS_TEACHING_2, "ERR_END_NOT_EMPTY", "错误：E指令行中，键入数据必须为空"},

	// 指令编辑列
	{SECT_LIST_TEACH, "HEADER_CMD", "指令"},
	{SECT_LIST_TEACH, "HEADER_SPEED", "速比"},
	{SECT_LIST_TEACH, "HEADER_PROBE", "探针"},
	{SECT_LIST_TEACH, "HEADER_CYLINDER", "气缸"},
	{SECT_LIST_TEACH, "HEADER_DELAY", "延时"},
	{SECT_LIST_TEACH, "HEADER_END_ANGLE", "结束角"},
	{SECT_LIST_TEACH, "HEADER_BEGIN_ANGLE", "开始角"},

	// 其他
	{SECT_USER_NAMES,"HINT_CURRENT_USER","当前用户："},
	{SECT_USER_NAMES,"HINT_PLEASE_CHANGE_USER","要使用该功能，请升级为"},

	{-1} // end
};

// 带有ID映射的键值对
const CLanguage::LANG_MAPKEY_VAL_T CLanguage::DEFAULT_MAPPING_TEXTS[]={
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_YEAR), "年"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_MONTH), "月"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_DAY), "日"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_HOUR), "时"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_MINUTE), "分"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_SECOND), "秒"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_RESULT_SUCCESS), "成功"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_RESULT_FAIL), "失败"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_RESULT_ERROR), "错误"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_INVALID_RANGE), "输入范围不符合"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_INVALID_PASSWORD), "无效的密码"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_MODBUS_BUSY), "通讯中，请稍后再试"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_MODBUS_FAIL), "通讯失败，是否重试？"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_MODBUS_FAIL_LOADWF), "通讯失败(加载文件)，是否重试？"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_BUTTON_OK), "确定"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_BUTTON_CANCEL), "取消"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_BUTTON_SAVE), "保存"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_USER_MINIMAL_LEVEL), "所需最低权限"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_USER_CURRENT_LEVEL), "当前用户"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_ENABLED), "启用"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_DISABLED), "禁用"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_ON), "开"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_OFF), "关"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TITLE_HINT), "提示"},

	// 其他
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_GUEST), "来宾"},
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_USER), "操作员"},
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_PROGRAMER), "调试员"},
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_SUPER), "超级用户"},

	{0,0,std::string()} // end
};

CLanguage::CLanguage(std::string filename, bool isReadOnly)
{
	iniLang=new CIniLanguage(filename, isReadOnly);

	_initSectionArray(); // 初始化Section段落数组
	_initDiaglogTexts(); // 初始化对话框文字
	_initMapTexts(); // 初始化映射ID的文字
	_initAlarmMsg(); // 初始化报警信息
	_initParamHints(); // 初始化参数提示
	_initIONames(); // 初始化IO名字
}

CLanguage::~CLanguage(void)
{
	while(!menuStrings.empty()){
		auto p=menuStrings.back();
		delete p;
		menuStrings.pop_back();
	}

	delete_pointer(iniLang);
}

CString CLanguage::getStringById(int id, int section_index)
{
	if(section_index >= SECT_MAX_INDEX){
		debug_printf("CLanguage::getStringById: wrong index, skip\n");
		return CString();
	}

	std::string res=iniLang->get_language_string_by_id(id,*_section_indexes[section_index]);
	return StringToCString(res);
}

CString CLanguage::getString(std::string key, int section_index, const std::string& default_value)
{
	if(section_index >= SECT_MAX_INDEX){
		debug_printf("CLanguage::getString1: wrong index, skip\n");
		return CString();
	}

	std::string res=iniLang->get_language_string(key,*_section_indexes[section_index],default_value);
	return StringToCString(res);
}

CString CLanguage::getString(std::string key, int section_index)
{
	if(section_index >= SECT_MAX_INDEX){
		debug_printf("CLanguage::getString2: wrong index, skip\n");
		return CString();
	}

	std::string res=iniLang->get_language_string(key,*_section_indexes[section_index]);
	return StringToCString(res);
}

void CLanguage::setDialogText(CWnd* context, LANG_ITEM_T* lang_array)
{
	auto i=0;
	while(std::string() != (lang_array+i)->id_str){//为最后的停止指令
		context->GetDlgItem((lang_array+i)->id)->SetWindowText(getString((lang_array+i)->id_str,(lang_array+i)->section_index));
		i++;
	}
}

void CLanguage::setMenuText(CMenu* menu, LANG_ITEM_T* lang_array)
{
	auto i=0;
	while(std::string() != (lang_array+i)->id_str){
		CString* got_str=new CString(getString((lang_array+i)->id_str,(lang_array+i)->section_index));
		menuStrings.push_back(got_str);
		menu->ModifyMenu((lang_array+i)->id,MF_BYCOMMAND | MF_OWNERDRAW,(lang_array+i)->id,(LPCTSTR)got_str);
		i++;
	}
}

void CLanguage::setMenuPopupText(CMenu* menu, LANG_ITEM_T* lang_array, int max_count)
{
	int index_str=0;
	int uID;
	int itemCount = menu->GetMenuItemCount();

	// https://stackoverflow.com/questions/20039268/how-do-i-change-popup-text-of-menu-without-id
	for(int i=0;i<itemCount;i++){
		uID = menu->GetMenuItemID(i);
		if (uID == (UINT)-1){
			CString* got_str=new CString(getString((lang_array+i)->id_str,(lang_array+i)->section_index));
			menuStrings.push_back(got_str);
			menu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, 0, (LPCTSTR)got_str);
			index_str++;

			// 避免数组越界
			if(index_str>=max_count)
				return;
		}
	}
}

void CLanguage::mapIDs(const LANG_MAPKEY_VAL_T* lang_array)
{
	auto i=0;
	while(std::string() != (lang_array+i)->id_str){
		mapID((lang_array+i)->id,(lang_array+i)->id_str,(lang_array+i)->section_index,(lang_array+i)->value);
		i++;
	}
}

void CLanguage::mapID(int id, std::string id_str, int section_index, const std::string& default_value)
{
	if(section_index >= SECT_MAX_INDEX){
		debug_printf("CLanguage::mapID: wrong index, skip\n");
		return;
	}

	iniLang->set_language_string_id(id,id_str,*_section_indexes[section_index],default_value);
}

void CLanguage::_initSectionArray()
{
	const LANG_SECT_ARRAY_T* item;

	for(item=&CLanguage::DEFAULT_SECTIONS[0]; item->section_index >= 0 ; ++item){
		if(item->section_index >= SECT_MAX_INDEX){
			debug_printf("CLanguage::_initSectionArray(): wrong index, skip\n");
			continue;
		}
		_section_indexes[item->section_index]=&item->section;
	}
}

void CLanguage::_initDiaglogTexts()
{
	const LANG_KEY_VAL_T* item;

	for(item=&CLanguage::DEFAULT_TEXTS[0]; item->section_index >= 0 ; ++item){
		if(item->section_index >= SECT_MAX_INDEX){
			debug_printf("CLanguage::_initAllString(): wrong index, skip\n");
			continue;
		}
		
		iniLang->get_language_string(item->key,*_section_indexes[item->section_index],item->value);
	}
}

void CLanguage::_initMapTexts()
{
	mapIDs(DEFAULT_MAPPING_TEXTS);
}

void CLanguage::_initAlarmMsg()
{
	auto offset=0;
	unsigned short alarmNum;

	auto copy_str = [](char* dst, char* src, int dst_len){
		sprintf_s(dst, dst_len, "%s", src);
	};

	// 特殊报警（有轴号前缀）
	for(auto i=0; i<sizeof(Alarm)/sizeof(Alarm[0]); i++){
		alarmNum=Alarm[i].fun;
		switch(alarmNum){
		case X_P_SoftLimit_Alarm:
		case Y_P_SoftLimit_Alarm:
		case Z_P_SoftLimit_Alarm:
		case A_P_SoftLimit_Alarm:
		case B_P_SoftLimit_Alarm:
		case C_P_SoftLimit_Alarm:
		case D_P_SoftLimit_Alarm:
		case E_P_SoftLimit_Alarm:
		case X1_P_SoftLimit_Alarm:
		case Y1_P_SoftLimit_Alarm:
		case Z1_P_SoftLimit_Alarm:
		case A1_P_SoftLimit_Alarm:
		case B1_P_SoftLimit_Alarm:
		case C1_P_SoftLimit_Alarm:
		case D1_P_SoftLimit_Alarm:
		case E1_P_SoftLimit_Alarm:
			offset=(alarmNum-X_P_SoftLimit_Alarm)/2;
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "正软件限位", 100);
			break;

		case X_M_SoftLimit_Alarm:
		case Y_M_SoftLimit_Alarm:
		case Z_M_SoftLimit_Alarm:
		case A_M_SoftLimit_Alarm:
		case B_M_SoftLimit_Alarm:
		case C_M_SoftLimit_Alarm:
		case D_M_SoftLimit_Alarm:
		case E_M_SoftLimit_Alarm:
		case X1_M_SoftLimit_Alarm:
		case Y1_M_SoftLimit_Alarm:
		case Z1_M_SoftLimit_Alarm:
		case A1_M_SoftLimit_Alarm:
		case B1_M_SoftLimit_Alarm:
		case C1_M_SoftLimit_Alarm:
		case D1_M_SoftLimit_Alarm:
		case E1_M_SoftLimit_Alarm:
			offset=(alarmNum-X_M_SoftLimit_Alarm)/2;
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "负软件限位", 100);
			break;

		case X_P_HardLimit_Alarm:
		case Y_P_HardLimit_Alarm:
		case Z_P_HardLimit_Alarm:
		case A_P_HardLimit_Alarm:
		case B_P_HardLimit_Alarm:
		case C_P_HardLimit_Alarm:
		case D_P_HardLimit_Alarm:
		case E_P_HardLimit_Alarm:
		case X1_P_HardLimit_Alarm:
		case Y1_P_HardLimit_Alarm:
		case Z1_P_HardLimit_Alarm:
		case A1_P_HardLimit_Alarm:
		case B1_P_HardLimit_Alarm:
		case C1_P_HardLimit_Alarm:
		case D1_P_HardLimit_Alarm:
		case E1_P_HardLimit_Alarm:
			offset=(alarmNum-X_P_HardLimit_Alarm)/2;
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "正硬件限位报警", 100);
			break;

		case X_M_HardLimit_Alarm:
		case Y_M_HardLimit_Alarm:
		case Z_M_HardLimit_Alarm:
		case A_M_HardLimit_Alarm:
		case B_M_HardLimit_Alarm:
		case C_M_HardLimit_Alarm:
		case D_M_HardLimit_Alarm:
		case E_M_HardLimit_Alarm:
		case X1_M_HardLimit_Alarm:
		case Y1_M_HardLimit_Alarm:
		case Z1_M_HardLimit_Alarm:
		case A1_M_HardLimit_Alarm:
		case B1_M_HardLimit_Alarm:
		case C1_M_HardLimit_Alarm:
		case D1_M_HardLimit_Alarm:
		case E1_M_HardLimit_Alarm:
			offset=(alarmNum-X_M_HardLimit_Alarm)/2;
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "负硬件限位报警", 100);
			break;

		case X_Servo_Alarm:
		case Y_Servo_Alarm:
		case Z_Servo_Alarm:
		case A_Servo_Alarm:
		case B_Servo_Alarm:
		case C_Servo_Alarm:
		case D_Servo_Alarm:
		case E_Servo_Alarm:
		case X1_Servo_Alarm:
		case Y1_Servo_Alarm:
		case Z1_Servo_Alarm:
		case A1_Servo_Alarm:
		case B1_Servo_Alarm:
		case C1_Servo_Alarm:
		case D1_Servo_Alarm:
		case E1_Servo_Alarm:
			offset=(alarmNum-X_Servo_Alarm);
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "伺服报警", 100);
			break;

		case GoZero_FailX:
		case GoZero_FailY:
		case GoZero_FailZ:
		case GoZero_FailA:
		case GoZero_FailB:
		case GoZero_FailC:
		case GoZero_FailD:
		case GoZero_FailE:
		case GoZero_FailX1:
		case GoZero_FailY1:
		case GoZero_FailZ1:
		case GoZero_FailA1:
		case GoZero_FailB1:
		case GoZero_FailC1:
		case GoZero_FailD1:
		case GoZero_FailE1:
			offset=(alarmNum-GoZero_FailX);
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "归零失败", 100);
			break;

		case SetStartOffsetPromitX:
		case SetStartOffsetPromitY:
		case SetStartOffsetPromitZ:
		case SetStartOffsetPromitA:
		case SetStartOffsetPromitB:
		case SetStartOffsetPromitC:
		case SetStartOffsetPromitD:
		case SetStartOffsetPromitE:
		case SetStartOffsetPromitX1:
		case SetStartOffsetPromitY1:
		case SetStartOffsetPromitZ1:
		case SetStartOffsetPromitA1:
		case SetStartOffsetPromitB1:
		case SetStartOffsetPromitC1:
		case SetStartOffsetPromitD1:
		case SetStartOffsetPromitE1:
			offset=(alarmNum-SetStartOffsetPromitX);
			map_alarm_num_idx_with_axis[alarmNum]=offset;
			copy_str(Alarm[i].name, "原点偏移设置成功", 100);
			break;

		default:break;
		}
	}

	std::stringstream ss_id;
	char alarmNumStr[10];
	for(auto i=0; i<sizeof(Alarm)/sizeof(Alarm[0]); i++){
		alarmNum=Alarm[i].fun;

		ss_id.str(std::string()); // flush all data
		sprintf_s(alarmNumStr,10,"%04u",alarmNum);
		ss_id << "W_" << alarmNumStr;
		
		mapID(alarmNum,ss_id.str(),CLanguage::SECT_WARNING_MESSAGE,std::string(Alarm[i].name));
	}
}

void CLanguage::_initParamHints()
{
	std::stringstream ss_id;
	char numberStr[10];
	int title_index=0;
	std::string param_key_str;

	for(int i=0; ParaRegTab[i].DataType!=DT_END; i++){
		// 标题
		if(DT_TITLE == ParaRegTab[i].DataType){
			ss_id.str(std::string()); // flush all data
			sprintf_s(numberStr,10,"%02d",title_index);
			ss_id << "TITLE_" << numberStr;

			map_param_titles[title_index]=getString(ss_id.str(),CLanguage::SECT_PARAM_NAMES,ParaRegTab[i].pDescText);
			title_index++;

			continue;
		}

		// 非标题
		if(NULL==ParaRegTab[i].ParamName)
			continue;

		if(DT_NONE == ParaRegTab[i].DataType)
			continue;

		param_key_str=std::string(ParaRegTab[i].ParamName);

		map_param_names[i] = getString(param_key_str,CLanguage::SECT_PARAM_NAMES,ParaRegTab[i].pDescText);
		map_param_descriptions[i] = getString(param_key_str,CLanguage::SECT_PARAM_DESCRIPTION,ParaRegTab[i].pRangeText);
	}
}

CString CLanguage::getAlarmMsg(int alarmNumber)
{
	CString text;

	CString msg_AlarmNum=_T("");
	msg_AlarmNum.Format(_T("%04u: "),alarmNumber);

	if(map_alarm_num_idx_with_axis.find(alarmNumber) == map_alarm_num_idx_with_axis.end()) // 查无键值
		text=getStringById(alarmNumber,CLanguage::SECT_WARNING_MESSAGE);
	else
		text=CharsToCString(g_axisName[map_alarm_num_idx_with_axis[alarmNumber]]) + getStringById(alarmNumber,CLanguage::SECT_WARNING_MESSAGE);

	return (msg_AlarmNum+text);
}

CString CLanguage::getParamName(int index)
{
	if(map_param_names.find(index) == map_param_names.end()) // 查无键值
		return _T("?");

	return map_param_names[index];
}

CString CLanguage::getParamDescription(int index)
{
	if(map_param_descriptions.find(index) == map_param_descriptions.end()) // 查无键值
		return _T("?");

	return map_param_descriptions[index];
}

CString CLanguage::getParamTitle(int title_index)
{
	if(map_param_titles.find(title_index) == map_param_titles.end()){ // 查无键值
		return _T("?");
	}

	return map_param_titles[title_index];
}

CString CLanguage::getCommonText(int id)
{
	return CString(getStringById(id,SECT_COMMON_TEXTS));
}

void CLanguage::_initIONames()
{
	int i;
	std::stringstream ss_io_param;
	char formattedStr[10];
	char* defaultName=nullptr;

	for(i=0;i<MAX_INPUT_NUM;++i){ // 诊断界面的IN名字
		ss_io_param.str(std::string()); // flush all data
		sprintf_s(formattedStr,10,"%03d",i);
		ss_io_param << "IN_" << formattedStr;

		Get_Input_Fun_Name(i, &defaultName);
		if(nullptr == defaultName)
			continue;

		iniLang->get_language_string(ss_io_param.str(),
			*_section_indexes[SECT_IO_NAMES_IN],
			std::string(defaultName));
	}

	for(i=0;i<MAX_OUTPUT_NUM;++i){ // 诊断界面的OUT名字
		ss_io_param.str(std::string()); // flush all data
		sprintf_s(formattedStr,10,"%03d",i);
		ss_io_param << "OUT_" << formattedStr;

		Get_Output_Fun_Name(i, &defaultName);
		if(nullptr == defaultName)
			continue;

		iniLang->get_language_string(ss_io_param.str(),
			*_section_indexes[SECT_IO_NAMES_OUT],
			std::string(defaultName));
	}
}

CString CLanguage::getIOName(int index, bool isInput)
{
	std::stringstream ss_io_param;
	char formattedStr[10];
	CString str;

	sprintf_s(formattedStr,10,"%03d",index);

	if(true == isInput){
		ss_io_param << "IN_" << formattedStr;
		str=getString(ss_io_param.str(),SECT_IO_NAMES_IN);

		if(index >= X_SERVO_ALARM && index <= C_SERVO_ALARM) // 6个脉冲轴 报警
			str=CString(g_axisName[(index-X_SERVO_ALARM)%16]) + str;
		else if(index >= X_HOME && index <= E1_M_LIMIT) // 原点，限位+- 报警
			str=CString(g_axisName[(index-X_HOME)%16]) + str;
	}else{
		ss_io_param << "OUT_" << formattedStr;
		str=getString(ss_io_param.str(),SECT_IO_NAMES_OUT);

		if(index >= OUT_X_SERVO_CLEAR && index <= OUT_C_SERVO_CLEAR) // 脉冲轴 报警复位
			str=CString(g_axisName[(index-OUT_X_SERVO_CLEAR)%16]) + str;
	}

	if(CString() == str)
		return _T("???");

	return str;
}