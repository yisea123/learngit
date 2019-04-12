/*
 * StartupDlg.cpp
 *
 * �������ɣ��������������Ի���
 *
 * Created on: 2017��12��13������9:07:37
 * Author: lixingcong
 */

#include "stdafx.h"
#include "Spring.h"
#include "StartupDlg.h"
#include "afxdialogex.h"
#include "Modbus\ModbusQueue.h"
#include "modbusAddress.h"
#include "Data.h"
#include "DogDecryptDlg.h"

#include "public.h"

#include <map>

#define MAX_SERIAL_COM_COUNT 24 // ���ö�ٴ��ڽӿ���

// CStartupDlg dialog

IMPLEMENT_DYNAMIC(CStartupDlg, CDialogEx)

unsigned short CStartupDlg::isRegistered=FALSE;

CStartupDlg::CStartupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStartupDlg::IDD, pParent)
{
	memset(&startup_flag,0,sizeof(startup_flag));
	isDlgRunning=true;
	isStopped=false;
	isUseUdp=true;
	comIndex=0;
}

CStartupDlg::~CStartupDlg()
{
}

void CStartupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STARTUP_HINT, m_text);
	DDX_Control(pDX, IDC_PROGRESS_STARTUP, m_progressCtrl);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipAddr);
	DDX_Control(pDX, IDC_COMBO1, m_cb_serial_list);
	DDX_Text(pDX, IDC_EDIT_STARTUP_TIMEOUT, g_softParams->params.modbus_timeout);
	DDX_Text(pDX, IDC_EDIT_STARTUP_RETRY_TIMES, g_softParams->params.modbus_retry_times);
}


BEGIN_MESSAGE_MAP(CStartupDlg, CDialogEx)
	ON_MESSAGE(WM_STARTUPDLG_QUERY_END, queryEnd)
	ON_MESSAGE(WM_STARTUPDLG_UPDATE_HINT, updateHintMessage)
	ON_MESSAGE(WM_STARTUPDLG_UPDATE_MODBUS_NUM, updateModbusNumber)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_CONTINUE, &CStartupDlg::OnBnClickedButtonStartupContinue)
	ON_BN_CLICKED(IDC_BUTTON2, &CStartupDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_STOP, &CStartupDlg::OnBnClickedButtonStartupStop)
END_MESSAGE_MAP()


BOOL CStartupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	EnumerateSerialPorts();

	((CButton*)GetDlgItem(IDC_RADIO_CONN_TYPE_IP))->SetCheck(TRUE);

	setMarqueStatus(true);
	setConnTypeEditable(false);

	GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(FALSE); // ֱ���������ini��ſ���

	startup_flag.workstep=WS_LOAD_LOCAL_CONFIG;

	queryTask_param.context=this;
	queryTask_param.isNeedAsynchronousKill=false;
	queryTask_param.threadLoopTaskRun=&CStartupDlg::queryTask;
	threadLoopTask_start(&queryTask_param);

	return TRUE;
}

