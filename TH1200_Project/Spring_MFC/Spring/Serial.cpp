// Serial.cpp

#include "StdAfx.h"
#include "Serial.h"
#include "Utils.h"

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

CSerial::CSerial()
{

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	m_bOpened = FALSE;

}

CSerial::~CSerial()
{

	Close();

}

BOOL CSerial::Open( int nPort, int nBaud )
{

	if( m_bOpened ) return( TRUE );

	wchar_t szPort[15];
	wchar_t szComParams[50];
	DCB dcb;

	wsprintf( szPort, L"\\\\.\\COM%d", nPort );
	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
	if( m_hIDComDev == NULL ) return( FALSE );

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );

	wsprintf( szComParams, L"COM%d:%d,n,8,1", nPort, nBaud );

	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	unsigned char ucSet;
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );
	if( !SetCommState( m_hIDComDev, &dcb ) ||
		!SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL ){
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return( FALSE );
		}

	m_bOpened = TRUE;

	return( m_bOpened );

}

BOOL CSerial::Close( void )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( TRUE );

	if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
	if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return( TRUE );

}

BOOL CSerial::WriteCommByte( unsigned char ucByte )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) ){
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) dwBytesWritten = 0;
		else{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
			}
		}

	return( TRUE );

}

bool CSerial::SendData( void *buffer, int size )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );	

	PurgeComm(m_hIDComDev,PURGE_RXABORT|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_TXCLEAR);

	// https://msdn.microsoft.com/en-us/library/ff802693.aspx

	OVERLAPPED osWrite = {0};
	DWORD dwWritten=0;
	DWORD dwRes;
	bool isOk=false;

	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL){
		// error creating overlapped event handle
		return 0;
	}

	// Issue write.
	if (!WriteFile(m_hIDComDev, buffer, size, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) { 
			// WriteFile failed, but isn't delayed. Report error and abort.
			isOk = false;
		}else{
			// Write is pending.
			dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE); // 无限等待

			switch(dwRes)
			{
			// OVERLAPPED structure's event has been signaled. 
			case WAIT_OBJECT_0:
			if (!GetOverlappedResult(m_hIDComDev, &osWrite, &dwWritten, FALSE))
				isOk = false;
			else
				// Write operation completed successfully.
				isOk = true;
			break;
            
			default:
				// An error has occurred in WaitForSingleObject.
				// This usually indicates a problem with the
				// OVERLAPPED structure's event handle.
				isOk = false;
				break;
			}
		}
	}else{
		// WriteFile completed immediately.
		isOk = true;
	}

	CloseHandle(osWrite.hEvent);

	return isOk;

}

bool CSerial::ReadData( void *buffer, int max_size, int* read_size, int timeout)
{
	bool isOk=false;

	if( !m_bOpened || m_hIDComDev == NULL )
		return false;

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	if(false == waitForData(timeout))
		return false;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if( !ComStat.cbInQue )
		return false;

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( max_size < (int) dwBytesRead ) dwBytesRead = (DWORD) max_size;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead );

	if( FALSE == bReadStatus ){
		if( GetLastError() == ERROR_IO_PENDING ){
			WaitForSingleObject( m_OverlappedRead.hEvent, INFINITE ); // 无限等待
			isOk=true;
		}
	}else{ // bReadStatus is TRUE
		isOk=true;
	}

	*read_size=(int) dwBytesRead;

	return isOk;
}

bool CSerial::waitForData(unsigned int timeout){
	// https://stackoverflow.com/questions/25364525/win32-api-how-to-read-the-serial-or-exit-within-a-timeout-if-wasnt-a-data

	bool isOk=false;

	unsigned long Occured;//returns the type of an occured event
	OVERLAPPED FileEvent = {0};
	FileEvent.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD waitTimeout;
	waitTimeout = (timeout == 0 ? INFINITE : static_cast<DWORD>(timeout));

	if(!SetCommMask(m_hIDComDev, EV_RXCHAR)){//set a mask for incoming characters event.
		return false;
	}

	do{
		if(!WaitCommEvent(m_hIDComDev, &Occured, &FileEvent)){
			if(GetLastError() != ERROR_IO_PENDING){
				// debug_printf("W-ERR##1\n");
				// debug_printf("Wait failed with error %d.\n", GetLastError());
				isOk=false;
				break;
			}
		}

		switch(WaitForSingleObject(FileEvent.hEvent, waitTimeout)){
			case WAIT_OBJECT_0: //the requested event happened
				isOk=true; //a success
				break;
			case WAIT_TIMEOUT: //time out
				// debug_printf("W-timeout\n");
				isOk=false;
				break;
			default: //error in WaitForSingleObject
				// debug_printf("W-ERRRRRRRRRRRRRRRRRRR\n");
				isOk=false;
				break;
		}
	}while(0);

	CloseHandle(FileEvent.hEvent);

	return isOk;
}