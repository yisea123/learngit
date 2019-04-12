/*
 * Language.cpp
 *
 * ��װIniLanguage���API�ӿں���
 *
 * Created on: 2018��4��17������10:08:14
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
	// ���Ի���ťԪ��
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_BACKZERO), "����(F2)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_CHECKIO), "���(F10)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_CUT), "����(F9)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_ADD_LINE), "�����(Z)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_ALL_LINES), "ȫɾ��(V)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_DELETE_LINE), "ɾ����(C)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_INSERT_LINE), "������(X)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_REDO), "�ָ�(N)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_EDIT_INSTRUCTIONS_UNDO), "����(B)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_HAND), "����(F7)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_KNIFE), "�˵�(F8)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_PROBE), "�趨"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_PRODUCTION), "�趨"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_RESETSONGXIAN), "��������"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_SPEEDRATE), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_OPENFILE), "��(F1)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_PROBE_FAIL_COUNTER_CLEAR2), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_PRODUCT_COUNT_CLEAR), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_RESET_ALARM), "���(F11)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_RUN), "����(F4)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MODELS), "ģ��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_SET_ORIGIN), "��ԭ��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_SINGLE), "����(F3)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_STOP), "��ͣ(F5)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_TEACH_FROM_1600E), "�̵�(M)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_TEST), "����(F6)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_DANBU), "����(O)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_WORKSPEED_RATE), "�ٶ�(F12)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_NOW_PROG), "״̬"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_PC_MOVE), "�ᶯ"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_STATISTIC), "ͳ��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_GROUPBOX_TEST), "���Լӹ�"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_CUR_AXISNAME), "ѡ��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_CUR_HANDRATE), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_DESTCOUNT), "�����趨"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_FILENAME), "��������"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_POSITION), "��ǰ����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_PROBE), "̽���趨"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_PROBEFAIL), "̽��ʧ��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_PRODUCTION), "ʵ������"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_REMAIN_TIME), "ʣ��ʱ��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_RUNSPEED), "�ӹ��ٶ�"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_RUNSPEED_UNIT), "��/����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_SPEEDRATE), "�ٶȰٷֱ�"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_HINT_SPRINGDLG_WORKSTATUS), "����״̬"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_CYLINDER_OUT), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_PROBE_IN), "̽��"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_STOP_IN), "��ͣ"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_STATIC_TEXT_XIANJIA_IN), "�߼�"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_O_1), "Һ����(Q)"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(IDC_BUTTON_MAIN_O_2), "Һ����(W)"},

	// ������Ĳ˵�
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32776), "��������/����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32777), "ϵͳ����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32778), "�������"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32782), "�鿴������־"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32783), "����־�ļ���"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32785), "�û��ֲ�"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32786), "����汾"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32860), "IP��ַ����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32864), "��ϵ�豸����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32865), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32866), "�ᶯͼ����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32862), "����"},
	{SECT_DIALOG_SPRING, LANGUAGE_STR(ID_32863), "����"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_1","��������"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_2","��־"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_3","�ۺ����"},
	{SECT_DIALOG_SPRING,"MENU_POPUP_TEXT_4","������ܹ�"},

	// �������������ʾ��Ϣ
	{SECT_DIALOG_SPRING, "LOADING_WORKFILE_HINT", "��ȡ�ӹ��ļ��У����Ժ�"},
	{SECT_DIALOG_SPRING, "AXIS_SELECTED_UNSPECIFIED", "δѡ��"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_STOP", "ֹͣ"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_RUN", "���ڼӹ�"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_PAUSE", "��ͣ"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_SINGLE", "����"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_ZERO", "����"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_HANDMOVE", "��ҡ�ᶯ"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_PCMOVE", "PC�ᶯ"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_USBLINK", "USB������"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_CUTWIRE", "������"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_TUIDAO", "�˵���"},
	{SECT_DIALOG_SPRING, "STATUS_TEXT_FTPLINK", "FTP����"},
	{SECT_DIALOG_SPRING, "SET_PRODUCTION_DLG_TITLE", "�������֣���Χ0-9999999"},
	{SECT_DIALOG_SPRING, "SET_PROBE_DLG_TITLE", "�������֣���Χ0-999"},
	{SECT_DIALOG_SPRING, "PROD_CLEAR_CONFIRM", "�Ƿ�������㣿"},
	{SECT_DIALOG_SPRING, "HANDBOX_RATIO_LOW", "��"},
	{SECT_DIALOG_SPRING, "HANDBOX_RATIO_MID", "��"},
	{SECT_DIALOG_SPRING, "HANDBOX_RATIO_HIGH", "��"},
	{SECT_DIALOG_SPRING, "SET_ORIGIN_INVALID_AXIS", "��ѡ����ȷ�����"},
	{SECT_DIALOG_SPRING, "SET_ORIGIN_CURRENT_AXIS", "��ǰѡ���᣺"},
	{SECT_DIALOG_SPRING, "SET_ORIGIN_CONFIRM", "ȷʵҪ���ø���ԭ�㣿"},
	{SECT_DIALOG_SPRING, "TEACH_ERR_HINT", "�̵���Ч��������ĳ�����Ƕ�Ȧģʽ����Ϊ���ģʽ"},
	{SECT_DIALOG_SPRING, "INVALID_MACHINE_BACKKNIFE", "�˵����ܽ�������12�����ϵ��ɻ�"},
	{SECT_DIALOG_SPRING, "SET_BACKKNIFE_DLG_TITLE", "�˵��ٶȰٷֱȣ���Χ0-500"},
	{SECT_DIALOG_SPRING, "SET_WORKSPEED_DLG_TITLE", "�ܼӹ��ٶȰٷֱȣ���Χ0-150"},
	{SECT_DIALOG_SPRING, "DOG_VALIDATION_HINT_SERIAL", "�������к�:"},
	{SECT_DIALOG_SPRING, "DOG_VALIDATION_HINT_SERIAL2", "�������к�:"},
	{SECT_DIALOG_SPRING, "DOG_VALIDATION_HINT_INPUT_PWD", "��������ܹ���������"},
	{SECT_DIALOG_SPRING, "DEST_COUNT_REACH_HINT", "�ӹ���ɣ��ѴﵽĿ�������"},
	{SECT_DIALOG_SPRING, "INVALID_MACHINE_MODEL", "ģ�͹��ܽ�������6��ѹ�ɻ�"},
	{SECT_DIALOG_SPRING, "INVALID_MACHINE_CUTWIRE", "���߹��ܽ�������12�����ϵ��ɻ�"},
	{SECT_DIALOG_SPRING, "ON_CANCEL_PWD_TITLE", "��������λ�˳�����"},
	{SECT_DIALOG_SPRING, "LOG_WAS_DISABLE", "��־�ѹرգ��������������������"},

	// �򿪶Ի���
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_STATIC_OPENFILE_TOTAL_TEXT), "�ļ�����"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_BUTTON_OPENFILE_COPY), "����"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_BUTTON_OPENFILE_NEWFILE), "�½�"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDC_BUTTON_OPENFILE_DELETE), "ɾ��"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDOK), "��"},
	{SECT_DIALOG_OPENFILE, LANGUAGE_STR(IDCANCEL), "ȡ��"},

	// ����Ի���
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_SINGLE), "�������"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_IS_MACHINE), "��е���"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_IS_PROG), "�������"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_MULTIPLE), "ȫ������"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDOK), "���������"},
	{SECT_DIALOG_BACKZERO, LANGUAGE_STR(IDC_STATIC_BACKZERODLG_FORCE_MACHINE), "ǿ�ƻ�е����"},

	// ϵͳ�汾�Ի���
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_REGISTER), "��Ʒע��״̬"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_PRODUCTID), "��������ƷID"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_MOTION_LIB), "�������˶���"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_ZYNQ), "������LIB�汾"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_FPGA1), "������FPGA1�汾"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_FPGA2), "������FPGA2�汾"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_MFC), "��λ������汾"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_VERSION_CONTROLLER), "����������汾"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_BUILDDATE_MFC), "��λ����������"},
	{SECT_DIALOG_ABOUT, LANGUAGE_STR(IDC_STATIC_ABOUT_HINT_BUILDDATE_CONTROLLER), "��������������"},

	// ���߶Ի���
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_MAIN), "����"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_MAIN_AXIS), "��ѡ��"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_MAIN_ANGLE), "�Ƕ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_MAIN_SPEED), "�ٶ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_OPT), "������"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_OPT_AXIS), "��ѡ��"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_OPT_ANGLE), "�Ƕ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_OPT_SPEED), "�ٶ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_SCISSOR), "Һѹ��"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_CHECK_CUT_WIRE_USE_SISSOR), "ʹ��Һѹ��"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_DELAY), "��ʱ(��)"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_HINT_OTHERS), "����"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_ZHUANXIN_ANGLE), "תо�Ƕ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_ZHUANXIN_SPEED), "תо�ٶ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_SONGXIAN_POS), "����λ��"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDC_STATIC_CUTWIREDLG_SONGXIAN_SPEED), "�����ٶ�"},
	{SECT_DIALOG_CUTWIRE, LANGUAGE_STR(IDOK), "ִ�м���"},

	// ���ܹ������ܶԻ���
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_TIME), "������ʱ��"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_REMAIN), "ʣ������"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_RANDOM), "�������"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_UNLOCK), "������"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_ISUNLOCK), "����״̬"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_SERIAL2), "�������к�"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDOK), "ȷ��"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_SERIAL), "�������к�"},
	{SECT_DIALOG_DOGDECRYPT, LANGUAGE_STR(IDC_STATIC_DOGDECRYPT_HINT_CURRENT), "��ǰ����"},

	// ���ܹ������ܶԻ���
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_STATUS), "���ܹ�״̬����"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_REMAIN), "ʣ������(���λ)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_RANDOM), "�������"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_SERIAL), "�������к�(���λ)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_SERIAL2), "�������к�(���λ)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_PWD), "��������(�ʮλ)"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_TIME), "������ʱ��"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_CHECK_DOGENCRYPTDLG_SETTIME), "�Ŀ�����ʱ��"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_BUTTON_DOGENCRYPTDLG_INIT), "��ʼ��"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_BUTTON_DOGENCRYPTDLG_SAVE), "Ӧ��"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDOK), "ȷ��"},
	{SECT_DIALOG_DOGENCRYPT, LANGUAGE_STR(IDC_STATIC_DOGENCRYPT_HINT_CURRENT), "��ǰ����"},

	// ����1600��IP�Ի���
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_IP), "������IP"},
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_MASK), "��������������"},
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_GW), "������Ĭ������"},
	{SECT_DIALOG_SET1600EIP, LANGUAGE_STR(IDC_STATIC_SETIP_1600_HINT_MAC), "������MAC��ַ"},

	// ������λ��Ŀ��IP�Ի���
	{SECT_DIALOG_SETDESTIP, LANGUAGE_STR(IDC_STATIC_SETIP_DEST_HINT_IP), "��ַ"},
	{SECT_DIALOG_SETDESTIP, LANGUAGE_STR(IDC_STATIC_SETIP_DEST_HINT_TIMEOUT), "��ʱ(ms)"},
	{SECT_DIALOG_SETDESTIP, LANGUAGE_STR(IDC_STATIC_SETIP_DEST_HINT_RETRY), "�ط�����"},

	// ϵͳ���öԻ���
	{SECT_DIALOG_SETTINGS, LANGUAGE_STR(IDC_STATIC_SETTINGSDLG_RANGE), "���뷶Χ"},
	{SECT_DIALOG_SETTINGS, "CHANGE_USER_HINT", "Ҫ�޸ģ����л��û���"},
	
	// �������
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_STATIC_SETTING_SW_HINT), "��ҳ������λ������Ϊ"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_LOG), "������־����"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_EXITHINT), "�˳�ǰ��Ҫ����"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_TOPMOST), "����������ǰ"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_CHECK_SETTING_SW_MAXIMIZED), "��������ʱ���"},
	{SECT_DIALOG_SETTINGSOFTWARE, LANGUAGE_STR(IDC_STATIC_WINDOWSTITLE_HINT), "���ڱ���"},

	// �����Ի���
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_BUTTON_REFRESH_SERIAL), "ˢ��"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_BUTTON_STARTUP_CONTINUE), "����"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_BUTTON_STARTUP_STOP), "�ж�����"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_RADIO_CONN_TYPE_IP), "IP"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_RADIO_CONN_TYPE_SERIAL), "����"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_1), "��ʱ"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_2), "����"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_OPT1), "ͨѶ��ʽ"},
	{SECT_DIALOG_STARTUP, LANGUAGE_STR(IDC_STATIC_STARTUP_OPT2), "�߼�ѡ��"},

	// Ȩ�޹���Ի���
	{SECT_DIALOG_USERS, LANGUAGE_STR(IDC_STATIC_USERS_HINT_CUR), "��ǰ�û�"},
	{SECT_DIALOG_USERS, LANGUAGE_STR(IDC_STATIC_USERS_HINT_SWITCH), "�л��û�"},
	{SECT_DIALOG_USERS, LANGUAGE_STR(IDC_STATIC_USERS_HINT_PWD), "�޸�����"},

	// �������Ի���
	{SECT_DIALOG_AXIS_RENAME,"TITLE","������"},

	// ����Ի���
	{SECT_DIALOG_BACKZERO,"TITLE","����"},

	// ���ڱ����
	{SECT_DIALOG_ABOUT,"TITLE","���ڱ����"},

	// ���߶Ի���
	{SECT_DIALOG_CUTWIRE, "SUFFIX_DISABLED", "��ͣ��"},
	{SECT_DIALOG_CUTWIRE, "ERR_NO_SAME_AXIS", "����������������ѡ����ͬ����"},
	{SECT_DIALOG_CUTWIRE, "TITLE", "����"},

	// ��϶Ի���
	{SECT_DIALOG_DIAGNOSE, "TITLE", "IO��ϣ���ѡ����ߵ�ƽ"},
	{SECT_DIALOG_DIAGNOSE, "TAB_TITLE_INPUT", "����"},
	{SECT_DIALOG_DIAGNOSE, "TAB_TITLE_OUTPUT", "���"},
	{SECT_DIALOG_DIAGNOSE, "TAB_TITLE_OTHERS", "ϵͳ����"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_UPGRADE", "���¹̼�"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_REBOOT", "����������"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADE", "ѡ��A9Rom.bin����"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_REBOOT", "��������ر����"},
	{SECT_DIALOG_DIAGNOSE, "CONFIRM_UPGRADE", "ȷʵҪ������"},
	{SECT_DIALOG_DIAGNOSE, "CONFIRM_REBOOT", "ȷʵҪ������"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADING", "�����У�����ϵ�"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADED", "������ϣ���������"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADE_FAIL", "����ʧ��"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_HINT_UPGRADE_FAIL2", "����ͣ��״̬������"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_USB_TO_DISCONNECT", "�ر�USB"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_USB_STATUS_ON", "USB�ѿ���"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_USB_TO_CONNECT", "����USB"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_USB_STATUS_OFF", "USB�ѹر�"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_FTP_TO_DISCONNECT", "�ر�FTP"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_FTP_STATUS_ON", "FTP�ѿ���"},
	{SECT_DIALOG_DIAGNOSE, "OPERATION_FTP_TO_CONNECT", "����FTP"},
	{SECT_DIALOG_DIAGNOSE, "STATIC_TEXT_FTP_STATUS_OFF", "FTP�ѹر�"},

	// ���ܹ����ܶԻ���
	{SECT_DIALOG_DOGDECRYPT, "UNLOCK_STATUS_FREE", "�ѽ���"},
	{SECT_DIALOG_DOGDECRYPT, "UNLOCK_STATUS_LOCKED", "������"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_NO_RESTRICTED", "������"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_UNLOCK_DONE", "лл���ѽ�������ʹ��ʱ��"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_UNLOCKED_COUNT", "�ѽ�������"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_UNLOCKED_DAYS", "��ʹ������"},
	{SECT_DIALOG_DOGDECRYPT, "HINT_WRITE_ERR", "д����ܹ�ʧ��"},
	{SECT_DIALOG_DOGDECRYPT, "TITLE", "������ܹ�"},

	// ���ܹ�����
	{SECT_DIALOG_DOGENCRYPT, "HINT_NO_RESTRICTED", "���ü��ܹ�"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_ENABLE_DOG1", "���ü��ܹ�1(����)"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_ENABLE_DOG2", "���ü��ܹ�2(����)"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_NO_EMPTY_PWD", "���벻��Ϊ��"},
	{SECT_DIALOG_DOGENCRYPT, "CONFIRM_RESET", "ȷ�����ü��ܹ���"},
	{SECT_DIALOG_DOGENCRYPT, "RESET_FAIL", "��ʼ��ʧ��"},
	{SECT_DIALOG_DOGENCRYPT, "HINT_WRITE_ERR", "д����ܹ�ʧ��"},
	{SECT_DIALOG_DOGENCRYPT, "TITLE", "���ܹ�����"},
	
	// ������ϵ��ʽͼƬ
	{SECT_DIALOG_MANUFACTURE, "TITLE", "�豸����"},
	{SECT_DIALOG_MANUFACTURE, "FILE_NOT_FOUND", "�Ҳ����ļ�"},

	// ���ļ��Ի���
	{SECT_DIALOG_OPENFILE, "HEADER_FILENAME", "�ļ���"},
	{SECT_DIALOG_OPENFILE, "HEADER_SIZE", "��С"},
	{SECT_DIALOG_OPENFILE, "HEADER_DATE", "�޸�����"},
	{SECT_DIALOG_OPENFILE, "HEADER_TIME", "�޸�ʱ��"},
	{SECT_DIALOG_OPENFILE, "READ_FILELIST_ERR", "modbus��ȡ�ļ��б�ʧ��"},
	{SECT_DIALOG_OPENFILE, "OPEN_NOT_THJ_ERR", "�޷����ļ���"},
	{SECT_DIALOG_OPENFILE, "HINT_SRC_FILENAME", "Դ�ļ�Ϊ"},
	{SECT_DIALOG_OPENFILE, "HINT_DUPLICATE_SRC_DST", "Ŀ���ļ�����Դ�ļ���ͬ"},
	{SECT_DIALOG_OPENFILE, "CONFIRM_DELETE", "ȷʵ��Ҫɾ����"},
	{SECT_DIALOG_OPENFILE, "HINT_INPUT_FILENAME", "�������ļ���"},
	{SECT_DIALOG_OPENFILE, "HINT_INVALID_LENGTH", "�ļ������Ȳ��Ϸ�"},
	{SECT_DIALOG_OPENFILE, "CONFIRM_OVERWRITE", "�Ѵ��ڣ�ȷʵ��Ҫ������"},
	{SECT_DIALOG_OPENFILE, "TITLE", "�ļ�"},

	// ����IP
	{SECT_DIALOG_SET1600EIP, "ERR_INVALID_MAC", "MAC��ַ��ʽ���Ϸ�"},
	{SECT_DIALOG_SETTINGIP, "TITLE", "IP����"},
	{SECT_DIALOG_SETTINGIP, "TAB_DEST", "Modbus��������"},
	{SECT_DIALOG_SETTINGIP, "TAB_CONTROLLER_IP", "������IP"},

	// �ᶯͼӳ��
	{SECT_DIALOG_AXIS_IMAGE, "TITLE", "�趨�ᶯͼ��ʵ����"},

	// �趨IO
	{SECT_DIALOG_SETTINGIO, "TITLE", "IO����"},
	{SECT_DIALOG_SETTINGIO, "TAB_INPUT", "����"},
	{SECT_DIALOG_SETTINGIO, "TAB_OUTPUT", "���"},

	// �������ԶԻ���
	{SECT_DIALOG_SETLANG, "HINT", "�������ԣ�������Ч"},
	{SECT_DIALOG_SETLANG, "TITLE", "��������"},
	{SECT_DIALOG_SETLANG, LANGUAGE_STR(IDC_BUTTON_SETLANG_GENERATE), "����ģ��"},
	{SECT_DIALOG_SETLANG, LANGUAGE_STR(IDC_BUTTON_SETLANG_IMPORT), "��������"},
	{SECT_DIALOG_SETLANG, LANGUAGE_STR(IDC_BUTTON_SETLANG_OPENFILES), "�������"},

	// ����ϵͳ����
	{SECT_DIALOG_SETTINGS, "TITLE", "ϵͳ����"},

	// �����������
	{SECT_DIALOG_SETTINGSOFTWARE, "TITLE", "�������"},

	// �����Ի���
	{SECT_DIALOG_STARTUP, "ERR_EXPIRE_SOON", "ʹ�����޻�ʣ1�죬����ϵ�豸����"},
	{SECT_DIALOG_STARTUP, "ERR_WRONG_COM", "�ô��ں���Ч"},
	{SECT_DIALOG_STARTUP, "WS_READ_REG_STATUS", "����ϵͳע��״̬"},
	{SECT_DIALOG_STARTUP, "WS_READ_REG_STATUS_FAIL", "ϵͳע��ʧ�ܣ����޷������ӹ�����������룬��������"},
	{SECT_DIALOG_STARTUP, "WS_READ_DOG", "��֤���ܹ�״̬"},
	{SECT_DIALOG_STARTUP, "WS_READ_IO_CONFIG_IN", "����IO�˿�����"},
	{SECT_DIALOG_STARTUP, "WS_READ_SETTINGS", "����ϵͳ����"},
	{SECT_DIALOG_STARTUP, "WS_INIT_PARAM_PREFIXES", "��ʼ����������"},
	{SECT_DIALOG_STARTUP, "WS_READ_SERVO_SET_RESULT", "�����ŷ�������"},
	{SECT_DIALOG_STARTUP, "WS_READ_SERVO_SET_RESULT_FAIL", "�ŷ���������ʼ��ʧ�ܣ���������������ú������������ŷ����ߣ�Ȼ���������"},
	{SECT_DIALOG_STARTUP, "WS_EXITING", "�����˳����"},
	{SECT_DIALOG_STARTUP, "WS_ABORTING", "�ж�����"},
	{SECT_DIALOG_STARTUP, "TITLE", "������"},

	// �û�����Ի���
	{SECT_DIALOG_USERS, "KEEP_UNCHANGED", "���ֲ���"},
	{SECT_DIALOG_USERS, "CUR_PWD", "��ǰ����"},
	{SECT_DIALOG_USERS, "NEW_PWD", "������"},
	{SECT_DIALOG_USERS, "INVALID_INPUT", "���뷶Χ���Ϸ���������0-99999999����"},
	{SECT_DIALOG_USERS, "TITLE", "Ȩ�����û�"},

	// ���������Ի�����
	{SECT_LIST_AXIS_RENAME, "HEADER_INDEX", "�����"},
	{SECT_LIST_AXIS_RENAME, "HEADER_NAME_OLD", "ԭ����"},
	{SECT_LIST_AXIS_RENAME, "HEADER_NAME_NEW", "������"},
	{SECT_LIST_AXIS_RENAME, "HEADER_NAME_SWITCH", "�Ὺ��"},

	// ���ܹ����ܶԻ�����
	{SECT_LIST_DOGPASSWORD, "HEADER_INDEX", "�ڼ���"},
	{SECT_LIST_DOGPASSWORD, "HEADER_PASSWORD", "����"},
	{SECT_LIST_DOGPASSWORD, "HEADER_DAYS", "����"},

	// ģ�����ɶԻ�����
	{SECT_LIST_MODEL, "HEADER_INDEX", "���"},
	{SECT_LIST_MODEL, "HEADER_NAME", "����"},
	{SECT_LIST_MODEL, "HEADER_VALUE", "ֵ"},
	{SECT_LIST_MODEL, "HEADER_HINT", "˵��"},

	// ����IO�Ի�����
	{SECT_LIST_SETTINGIO, "HEADER_INDEX", "���"},
	{SECT_LIST_SETTINGIO, "HEADER_NAME", "����"},
	{SECT_LIST_SETTINGIO, "HEADER_MAP", "ӳ��IO"},
	{SECT_LIST_SETTINGIO, "HEADER_LOGIC", "��Ч��ƽ"},

	// ϵͳ������
	{SECT_LIST_SETTINGS, "HEADER_INDEX", "���"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_NAME", "����"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_VALUE", "ֵ"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_RANGE", "��Χ"},
	{SECT_LIST_SETTINGS, "HEADER_PARAM_USER", "�޸�Ȩ"},
	{SECT_LIST_SETTINGS, "PERMISSION_READ_ONLY", "ֻ��"},

	// �̵�ָ��༭������Ϣ
	{SECT_ERRORS_TEACHING_1, "ERR_DELETING_S", "����: ��һ��Sָ���ɾ��"},
	{SECT_ERRORS_TEACHING_1, "ERR_CONFIRM_DEL_ALL", "�ò������ܳ�����ȷʵɾ�������У�"},
	{SECT_ERRORS_TEACHING_1, "ERR_UNDO_FAIL", "�����޷�������������޸�"},
	{SECT_ERRORS_TEACHING_1, "ERR_REDO_FAIL", "�����޷��ָ���������޸�"},
	{SECT_ERRORS_TEACHING_1, "ERR_UNKNOWN_CMD", "δ֪�Ľ̵�ָ��"},
	{SECT_ERRORS_TEACHING_1, "ERR_MOVE_SAME_LINE_S_PLUS", "����\"+\",\"-\",\"S\"ָ���̽�벻��ͬʱ������MOVEָ����"},
	{SECT_ERRORS_TEACHING_1, "HINT_SINGLE", "��Ȧģʽ�����뷶Χ"},
	{SECT_ERRORS_TEACHING_1, "HINT_MULTI", "��Ȧģʽ�����뷶Χ-100~100(Ȧ)����λС����"},
	{SECT_ERRORS_TEACHING_1, "HINT_SCREW", "˿��ģʽ�����뷶Χ-999999~1999999"},
	{SECT_ERRORS_TEACHING_1, "HINT_FEED", "����ģʽ�����뷶Χ-999999~1999999"},
	{SECT_ERRORS_TEACHING_1, "HINT_CMD_TYPE", "M:�˶�, E:����, S:�ٶ�, J/G:��ת, L:ѭ��, N:��־"},
	{SECT_ERRORS_TEACHING_1, "HINT_SPEED", "�ٶȱ��������뷶Χ0.01~5.0"},
	{SECT_ERRORS_TEACHING_1, "HINT_PROBE", "̽��(1~4)������(5~24)���쳤(255)"},
	{SECT_ERRORS_TEACHING_1, "HINT_CYLINDER", "���ף�0�ر�����,���Źرյ���,���Ÿ�����������"},
	{SECT_ERRORS_TEACHING_1, "HINT_DELAY", "��ʱ�����뷶Χ0.01~500����λΪ��"},

	// ָ��༭��Ԫ����������Ϣ
	{SECT_ERRORS_TEACHING_2, "ERR_EXCEED_FEEDDING", "�����������ٶ��趨�����������ֵ"},
	{SECT_ERRORS_TEACHING_2, "ERR_SPEED_NEG", "�����ٶ��趨С��0"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_SPEED_COLUNM", "�����ٶ�ָ�����У�����ֻ�ܼ��뵽��������"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_FEED", "�������߳��ȺϷ���ΧΪ-999999~1999999"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_SINGLE", "���󣺵�Ȧģʽ�Ϸ���ΧΪ"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_MULTI_1", "���󣺶�ȦģʽС��������ΧΪ0.3600"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_MULTI_2", "���󣺶�ȦģʽȦ����Ч��ΧΪ-100~100"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RUNMODE", "���󣺲�֧�ֵ�������ģʽ"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_SPEED", "�����ٶȱ�����Ч��ΧΪ0.01~5.0"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_PROBE", "����̽����Ч��ΧΪ1~255"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_CYLINDER", "�������׺Ϸ����뷶Χ0~20"},
	{SECT_ERRORS_TEACHING_2, "ERR_NOT_NUMBER", "�����������ݲ�������"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_RANGE_JUMP", "������ת�����Ϸ���Χ1~200(MAXLINE)"},
	{SECT_ERRORS_TEACHING_2, "ERR_INVALID_JUMP_COLUMN", "������תָ�����У�����ֻ�ܼ��뵽��һ��"},
	{SECT_ERRORS_TEACHING_2, "ERR_END_NOT_EMPTY", "����Eָ�����У��������ݱ���Ϊ��"},

	// ָ��༭��
	{SECT_LIST_TEACH, "HEADER_CMD", "ָ��"},
	{SECT_LIST_TEACH, "HEADER_SPEED", "�ٱ�"},
	{SECT_LIST_TEACH, "HEADER_PROBE", "̽��"},
	{SECT_LIST_TEACH, "HEADER_CYLINDER", "����"},
	{SECT_LIST_TEACH, "HEADER_DELAY", "��ʱ"},
	{SECT_LIST_TEACH, "HEADER_END_ANGLE", "������"},
	{SECT_LIST_TEACH, "HEADER_BEGIN_ANGLE", "��ʼ��"},

	// ����
	{SECT_USER_NAMES,"HINT_CURRENT_USER","��ǰ�û���"},
	{SECT_USER_NAMES,"HINT_PLEASE_CHANGE_USER","Ҫʹ�øù��ܣ�������Ϊ"},

	{-1} // end
};

// ����IDӳ��ļ�ֵ��
const CLanguage::LANG_MAPKEY_VAL_T CLanguage::DEFAULT_MAPPING_TEXTS[]={
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_YEAR), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_MONTH), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_DAY), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_HOUR), "ʱ"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_MINUTE), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TIME_SECOND), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_RESULT_SUCCESS), "�ɹ�"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_RESULT_FAIL), "ʧ��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_RESULT_ERROR), "����"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_INVALID_RANGE), "���뷶Χ������"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_INVALID_PASSWORD), "��Ч������"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_MODBUS_BUSY), "ͨѶ�У����Ժ�����"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_MODBUS_FAIL), "ͨѶʧ�ܣ��Ƿ����ԣ�"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_MODBUS_FAIL_LOADWF), "ͨѶʧ��(�����ļ�)���Ƿ����ԣ�"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_BUTTON_OK), "ȷ��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_BUTTON_CANCEL), "ȡ��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_BUTTON_SAVE), "����"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_USER_MINIMAL_LEVEL), "�������Ȩ��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_USER_CURRENT_LEVEL), "��ǰ�û�"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_ENABLED), "����"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_DISABLED), "����"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_ON), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_SWITCH_OFF), "��"},
	{CLanguage::SECT_COMMON_TEXTS, LANGUAGE_MAP_ID(LANG_TITLE_HINT), "��ʾ"},

	// ����
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_GUEST), "����"},
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_USER), "����Ա"},
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_PROGRAMER), "����Ա"},
	{CLanguage::SECT_USER_NAMES, LANGUAGE_MAP_ID(REG_SUPER), "�����û�"},

	{0,0,std::string()} // end
};

CLanguage::CLanguage(std::string filename, bool isReadOnly)
{
	iniLang=new CIniLanguage(filename, isReadOnly);

	_initSectionArray(); // ��ʼ��Section��������
	_initDiaglogTexts(); // ��ʼ���Ի�������
	_initMapTexts(); // ��ʼ��ӳ��ID������
	_initAlarmMsg(); // ��ʼ��������Ϣ
	_initParamHints(); // ��ʼ��������ʾ
	_initIONames(); // ��ʼ��IO����
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
	while(std::string() != (lang_array+i)->id_str){//Ϊ����ָֹͣ��
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

			// ��������Խ��
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

	// ���ⱨ���������ǰ׺��
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
			copy_str(Alarm[i].name, "�������λ", 100);
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
			copy_str(Alarm[i].name, "�������λ", 100);
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
			copy_str(Alarm[i].name, "��Ӳ����λ����", 100);
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
			copy_str(Alarm[i].name, "��Ӳ����λ����", 100);
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
			copy_str(Alarm[i].name, "�ŷ�����", 100);
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
			copy_str(Alarm[i].name, "����ʧ��", 100);
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
			copy_str(Alarm[i].name, "ԭ��ƫ�����óɹ�", 100);
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
		// ����
		if(DT_TITLE == ParaRegTab[i].DataType){
			ss_id.str(std::string()); // flush all data
			sprintf_s(numberStr,10,"%02d",title_index);
			ss_id << "TITLE_" << numberStr;

			map_param_titles[title_index]=getString(ss_id.str(),CLanguage::SECT_PARAM_NAMES,ParaRegTab[i].pDescText);
			title_index++;

			continue;
		}

		// �Ǳ���
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

	if(map_alarm_num_idx_with_axis.find(alarmNumber) == map_alarm_num_idx_with_axis.end()) // ���޼�ֵ
		text=getStringById(alarmNumber,CLanguage::SECT_WARNING_MESSAGE);
	else
		text=CharsToCString(g_axisName[map_alarm_num_idx_with_axis[alarmNumber]]) + getStringById(alarmNumber,CLanguage::SECT_WARNING_MESSAGE);

	return (msg_AlarmNum+text);
}

CString CLanguage::getParamName(int index)
{
	if(map_param_names.find(index) == map_param_names.end()) // ���޼�ֵ
		return _T("?");

	return map_param_names[index];
}

CString CLanguage::getParamDescription(int index)
{
	if(map_param_descriptions.find(index) == map_param_descriptions.end()) // ���޼�ֵ
		return _T("?");

	return map_param_descriptions[index];
}

CString CLanguage::getParamTitle(int title_index)
{
	if(map_param_titles.find(title_index) == map_param_titles.end()){ // ���޼�ֵ
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

	for(i=0;i<MAX_INPUT_NUM;++i){ // ��Ͻ����IN����
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

	for(i=0;i<MAX_OUTPUT_NUM;++i){ // ��Ͻ����OUT����
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

		if(index >= X_SERVO_ALARM && index <= C_SERVO_ALARM) // 6�������� ����
			str=CString(g_axisName[(index-X_SERVO_ALARM)%16]) + str;
		else if(index >= X_HOME && index <= E1_M_LIMIT) // ԭ�㣬��λ+- ����
			str=CString(g_axisName[(index-X_HOME)%16]) + str;
	}else{
		ss_io_param << "OUT_" << formattedStr;
		str=getString(ss_io_param.str(),SECT_IO_NAMES_OUT);

		if(index >= OUT_X_SERVO_CLEAR && index <= OUT_C_SERVO_CLEAR) // ������ ������λ
			str=CString(g_axisName[(index-OUT_X_SERVO_CLEAR)%16]) + str;
	}

	if(CString() == str)
		return _T("???");

	return str;
}