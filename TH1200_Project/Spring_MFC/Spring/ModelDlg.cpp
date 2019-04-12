// ModelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Spring.h"
#include "ModelDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "Data.h"
#include "modbusAddress.h"


// CModelDlg dialog

IMPLEMENT_DYNAMIC(CModelDlg, CDialogEx)

CModelDlg::CModelDlg(int default_index ,CFont* font, CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelDlg::IDD, pParent)
{
	default_model_index=default_index;
	waitingDlg=nullptr;
	isGenerateOk=false;
	m_pfont=font;
}

CModelDlg::~CModelDlg()
{
	delete_pointer(waitingDlg);
}

void CModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_COMBO1, m_cb_models);
	DDX_Control(pDX, IDC_STATIC_MODELDLG_PIC, m_picture);
}


BEGIN_MESSAGE_MAP(CModelDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, onSelectChange)
	ON_MESSAGE(WM_MODELDLG_QUERY_FAIL, queryFail)
	ON_MESSAGE(WM_MODELDLG_DISMISS, dismissWaitingDlg)
END_MESSAGE_MAP()


BOOL CModelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	setCurrentDialogFont(this,m_pfont);

	m_list.init();

	size_t model_count=g_modelArray->getCount();

	for(size_t i=0;i<model_count;++i){
		m_cb_models.InsertString(static_cast<int>(i),StringToCString(g_modelArray->get(i)->name));
	}

	m_cb_models.SetCurSel(default_model_index);
	reloadModel(g_modelArray->get(static_cast<size_t>(default_model_index)));

	// 线程
	queryTask_param.context=this;
	queryTask_param.isNeedAsynchronousKill=false;
	queryTask_param.threadLoopTaskRun=&CModelDlg::queryTask;

	return TRUE;
}

void CModelDlg::reloadModel(CModelArray::MODEL_T* m)
{
	m_list.DeleteAllItems();
	m_list.setModelParam(m);

	size_t param_count=m->params.size();

	// TODO: name show in dialog

	m_picture.setPicture(m->picture_id);

	CString str;
	for(size_t i=0;i<param_count;++i){
		str.Format(_T("%u"),i+1);
		m_list.InsertItem(i,str);

		str=StringToCString(m->params[i].description);
		m_list.SetItemText(i,1,str);

		str.Format(_T("%.10g"),m->params[i].value);
		m_list.SetItemText(i,2,str);

		str=StringToCString(m->params[i].range_text);
		m_list.SetItemText(i,3,str);
	}

}

void CModelDlg::onSelectChange()
{
	int selected=m_cb_models.GetCurSel();

	if(selected<0 || selected>=m_cb_models.GetCount())
		return;

	reloadModel(g_modelArray->get(static_cast<size_t>(selected)));
}

bool CModelDlg::writeTo1600E(bool isExecute)
{
	int selected=m_cb_models.GetCurSel();

	if(selected<0 || selected>=m_cb_models.GetCount())
		return false;

	CModelArray::MODEL_T* m=g_modelArray->get(static_cast<size_t>(selected));
	
	TSpringCraftParam param_to_1600e;
	memset(&param_to_1600e,0,sizeof(param_to_1600e));

	for(size_t i=0;i<m->params.size();++i){
		param_to_1600e.para[i]=static_cast<float>(m->params[i].value);
	}

	if(isExecute)
		param_to_1600e.iExcute=1;

	// 不同模型的地址
	unsigned short addr;
	switch(selected){
	case 0: addr=MODBUS_ADDR_MODEL_1;break;
	case 1: addr=MODBUS_ADDR_MODEL_2;break;
	case 2: addr=MODBUS_ADDR_MODEL_3;break;
	case 3: addr=MODBUS_ADDR_MODEL_4;break;
	default:return false;break;
	}

	return readWriteModbus(&param_to_1600e,sizeof(param_to_1600e),addr,false);
}

void CModelDlg::OnCancel()
{
	if(false==writeTo1600E(false))
		MessageBox(_T("写入失败"));

	CDialogEx::OnCancel();
}

void CModelDlg::OnOK()
{
	if (MessageBox(_T("确实要覆盖当前文件并生成模型吗？"), g_lang->getCommonText(CLanguage::LANG_TITLE_HINT), MB_YESNO | MB_ICONWARNING) == IDNO){
		return;
	}
	
	if(false==writeTo1600E(true)){
		MessageBox(_T("写入失败"));
		return;
	}

	if(nullptr==waitingDlg){
		CString loading_hint=_T("生成模型中，请稍候");
		waitingDlg=new CWaitingDlg(loading_hint);
	}

	threadLoopTask_start(&queryTask_param);

	waitingDlg->DoModal(); // 阻塞在这里，对话框通过PostMessage关闭

	if(true == isGenerateOk)
		CDialogEx::OnOK();
}

UINT CModelDlg::queryTask(LPVOID lpPara)
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);
	CModelDlg* p=static_cast<CModelDlg*>(param->context);
	unsigned short isFinish;

	while(1){
		Sleep(500);

		if(false == readWriteModbus(&isFinish,sizeof(isFinish),MODBUS_ADDR_MODEL_FINISH)){
			// 通讯失败
			p->PostMessage(WM_MODELDLG_QUERY_FAIL);
			break;
		}

		if(1 == isFinish){
			p->isGenerateOk=true; // 标记完成了
			p->PostMessage(WM_MODELDLG_DISMISS);
			break;
		}
	}

	return 0;
}

LRESULT CModelDlg::queryFail(WPARAM wpD, LPARAM lpD)
{
	if(MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL), g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR), MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
		threadLoopTask_start(&queryTask_param);
	}else{
		PostMessage(WM_MODELDLG_DISMISS);

		g_bIsModbusFail=true;
		g_mbq->query_abort();
	}

	return LRESULT();
}

LRESULT CModelDlg::dismissWaitingDlg(WPARAM wpD, LPARAM lpD)
{
	if(nullptr != waitingDlg)
		waitingDlg->SendMessage(WM_WAITINGDLG_DISMISS);

	return LRESULT();
}
