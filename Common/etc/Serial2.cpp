
#include "stdafx.h"
#include "Serial2.h"

using namespace common;


cSerial2::cSerial2()
	: m_dev(nullptr)
{
	memset(&m_read, 0, sizeof(m_read));
 	memset(&m_write, 0, sizeof(m_write));
}

cSerial2::~cSerial2()
{
	Close();
}


// connect serial 
// port: serial port number
// baud: baud rate, (9600, 115200 ... etc)
bool cSerial2::Open( const int port, const int baud )
{
	if (m_dev)
		return true; // already open

	char szPort[15];
	sprintf(szPort, "\\\\.\\COM%d", port);
	m_dev = CreateFileA( szPort
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

	m_read.hEvent = CreateEvent( nullptr, TRUE, FALSE, nullptr);
	m_write.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

	const DWORD inputBufferSize = 10000;
	const DWORD outputBufferSize = 10000;

	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_dev, &dcb );
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	if (!SetCommState(m_dev, &dcb) 
		|| !SetupComm( m_dev, inputBufferSize, outputBufferSize)
		|| !m_read.hEvent
		|| !m_write.hEvent)
	{
		Close();
		return false;
	}

	return true;
 }


// close serial
bool cSerial2::Close()
{
	if (!m_dev)
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
	m_dev = nullptr;
	return true;
}


// send data
bool cSerial2::WriteCommByte( unsigned char data )
{
	DWORD dwBytesWritten;
	const BOOL bWriteStat = WriteFile(m_dev, (LPSTR) &data, 1, &dwBytesWritten, &m_write);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING))
	{
		if (WaitForSingleObject(m_write.hEvent, 1000))
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult(m_dev, &m_write, &dwBytesWritten, FALSE);
			m_write.Offset += dwBytesWritten;
		}
	}

	return true;
}


// send data
// return: send data byte size
int cSerial2::SendData( const char *buffer, const uint size )
{
	if (!m_dev || (0 == size))
		return 0;

	DWORD dwBytesWritten = 0;
	const BOOL result = WriteFile(m_dev, buffer, size, &dwBytesWritten, &m_write);
	if (!result && (GetLastError() == ERROR_IO_PENDING))
	{
		if (WaitForSingleObject(m_write.hEvent, 1000))
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult(m_dev, &m_write, &dwBytesWritten, FALSE);
			m_write.Offset += dwBytesWritten;
		}
	}

	return (int)dwBytesWritten;
}


// return receive data size
// return -1: fail
int cSerial2::ReadDataWaiting()
{
	if (!m_dev)
		return 0;

	DWORD dwErrorFlags;
	COMSTAT ComStat;
	const BOOL result = ClearCommError( m_dev, &dwErrorFlags, &ComStat );
	if (!result)
	{
		Close();
		return -1;
	}
	return (int)ComStat.cbInQue;
}


// read data
// return: read byte size, -1:fail
int cSerial2::ReadData( void *buffer, const uint limit )
{
	if (!m_dev)
		return -1;

	const int queueSize = ReadDataWaiting();
	if (queueSize < 0)
		return -1;

	if (limit > (uint)queueSize)
		return 0; // wait

	DWORD dwBytesRead = (DWORD)limit;
	const BOOL result = ReadFile(m_dev, buffer, dwBytesRead, &dwBytesRead, &m_read);
	if (!result)
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
