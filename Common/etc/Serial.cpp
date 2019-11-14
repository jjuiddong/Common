
#include "stdafx.h"
#include "Serial.h"

using namespace common;


cSerial::cSerial()
	: m_dev(nullptr)
	, m_isOpened(false)
{
	memset(&m_read, 0, sizeof(m_read));
 	memset(&m_write, 0, sizeof(m_write));
}

cSerial::~cSerial()
{
	Close();
}


bool cSerial::Open( const int nPort, const int nBaud )
{
	if (m_isOpened)
		return true;

	TCHAR szPort[15];
	DCB dcb;

	//wsprintf( szPort, TEXT("COM%d"), nPort );
	wsprintf(szPort, TEXT("\\\\.\\COM%d"), nPort);
	m_dev = CreateFile( szPort
		, GENERIC_READ | GENERIC_WRITE
		, 0, NULL, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED
		, NULL );

	if (!m_dev) 
		return false;

	memset(&m_read, 0, sizeof(m_read));
 	memset(&m_write, 0, sizeof(m_write));

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_dev, &CommTimeOuts );

	m_read.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_write.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	const DWORD inputBufferSize = 10000;
	const DWORD outputBufferSize = 10000;

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_dev, &dcb );
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	if (!SetCommState(m_dev, &dcb) 
		|| !SetupComm( m_dev, inputBufferSize, outputBufferSize)
		|| !m_read.hEvent
		|| !m_write.hEvent)
	{
		const DWORD dwError = GetLastError();
		if (m_read.hEvent)
		{
			CloseHandle(m_read.hEvent);
			m_read.hEvent = nullptr;
		}
		if (m_write.hEvent)
		{
			CloseHandle(m_write.hEvent);
			m_write.hEvent = nullptr;
		}

		CloseHandle(m_dev);
		m_dev = nullptr;
		return false;
	}

	m_isOpened = true;

	return m_isOpened;
 }


bool cSerial::Close()
{
	if (!m_isOpened || !m_dev)
		return true;

	if (m_read.hEvent)
	{
		CloseHandle(m_read.hEvent);
		m_read.hEvent = nullptr;
	}
	
	if (m_write.hEvent)
	{
		CloseHandle(m_write.hEvent);
		m_write.hEvent = nullptr;
	}

	CloseHandle(m_dev);
	m_isOpened = false;
	m_dev = nullptr;

	return true;
}


bool cSerial::WriteCommByte( unsigned char ucByte )
{
	DWORD dwBytesWritten;

	const BOOL bWriteStat = WriteFile(m_dev, (LPSTR) &ucByte, 1, &dwBytesWritten
		, &m_write);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING))
	{
		if (WaitForSingleObject(m_write.hEvent, 1000))
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult(m_dev, &m_write
				, &dwBytesWritten, FALSE);
			m_write.Offset += dwBytesWritten;
		}
	}

	return true;
}


int cSerial::SendData( const char *buffer, const int size )
{
	if (!m_isOpened || !m_dev)
		return 0;

	int bytesWritten = 0;
	for (int i=0; i < size; i++)
	{
		const bool result = WriteCommByte(buffer[i]);
		bytesWritten++;
	}

	return bytesWritten;
}


// return receive data size
int cSerial::ReadDataWaiting()
{
	if (!m_isOpened || !m_dev)
		return 0;

	DWORD dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError( m_dev, &dwErrorFlags, &ComStat );
	return (int)ComStat.cbInQue;
}


int cSerial::ReadData( void *buffer, const int limit )
{
	if (!m_isOpened || !m_dev)
		return 0;

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	const BOOL isSucceed = ClearCommError(m_dev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) 
		return 0;

	if (!isSucceed)
	{
		m_isOpened = false;
		return -1;
	}

	DWORD dwBytesRead = (DWORD)ComStat.cbInQue;
	if (limit < (int)dwBytesRead) 
		dwBytesRead = (DWORD)limit;

	const BOOL bReadStatus = ReadFile(m_dev, buffer
		, dwBytesRead, &dwBytesRead, &m_read);
	if (!bReadStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_read.hEvent, 2000 );
			return (int)dwBytesRead;
		}
		return 0;
	}

	return (int)dwBytesRead;
}
