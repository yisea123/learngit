/*
 * CutWireDlg.cpp
 *
 * 剪线对话框
 *
 * Created on: 2017年12月23日下午4:45:29
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "CutWireDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "Data.h"
#include "Modbus\ModbusQueue.h"
#include "modbusAddress.h"


// CCutWireDlg dialog

IMPLEMENT_DYNAMIC(CCutWireDlg, CDialogEx)

CCutWireDlg::CCutWireDlg(T_JXParam* p_jxparam, CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CCutWireDlg::IDD, pParent)
{
	m_jxparam=p_jxparam;
	m_pfont=font;

	// 输入范围
	if(0 == g_Sysparam.UnpackMode){
		range_angle[0]=-3600;
		range_angle[1]=3600;
	}else{
		range_angle[0]=-360;
		range_angle[1]=360;
	}

	range_speed[0]=1;
	range_speed[1]=5000;

	range_pos_songxian[0]=0;
	range_pos_songxian[1]=99999;
}

CCutWireDlg::~CCutWireDlg()
{
}

void CCutWireDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// 主刀
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_MAIN_ANGLE, m_jxparam->fCutAxisPos);
	DDV_MinMaxFloat(pDX, m_jxparam->fCutAxisPos, range_angle[0], range_angle[1]);
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_MAIN_SPEED, m_jxparam->iCutAxisSpeed);
	DDV_MinMaxFloat(pDX, m_jxparam->iCutAxisSpeed, range_speed[0], range_speed[1]);

	// 辅助刀
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_OPTIONAL_ANGLE, m_jxparam->fAuxToolPos);
	DDV_MinMaxFloat(pDX, m_jxparam->fAuxToolPos, range_angle[0], range_angle[1]);
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_OPTIONAL_SPEED, m_jxparam->iAuxToolvSpeed);
	DDV_MinMaxFloat(pDX, m_jxparam->iAuxToolvSpeed, range_speed[0], range_speed[1]);

	// 轴位置
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_ZHUANXIN_POS, m_jxparam->fRotaCoreAxisPos);
	DDV_MinMaxFloat(pDX, m_jxparam->fRotaCoreAxisPos, range_angle[0], range_angle[1]); // 转芯
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_SONGXIAN_POS, m_jxparam->fWireAxisPos);
	DDV_MinMaxFloat(pDX, m_jxparam->fWireAxisPos, range_pos_songxian[0], range_pos_songxian[1]); // 送线

	// 轴速度
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_ZHUANXIN_SPEED, m_jxparam->iRotaCoreAxisSpeed);
	DDV_MinMaxFloat(pDX, m_jxparam->iRotaCoreAxisSpeed, 1, 5000); // 转芯
	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_SONGXIAN_SPEED, m_jxparam->iWireAxisSpeed);
	DDV_MinMaxFloat(pDX, m_jxparam->iWireAxisSpeed, range_speed[0], range_speed[1]); // 送线

	DDX_Text(pDX, IDC_EDIT_CUT_WIRE_SISSOR_DELAY, m_jxparam->fCutTime);
	DDV_MinMaxFloat(pDX, m_jxparam->fCutTime, (float)0.001, (float)100);

	DDX_Control(pDX, IDC_COMBO_CUT_WIRE_MAIN, m_cb_main_knife);
	DDX_Control(pDX, IDC_COMBO_CUT_WIRE_OPTIONAL, m_cb_optional_knife);
}


BEGIN_MESSAGE_MAP(CCutWireDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CCutWireDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	refreshLanguageUI();

	setCurrentDialogFont(this,m_pfont);

	UpdateData(FALSE);

	showComboBox(&m_cb_main_knife,m_jxparam->iCutAxis);
	showComboBox(&m_cb_optional_knife,m_jxparam->iAuxTool);

	return TRUE;
}

void CCutWireDlg::showComboBox(CComboBox* cb, unsigned char defaultIndex)
{
	int axis_num;
	cb->InsertString(0,g_lang->getCommonText(CLanguage::LANG_SWITCH_DISABLED)); // 禁用

	CString axis_name;
	CString suffix_disabled=g_lang->getString("SUFFIX_DISABLED", CLanguage::SECT_DIALOG_CUTWIRE); // 已停用

	for(int i=1;i<=8;i++){
		axis_num=i+3; // 跳过前三个轴（送线，转心，转线）

		if(axis_num>g_currentAxisCount)
			break;

		axis_name=CharsToCString(g_axisName[axis_num-1]);

		if(0==g_Sysparam.AxisParam[axis_num-1].iAxisSwitch) // 禁用
			axis_name+=suffix_disabled;

		cb->InsertString(i,axis_name);
	}

	if(defaultIndex>=0 && defaultIndex<=8)
		cb->SetCurSel(defaultIndex);
	else
		cb->SetCurSel(0);
}

void CCutWireDlg::OnOK()
{
#define SHOW_DEBUG_PARAM(param) do{\
		debug_printf(#param); \
		debug_printf("=%f\n",(float)m_jxparam->param); \
	}while(0);

	UINT isUsedScissor=IsDlgButtonChecked(IDC_CHECK_CUT_WIRE_USE_SISSOR);
	
	m_jxparam->iHydraScissorsEna=static_cast<unsigned char>(isUsedScissor);
	m_jxparam->iCutAxis=m_cb_main_knife.GetCurSel();
	m_jxparam->iAuxTool =m_cb_optional_knife.GetCurSel();

	m_jxparam->iAuxToolEna=m_jxparam->iAuxTool; // 这是多余步骤。。。但是下位机需要它
	m_jxparam->iExecute=1; // 执行剪线

	if(m_jxparam->iCutAxis==m_jxparam->iAuxTool && m_jxparam->iCutAxis!=0){
		MessageBox(g_lang->getString("ERR_NO_SAME_AXIS", CLanguage::SECT_DIALOG_CUTWIRE)); // "辅助刀与主刀不能选择相同的轴"
		return;
	}
	
	if(FALSE==UpdateData(TRUE))
		return;

	debug_printf("\n\n");
	SHOW_DEBUG_PARAM(iCutAxis);
	SHOW_DEBUG_PARAM(iCutAxisSpeed);
	SHOW_DEBUG_PARAM(fCutAxisPos);
	SHOW_DEBUG_PARAM(iHydraScissorsEna);
	SHOW_DEBUG_PARAM(fCutTime);
	SHOW_DEBUG_PARAM(iAuxToolEna);
	SHOW_DEBUG_PARAM(iAuxTool);
	SHOW_DEBUG_PARAM(iAuxToolvSpeed);
	SHOW_DEBUG_PARAM(fAuxToolPos);
	SHOW_DEBUG_PARAM(fRotaCoreAxisPos);
	SHOW_DEBUG_PARAM(iRotaCoreAxisSpeed);
	SHOW_DEBUG_PARAM(fWireAxisPos);
	SHOW_DEBUG_PARAM(iWireAxisSpeed);
	SHOW_DEBUG_PARAM(iExecute);

	CDialogEx::OnOK();
#undef SHOW_DEBUG_PARAM
}

void CCutWireDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_HINT_MAIN), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_MAIN_AXIS), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_MAIN_ANGLE), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_MAIN_SPEED), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_HINT_OPT), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_OPT_AXIS), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_OPT_ANGLE), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_OPT_SPEED), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_HINT_SCISSOR), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_CHECK_CUT_WIRE_USE_SISSOR), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_DELAY), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_HINT_OTHERS), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_ZHUANXIN_ANGLE), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_ZHUANXIN_SPEED), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_SONGXIAN_POS), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDC_STATIC_CUTWIREDLG_SONGXIAN_SPEED), CLanguage::SECT_DIALOG_CUTWIRE},
		{LANGUAGE_MAP_ID(IDOK), CLanguage::SECT_DIALOG_CUTWIRE},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_CUTWIRE));

	// 显示范围
	auto updateRangeText = [](CCutWireDlg* context, int id, float minval, float maxval){
		CString str_prefix,str_suffix;
		context->GetDlgItem(id)->GetWindowText(str_prefix);
		str_suffix.Format(_T("(%d~%d)"),static_cast<int>(minval),static_cast<int>(maxval));
		context->GetDlgItem(id)->SetWindowText(str_prefix+str_suffix);
	};

	updateRangeText(this,IDC_STATIC_CUTWIREDLG_MAIN_ANGLE,range_angle[0],range_angle[1]);
	updateRangeText(this,IDC_STATIC_CUTWIREDLG_OPT_ANGLE,range_angle[0],range_angle[1]);
	updateRangeText(this,IDC_STATIC_CUTWIREDLG_ZHUANXIN_ANGLE,range_angle[0],range_angle[1]);
	updateRangeText(this,IDC_STATIC_CUTWIREDLG_MAIN_SPEED,range_speed[0],range_speed[1]);
	updateRangeText(this,IDC_STATIC_CUTWIREDLG_OPT_SPEED,range_speed[0],range_speed[1]);
	updateRangeText(this,IDC_STATIC_CUTWIREDLG_SONGXIAN_POS,range_pos_songxian[0],range_pos_songxian[1]);
}