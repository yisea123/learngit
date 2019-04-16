
// chatUDP_yibu.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CchatUDP_yibuApp:
// See chatUDP_yibu.cpp for the implementation of this class
//

class CchatUDP_yibuApp : public CWinApp
{
public:
	CchatUDP_yibuApp();

// Overrides
public:
	~CchatUDP_yibuApp();
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CchatUDP_yibuApp theApp;