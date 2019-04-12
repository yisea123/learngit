// XIPAddressCtrl.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XIPAddressCtrl.cpp implements an enhanced version of CIPAddressCtrl,
//     with string handling and profile read/write functions.
//
// History:
//     Version 1.0 - 2005 January 5
//     - Initial public release
//
// Public APIs:
//             NAME                            DESCRIPTION
//     ---------------------  -------------------------------------------------
//     GetAddress()           Retrieve the IP address from the control and 
//                            return it as a CString
//     SetAddress()           Write the dotted IP string address to the control
//     ByteAddressToString()  Convert the four byte fields to a dotted IP 
//                            address string
//     StringAddressToByte()  Convert a dotted IP address string to four byte 
//                            fields
//     ReadProfileAddress()   Retrieve the IP address from the profile and 
//                            write it to the control
//     WriteProfileAddress()  Retrieve the IP address from the control and 
//                            write it to the profile
//
// License:
//     This software is released into the public domain.  You are free to use 
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XIPAddressCtrl.h"
#include <malloc.h>		// for _alloca

IMPLEMENT_DYNAMIC(CXIPAddressCtrl, CIPAddressCtrl)

///////////////////////////////////////////////////////////////////////////////
//
// GetAddress()
//
// Purpose:     Retrieve the IP address from the control and return it as a 
//              CString.
//
// Parameters:  strAddress - [out] dotted IP address; e.g., "127.0.0.1"
//
// Returns:     int        - number of non-blank fields in the address
//
int CXIPAddressCtrl::GetAddress(CString& strAddress)
{
	BYTE nField0, nField1, nField2, nField3;
	
	int rc = CIPAddressCtrl::GetAddress(nField0, nField1, nField2, nField3);

	strAddress = ByteAddressToString(nField0, nField1, nField2, nField3);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// SetAddress()
//
// Purpose:     Write the dotted IP string address to the control.
//
// Parameters:  lpszAddress - [in] dotted IP address; e.g., "127.0.0.1"
//
// Returns:     None
//
void CXIPAddressCtrl::SetAddress(LPCTSTR lpszAddress)
{
	ASSERT(lpszAddress && lpszAddress[0] != _T('\0'));

	BYTE nField0, nField1, nField2, nField3;

	StringAddressToByte(lpszAddress, nField0, nField1, nField2, nField3);

	CIPAddressCtrl::SetAddress(nField0, nField1, nField2, nField3);
}

///////////////////////////////////////////////////////////////////////////////
//
// ByteAddressToString()
//
// Purpose:     Convert the four byte fields to a dotted IP address string. 
//
// Parameters:  nField0 - [in] field 0 from a packed IP address
//              nField1 - [in] field 1 from a packed IP address
//              nField2 - [in] field 2 from a packed IP address
//              nField3 - [in] field 3 from a packed IP address
//
// Returns:     CString - dotted IP address string; e.g., "127.0.0.1"
//
CString CXIPAddressCtrl::ByteAddressToString(BYTE nField0, 
											 BYTE nField1, 
											 BYTE nField2, 
											 BYTE nField3)
{
	CString strIP = _T("");

	strIP.Format(_T("%u.%u.%u.%u"), nField0, nField1, nField2, nField3);

	return strIP;
}

///////////////////////////////////////////////////////////////////////////////
//
// StringAddressToByte()
//
// Purpose:     Convert a dotted IP address string to four byte fields.
//
// Parameters:  lpszAddress - [in]  dotted IP address string
//              nField0     - [out] reference to field 0 of IP address
//              nField1     - [out] reference to field 1 of IP address
//              nField2     - [out] reference to field 2 of IP address
//              nField3     - [out] reference to field 3 of IP address
//
// Returns:     CString     - dotted IP address string; e.g., "127.0.0.1"
//
void CXIPAddressCtrl::StringAddressToByte(LPCTSTR lpszAddress,
										  BYTE& nField0, 
										  BYTE& nField1, 
										  BYTE& nField2, 
										  BYTE& nField3)
{
	ASSERT(lpszAddress && lpszAddress[0] != _T('\0'));

	// get copy of string for _tcstok
	int len = _tcslen(lpszAddress)+10;		// size in TCHARs
	TCHAR *buf_new = new TCHAR[len * sizeof(TCHAR)];
	TCHAR *buf=buf_new;
	memset(buf, 0, len * sizeof(TCHAR));
	_tcscpy_s(buf, len * sizeof(TCHAR), lpszAddress);

	TCHAR *seps = _T(".\r\n");
	TCHAR *field = NULL;
	TCHAR *context;
	BYTE fields[4] = { 0 };
	field = _tcstok_s(buf, seps, &context);

	// loop to get all 4 fields
	for (int i = 0; i < 4; i++)
	{
		if (!field)
			break;
		fields[i] = (BYTE) _tcstoul(field, NULL, 10);
		field = _tcstok_s(NULL, seps, &context);
	}

	nField0 = fields[0];
	nField1 = fields[1];
	nField2 = fields[2];
	nField3 = fields[3];

	delete[] buf_new;
}

///////////////////////////////////////////////////////////////////////////////
//
// ReadProfileAddress()
//
// Purpose:     Retrieve the IP address from the profile and write it to the 
//              control.
//
// Parameters:  lpszSection - [in] pointer to string that specifies the section 
//                            containing the entry
//              lpszEntry   - [in] pointer to string that contains the entry 
//                            whose value is to be retrieved
//              lpszDefault - [in] pointer to default string value for the 
//                            given entry if the entry cannot be found 
//
// Returns:     CString     - string from the application’s profile, or 
//                            lpszDefault if the string cannot be found
//
CString CXIPAddressCtrl::ReadProfileAddress(LPCTSTR lpszSection,
											LPCTSTR lpszEntry,
											LPCTSTR lpszDefault /*= _T("0.0.0.0")*/)
{
	ASSERT(lpszSection && lpszSection[0] != _T('\0'));
	ASSERT(lpszEntry && lpszEntry[0] != _T('\0'));

	CString strDefault = lpszDefault;

	CString strIP = AfxGetApp()->GetProfileString(lpszSection, lpszEntry, strDefault);

	SetAddress(strIP);

	return strIP;
}

///////////////////////////////////////////////////////////////////////////////
//
// WriteProfileAddress()
//
// Purpose:     Retrieve the IP address from the control and write it to the 
//              profile.
//
// Parameters:  lpszSection - [in] pointer to string that specifies the section 
//                            containing the entry
//              lpszEntry   - [in] pointer to string that contains the entry 
//                            whose value is to be written
//
// Returns:     None
//
void CXIPAddressCtrl::WriteProfileAddress(LPCTSTR lpszSection, 
										  LPCTSTR lpszEntry)
{
	ASSERT(lpszSection && lpszSection[0] != _T('\0'));
	ASSERT(lpszEntry && lpszEntry[0] != _T('\0'));

	CString strIP = _T("");

	GetAddress(strIP);

	AfxGetApp()->WriteProfileString(lpszSection, lpszEntry, strIP);
}
