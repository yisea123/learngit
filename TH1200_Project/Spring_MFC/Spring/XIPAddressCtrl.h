// XIPAddressCtrl.h  Version 1.0
//
// Author: Hans Dietrich
//         hdietrich@gmail.com
//
// This software is released into the public domain.  You are free to use it
// in any way you like, except that you may not sell this source code.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this software
// may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef  XIPADDRESSCTRL_H 
#define  XIPADDRESSCTRL_H 

class CXIPAddressCtrl : public CIPAddressCtrl
{
	DECLARE_DYNAMIC(CXIPAddressCtrl)

// Constructors
public:
	CXIPAddressCtrl() {};

// Attributes
public:
	int GetAddress(CString& strAddress);

	int GetAddress(BYTE& nField0, BYTE& nField1, BYTE& nField2, BYTE& nField3)
	{
		return CIPAddressCtrl::GetAddress(nField0, nField1, nField2, nField3);
	}

	int GetAddress(DWORD& dwAddress)
	{
		return CIPAddressCtrl::GetAddress(dwAddress);
	}

	void SetAddress(LPCTSTR lpszAddress);

	void SetAddress(BYTE nField0, BYTE nField1, BYTE nField2, BYTE nField3)
	{
		CIPAddressCtrl::SetAddress(nField0, nField1, nField2, nField3);
	}

	void SetAddress(DWORD dwAddress)
	{
		CIPAddressCtrl::SetAddress(dwAddress);
	}


// Operations
public:
	CString	ReadProfileAddress(LPCTSTR lpszSection,
							   LPCTSTR lpszEntry,
							   LPCTSTR lpszDefault = _T("0.0.0.0"));

	void	WriteProfileAddress(LPCTSTR lpszSection, 
								LPCTSTR lpszEntry);

// Implementation
public:
	CString	ByteAddressToString(BYTE nField0, 
								BYTE nField1, 
								BYTE nField2, 
								BYTE nField3);

	void	StringAddressToByte(LPCTSTR lpszAddress,
								BYTE& nField0, 
								BYTE& nField1, 
								BYTE& nField2, 
								BYTE& nField3);
};

#endif // XIPADDRESSCTRL_H 