UINT CStartupDlg::queryTask(LPVOID lpPara)
{
	threadLoopTask_param_t* param=static_cast<threadLoopTask_param_t*>(lpPara);
	CStartupDlg* p=static_cast<CStartupDlg*>(param->context);

	modbusQ_push_t mbq;
	bool isQueryOk=true;

	unsigned short dummyVar; // д�뵽modbus������(dummy)��ʱ����
	unsigned short addr;
	TSpringCraftParam model_param;

	// ������ʾ��Ϣ
	p->PostMessage(WM_STARTUPDLG_UPDATE_HINT);

	switch(p->startup_flag.workstep){
	case WS_LOAD_LOCAL_CONFIG:
		isQueryOk=g_softParams->loadAllParamsFromIni();
		if(true==isQueryOk){
			g_lang = new CLanguage(getIniFileName("Languages",g_softParams->params.languageFile),g_softParams->params.languageFile == std::string("default"));

			p->m_ipAddr.SetAddress(CharsToCString(g_softParams->params.destIPAddr));
			p->PostMessage(WM_STARTUPDLG_UPDATE_MODBUS_NUM);

			g_logger->setEnable(g_softParams->params.isEnableLogger);

			// ����ģ��
			g_modelArray->loadDefault();
		}
		break;

	case WS_START_MODBUS:
		if(true == p->isUseUdp){
			isQueryOk=g_mbq->init(g_softParams->params.destIPAddr, // ʹ���ض���IP����
				g_softParams->params.modbus_timeout,
				g_softParams->params.modbus_retry_times,
				MODBUS_UDP_ID);
		}else{
			isQueryOk=g_mbq->init(p->comIndex, // ʹ���ض��Ĵ�������
				g_softParams->params.modbus_timeout,
				g_softParams->params.modbus_retry_times,
				MODBUS_UDP_ID);
		}
		break;

	case WS_READ_REG_STATUS:
		isQueryOk=readWriteModbus(&isRegistered,sizeof(isRegistered),MODBUS_ADDR_REGISTER_STATUS);
		break;

	case WS_READ_REG_STATUS_FAIL:
		isQueryOk=true;
		break;

	case WS_READ_DOG:
		isQueryOk=g_dog->readDog();
		break;

	case WS_WRITE_DOG_PROGRESS:
		//debug_printf("write 0 to dog progress...\n");
		dummyVar=0;
		isQueryOk=readWriteModbus(&dummyVar,sizeof(dummyVar),MODBUS_ADDR_DOG_PROGRESS,false);
		break;

	case WS_READ_IO_CONFIG_IN:
		isQueryOk=readWriteModbus(&g_InConfig[0],sizeof(IOConfig)*MAX_INPUT_NUM,MODBUS_ADDR_IOCONFIG_IN);
		break;

	case WS_READ_IO_CONFIG_OUT:
		isQueryOk=readWriteModbus(&g_OutConfig[0],sizeof(IOConfig)*MAX_OUTPUT_NUM,MODBUS_ADDR_IOCONFIG_OUT);
		break;

	case WS_READ_SETTINGS:
		isQueryOk=g_paramTable->queryAllParams();//����λ�� ȡ������
		break;

	case WS_READ_MODELS:
		for(size_t i=0;i<g_modelArray->getCount();++i){
			switch(i){
			case 0: addr=MODBUS_ADDR_MODEL_1;break;
			case 1: addr=MODBUS_ADDR_MODEL_2;break;
			case 2: addr=MODBUS_ADDR_MODEL_3;break;
			case 3: addr=MODBUS_ADDR_MODEL_4;break;
			default:isQueryOk=false;return 0;break;
			}

			if(true == (isQueryOk=readWriteModbus(&model_param,sizeof(model_param),addr))){
				g_modelArray->set(i, &model_param.para[0]);
			}else{
				break; // break for loop
			}
		}
		break;

	case WS_INIT_PARAM_PREFIXES:
		//g_paramTable->fillEmptyParams();
		isQueryOk=true;
		break;

	case WS_READ_1600E_IP:
		isQueryOk=readWriteModbus(&g_Sysparam.IP,sizeof(g_Sysparam.IP),MODBUS_ADDR_IPADDR);
		break;

	case WS_READ_USERS:
		if( readWriteModbus(&g_currentUser,sizeof(g_currentUser),MODBUS_ADDR_CURRENT_USER) &&
			readWriteModbus(&g_passwords[1],sizeof(g_passwords[1]),MODBUS_ADDR_PASSWORD_USER) &&
			readWriteModbus(&g_passwords[2],sizeof(g_passwords[2]),MODBUS_ADDR_PASSWORD_PROGRAMMER) &&
			readWriteModbus(&g_passwords[3],sizeof(g_passwords[3]),MODBUS_ADDR_PASSWORD_SUPER))
		{
			char userBuf[100];
			CString str=g_lang->getString("HINT_CURRENT_USER",CLanguage::SECT_USER_NAMES) +g_lang->getStringById(g_currentUser, CLanguage::SECT_USER_NAMES);
			CStringToChars(str,userBuf,sizeof(userBuf));
			g_logger->log(LoggerEx::INFO,userBuf);

			isQueryOk=true;
			// debug_printf("curUser=%u, p0=%u, p1=%u, p2=%u\n",g_currentUser,g_passwords[0],g_passwords[1],g_passwords[2]);
		}else{
			isQueryOk=false;
		}
		break;

	case WS_READ_OTHER_FLAGS:
		isQueryOk=readWriteModbus(&g_TeachKey,sizeof(g_TeachKey),MODBUS_ADDR_TEACH_KEY_COUNTER);
		break;

	case WS_READ_SERVO_SET_RESULT:
		Sleep(200); // 200ms���������û������ȴ�����Ƶ������modbus����
		isQueryOk=readWriteModbus(&p->startup_flag.returnVal,sizeof(p->startup_flag.returnVal),MODBUS_ADDR_MOTION_INIT_RETURN_VALUE);
		break;

	default:
		return 0;
		break;
	}

	if(false==p->isStopped)
		p->SendMessage(WM_STARTUPDLG_QUERY_END,(WPARAM)(isQueryOk?1:0));
	else{
		p->setConnTypeEditable(true);
		p->setMarqueStatus(false);
	}

	return 0;
}

