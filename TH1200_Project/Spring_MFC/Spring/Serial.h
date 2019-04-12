// Serial.h

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "windows.h"

class CSerial
{
	// ÐÞ¸Ä×Ô https://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2503/CSerial--A-C-Class-for-Serial-Communications.htm
public:
	CSerial();
	~CSerial();

	BOOL Open( int nPort, int nBaud = 9600 );
	BOOL Close( void );

	bool ReadData( void *buffer, int max_size, int* read_size, int timeout=0);
	bool SendData( void *buffer, int size);

	BOOL IsOpened( void ){ return( m_bOpened ); }

protected:
	BOOL WriteCommByte( unsigned char );

	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	BOOL m_bOpened;

	bool waitForData(unsigned int timeout);

};

#endif
