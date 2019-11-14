//
// author: jjuiddong
//  2015-04-04
//
// CSerial 클래스를 상속 받아, ReadStringUntil() 함수를 구현한 클래스다.
// 특정 문자 (개행문자) 단위로 정보를 읽어 올 때, 고속으로 동작하는 cSerial 클래스를
// 오버헤드 없이, 버퍼를 효율적으로 관리하면서, 원하는 기능을 제공하는 역할을 한다.
//
// 링버퍼로 구현되어서, 버퍼 재할당, 복사가 일어나지 않게 했다.
// 이 클래스를 사용할 경우, ReadData()함수 대신, ReadStringUntil() 함수를 써야한다.
//
//
// 2015-08-25
// 일정 시간 이상동안 ch문자가 들어오지 않으면, 버퍼를 리턴한다.
//
// 2019-06-22
//	CircularQueue 적용
//
// 2019-11-14, jjuiddong
//		- read buffer bugfix
//
//
#pragma once
#include "Serial.h"


namespace common
{

	class cBufferedSerial : public cSerial
	{
	public:
		cBufferedSerial();
		virtual ~cBufferedSerial();

		bool ReadStringUntil(const char ch, OUT char *out
			, OUT int &outLen, const int maxSize);
		void ClearBuffer();


	public:
		enum { MAX_BUFFERSIZE = 2048 };
		cCircularQueue<char, MAX_BUFFERSIZE> m_q;
	};

}