LRESULT CStartupDlg::queryEnd(WPARAM wpD, LPARAM lpD)
{
	BOOL isQueryOk=static_cast<BOOL>(wpD);
	CDogDecryptDlg* dlg_dogdecrypt=nullptr;

	if(false==isDlgRunning)
		return LRESULT();

	if(0==isQueryOk){
		g_logger->log(LoggerEx::ERROR2,"�������棺Modbusͨ�Ŵ����Ƿ����ԣ�");

		if(MessageBox(g_lang->getCommonText(CLanguage::LANG_MODBUS_FAIL), g_lang->getCommonText(CLanguage::LANG_RESULT_ERROR), MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY){
			threadLoopTask_start(&queryTask_param);
		}else{
			setMarqueStatus(false);
			setConnTypeEditable(true);
			//OnCancel(); // �˳�
		}
	}else{// isQueryOk�ɹ�
		switch(startup_flag.workstep){
		case WS_LOAD_LOCAL_CONFIG:
			GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(TRUE);

			refreshLanguageUI();

			startup_flag.workstep=WS_START_MODBUS;
			break;
		case WS_START_MODBUS:
			startup_flag.workstep=WS_READ_REG_STATUS;
			break;
		case WS_READ_REG_STATUS:
			startup_flag.workstep=(TRUE == isRegistered)?WS_READ_DOG:WS_READ_REG_STATUS_FAIL;
			break;
		case WS_READ_REG_STATUS_FAIL:
			return LRESULT(); // ��ͣ��ѯ�߳�
			break;
		case WS_READ_DOG:
			if(1 == g_dog->getDogType()){
				// ���ܹ�1
				if(true==g_dog->isUnlocked()){ // ���ܹ�������
					startup_flag.workstep=WS_READ_IO_CONFIG_IN;
				}else if(g_dog->getRemainDays() == 1){ // ʹ������ֻʣ�����ˣ���ʾ�û�ץ��ʱ��ע��
					MessageBox(g_lang->getString("ERR_EXPIRE_SOON", CLanguage::SECT_DIALOG_STARTUP)); // "ʹ�����޻�ʣ1�죬����ϵ�豸����"));
					startup_flag.workstep=WS_READ_IO_CONFIG_IN;
				}else if(1 == g_dog->getDogProcessCode()){ // ������
					dlg_dogdecrypt=new CDogDecryptDlg;
					if(IDOK == dlg_dogdecrypt->DoModal()){ // ������ȷ
						startup_flag.workstep=WS_WRITE_DOG_PROGRESS;
					}else{
						delete_pointer(dlg_dogdecrypt);
						setMarqueStatus(false);
						setConnTypeEditable(true);
						return LRESULT();
					}
				}else{ // ��ʱû�й���(DogProcessΪ0)
					startup_flag.workstep=WS_READ_IO_CONFIG_IN;
				}
			}else{
				// ���ܹ�2
				switch(g_dog->getDogProcessCode()){
				case 0: // ������
					startup_flag.workstep=WS_READ_IO_CONFIG_IN;
					break;
				case 2: // �쵽����
					dlg_dogdecrypt=new CDogDecryptDlg;

					if(IDOK == dlg_dogdecrypt->DoModal()){ // �û������д�����뵽��λ��
						startup_flag.workstep=WS_READ_DOG;
					}else{ // �û�����˹ر�
						startup_flag.workstep=WS_READ_IO_CONFIG_IN;
					}
					break;
				case 1: // �ѹ���
				default:
					dlg_dogdecrypt=new CDogDecryptDlg;

					if(IDOK == dlg_dogdecrypt->DoModal()){ // �û������д�����뵽��λ��
						startup_flag.workstep=WS_READ_DOG;
					}else{ // �û�����˹ر�
						delete_pointer(dlg_dogdecrypt);
						setMarqueStatus(false);
						setConnTypeEditable(true);
						return LRESULT();
					}

					break;
				}
			}
			delete_pointer(dlg_dogdecrypt);
			break;
		case WS_WRITE_DOG_PROGRESS:
			startup_flag.workstep=WS_READ_IO_CONFIG_IN;
			break;
		case WS_READ_IO_CONFIG_IN:
			startup_flag.workstep=WS_READ_IO_CONFIG_OUT;
			break;
		case WS_READ_IO_CONFIG_OUT:
			startup_flag.workstep=WS_READ_SETTINGS;
			break;
		case WS_READ_SETTINGS:
			startup_flag.workstep=WS_READ_MODELS;
			break;
		case WS_READ_MODELS:
			startup_flag.workstep=WS_INIT_PARAM_PREFIXES;
			break;
		case WS_INIT_PARAM_PREFIXES:
			startup_flag.workstep=WS_READ_1600E_IP;
			break;
		case WS_READ_1600E_IP:
			startup_flag.workstep=WS_READ_USERS;
			break;
		case WS_READ_USERS:
			startup_flag.workstep=WS_READ_OTHER_FLAGS;
			break;
		case WS_READ_OTHER_FLAGS:
			startup_flag.workstep=WS_READ_SERVO_SET_RESULT;
			break;
		case WS_READ_SERVO_SET_RESULT:
			if(startup_flag.returnVal<0){
				// ������ȡ
			}else if(startup_flag.returnVal==0){
				g_logger->log(LoggerEx::INFO,"�����ŷ��������ɹ�");
				OnOK(); // �ɹ�
				return LRESULT();
			}else{
				g_logger->log(LoggerEx::INFO,"�����ŷ�������ʧ��");
				startup_flag.workstep=WS_READ_SERVO_SET_RESULT_FAIL;
			}
			break;
		default:
			return LRESULT();
			break;
		}

		threadLoopTask_start(&queryTask_param);
	}

	return LRESULT();
}

void CStartupDlg::OnBnClickedButtonStartupContinue()
{
	BYTE ip[4];

	if(WS_READ_SERVO_SET_RESULT_FAIL==startup_flag.workstep || WS_READ_DONE==startup_flag.workstep){
		OnOK();
	}else if(WS_READ_REG_STATUS_FAIL==startup_flag.workstep){
		isStopped=false;
		setMarqueStatus(true);

		startup_flag.workstep=WS_READ_DOG;
		threadLoopTask_start(&queryTask_param);
	}else{
		UpdateData(TRUE); // д��timeout��retry

		if(((CButton*)GetDlgItem(IDC_RADIO_CONN_TYPE_IP))->GetCheck()){
			m_ipAddr.GetAddress(ip[0],ip[1],ip[2],ip[3]);
			sprintf_s(g_softParams->params.destIPAddr,MAX_LEN_IO_NAME,"%u.%u.%u.%u",ip[0],ip[1],ip[2],ip[3]);

			isUseUdp=true;
		}else{
			int comboxIndex=m_cb_serial_list.GetCurSel();
			if(false == validateComboBoxSerial(comboxIndex,&comIndex)){
				MessageBox(g_lang->getString("ERR_WRONG_COM", CLanguage::SECT_DIALOG_STARTUP)); // "�ô��ں���Ч"));
				return;
			}

			isUseUdp=false;
		}

		isStopped=false;
		setMarqueStatus(true);
		setConnTypeEditable(false);

		startup_flag.workstep=WS_START_MODBUS;
		threadLoopTask_start(&queryTask_param);

		// ���µ�modbus������IP����ʱ�����ԣ�д�뵽�����ļ�
		if(nullptr!=g_softParams)
			g_softParams->saveAllParamsToIni();
	}
}

LRESULT CStartupDlg::updateHintMessage(WPARAM wpD, LPARAM lpD)
{
	CStatic* p_static=static_cast<CStatic*>(GetDlgItem(IDC_STATIC_STARTUP_HINT));

	switch(startup_flag.workstep){
	case WS_LOAD_LOCAL_CONFIG:
		p_static->SetWindowText(_T("���ر����������"));
		break;
	case WS_READ_REG_STATUS:
		p_static->SetWindowText(g_lang->getString("WS_READ_REG_STATUS", CLanguage::SECT_DIALOG_STARTUP)); // "��ȡϵͳע��״̬"));
		break;
	case WS_READ_REG_STATUS_FAIL:
		p_static->SetWindowText(g_lang->getString("WS_READ_REG_STATUS_FAIL", CLanguage::SECT_DIALOG_STARTUP)); // "ϵͳע��ʧ��"));
		setMarqueStatus(false);
		GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(FALSE);
		break;
	case WS_READ_DOG:
		p_static->SetWindowText(g_lang->getString("WS_READ_DOG", CLanguage::SECT_DIALOG_STARTUP)); // "��֤���ܹ�״̬"));
		GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(TRUE);
		break;
	case WS_READ_IO_CONFIG_IN:
		p_static->SetWindowText(g_lang->getString("WS_READ_IO_CONFIG_IN", CLanguage::SECT_DIALOG_STARTUP)); // "����IO�˿�����"));
		break;
	case WS_READ_SETTINGS:
		p_static->SetWindowText(g_lang->getString("WS_READ_SETTINGS", CLanguage::SECT_DIALOG_STARTUP)); // "����ϵͳ����"));
		break;
	case WS_INIT_PARAM_PREFIXES:
		p_static->SetWindowText(g_lang->getString("WS_INIT_PARAM_PREFIXES", CLanguage::SECT_DIALOG_STARTUP)); // "��ʼ����������"));
		break;
	case WS_READ_SERVO_SET_RESULT:
		p_static->SetWindowText(g_lang->getString("WS_READ_SERVO_SET_RESULT", CLanguage::SECT_DIALOG_STARTUP)); // "�����ŷ�������"));
		break;
	case WS_READ_SERVO_SET_RESULT_FAIL:
		p_static->SetWindowText(g_lang->getString("WS_READ_SERVO_SET_RESULT_FAIL", CLanguage::SECT_DIALOG_STARTUP)); // "�ŷ���������ʼ��ʧ�ܣ���������������ú������������ŷ����ߣ�Ȼ���������"));
		setMarqueStatus(false);
		GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(FALSE);
		break;
	case WS_EXITING:
		p_static->SetWindowText(g_lang->getString("WS_EXITING", CLanguage::SECT_DIALOG_STARTUP)); // "�����˳����"));
		break;
	case WS_ABORTING:
		p_static->SetWindowText(g_lang->getString("WS_ABORTING", CLanguage::SECT_DIALOG_STARTUP)); // "�ж�����"));
		break;
	default:break;
	}

	return LRESULT();
}

void CStartupDlg::OnCancel()
{
	debug_printf("CStartupDlg::onCancel\n");
	UpdateData(FALSE);
	isDlgRunning=false;

	g_mbq->query_abort();

	startup_flag.workstep=WS_EXITING;
	SendMessage(WM_STARTUPDLG_UPDATE_HINT);

	Sleep(1000); // �ȴ������߳̽���

	CDialogEx::OnCancel();
}

void CStartupDlg::setMarqueStatus(bool isRun)
{
	if(true==isRun){
		m_progressCtrl.ModifyStyle(0, PBS_MARQUEE);
		m_progressCtrl.SetMarquee(TRUE, 50);

		GetDlgItem(IDC_BUTTON_STARTUP_CONTINUE)->EnableWindow(FALSE);
	}else{
		m_progressCtrl.ModifyStyle(PBS_MARQUEE, 0);
		m_progressCtrl.SetMarquee(FALSE, 20);
		m_progressCtrl.SetPos(50);

		GetDlgItem(IDC_BUTTON_STARTUP_CONTINUE)->EnableWindow(TRUE);
	}
}

void CStartupDlg::setConnTypeEditable(bool isEditable)
{
	GetDlgItem(IDC_RADIO_CONN_TYPE_IP)->EnableWindow(isEditable);
	GetDlgItem(IDC_RADIO_CONN_TYPE_SERIAL)->EnableWindow(isEditable);
	GetDlgItem(IDC_BUTTON_REFRESH_SERIAL)->EnableWindow(isEditable);
	GetDlgItem(IDC_STATIC_STARTUP_1)->EnableWindow(isEditable);
	GetDlgItem(IDC_STATIC_STARTUP_2)->EnableWindow(isEditable);
	GetDlgItem(IDC_EDIT_STARTUP_TIMEOUT)->EnableWindow(isEditable);
	GetDlgItem(IDC_EDIT_STARTUP_RETRY_TIMES)->EnableWindow(isEditable);

	m_cb_serial_list.EnableWindow(isEditable);
	m_ipAddr.EnableWindow(isEditable);

	// ���߼�
	GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(!isEditable);
}

BOOL CStartupDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN ){
		if(!(GetKeyState(VK_CONTROL)& 0x8000)){
			switch(pMsg->wParam){
			case VK_ESCAPE: // �����˳����ͻس���
			case VK_RETURN:
				return TRUE;		    	// DO NOT process further
				break;

			default:
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CStartupDlg::EnumerateSerialPorts()
{
	CString ComFile,ComName;
	HANDLE h;

	m_cb_serial_list.ResetContent();

	for(int i=0; i<MAX_SERIAL_COM_COUNT; i++)
		validateComboBoxSerial(i,&i,true,false);

	int comboxBoxIndex=0;

	for(int i=0; i<MAX_SERIAL_COM_COUNT; i++) // checking ports from COM0 to COM255
	{
		ComFile.Format(_T("\\\\.\\COM%d"), i);

		h = ::CreateFile(ComFile, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

		if (h == INVALID_HANDLE_VALUE){
			if(GetLastError()==ERROR_ACCESS_DENIED){
				//TRACE(_T("BUSY: "));
				ComName.Format(_T("COM%d-BUSY"),i);
				m_cb_serial_list.AddString(ComName);

				validateComboBoxSerial(comboxBoxIndex++, &i, true, false);
			}


		}else{
			ComName.Format(_T("COM%d"),i);
			m_cb_serial_list.AddString(ComName);
			CloseHandle(h);

			validateComboBoxSerial(comboxBoxIndex++, &i, true, true);
		}
	}

	if(m_cb_serial_list.GetCount()>0)
		m_cb_serial_list.SetCurSel(0);
}

bool CStartupDlg::validateComboBoxSerial(int comboBoxIndex, int* serialComIndex, bool isModify, bool newStatus)
{
	static std::map<int,bool> set_isAvailabe;
	static std::map<int,int> set_comboxIndex;

	if(0> comboBoxIndex || comboBoxIndex >= MAX_SERIAL_COM_COUNT)
		return false;

	if(true == isModify){
		set_comboxIndex[comboBoxIndex]=*serialComIndex;
		set_isAvailabe[comboBoxIndex]=newStatus;
		return true;
	}

	*serialComIndex=set_comboxIndex[comboBoxIndex];
	return set_isAvailabe[comboBoxIndex];
}

void CStartupDlg::OnBnClickedButton2()
{
	EnumerateSerialPorts();
}

LRESULT CStartupDlg::updateModbusNumber(WPARAM wpD, LPARAM lpD)
{
	UpdateData(FALSE);
	return LRESULT();
}

void CStartupDlg::OnOK()
{
	UpdateData(TRUE);
	CDialogEx::OnOK();
}

void CStartupDlg::OnBnClickedButtonStartupStop()
{
	GetDlgItem(IDC_BUTTON_STARTUP_STOP)->EnableWindow(FALSE);
	startup_flag.workstep=WS_ABORTING;
	SendMessage(WM_STARTUPDLG_UPDATE_HINT);
	isStopped=true;
}

void CStartupDlg::refreshLanguageUI()
{
	CLanguage::LANG_ITEM_T dialog_items[]={
		{LANGUAGE_MAP_ID(IDC_BUTTON_REFRESH_SERIAL), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_BUTTON_STARTUP_CONTINUE), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_BUTTON_STARTUP_STOP), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_RADIO_CONN_TYPE_IP), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_RADIO_CONN_TYPE_SERIAL), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_STATIC_STARTUP_1), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_STATIC_STARTUP_2), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_STATIC_STARTUP_OPT1), CLanguage::SECT_DIALOG_STARTUP},
		{LANGUAGE_MAP_ID(IDC_STATIC_STARTUP_OPT2), CLanguage::SECT_DIALOG_STARTUP},
		{0,std::string()} // stop
	};

	g_lang->setDialogText(this,dialog_items);

	SetWindowText(g_lang->getString("TITLE", CLanguage::SECT_DIALOG_STARTUP)); // "������"));

}