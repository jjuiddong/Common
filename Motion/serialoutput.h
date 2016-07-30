//
// 2026-04-13, jjuiddong
// 시리얼 통신으로 모션 결과를 내보내는 기능을 한다.
// 
#pragma once

#include "output.h"

namespace motion
{
	
	class cSerialOutput : public cOutput
	{
	public:
		cSerialOutput();
		virtual ~cSerialOutput();

		bool Init(const int portNum, const int baudRate, const string &outputLog, const vector<string*> &formatScripts);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;
		virtual void SendImmediate() override;
		virtual void SetFormat(const int index) override;


	public:
		int m_portNum;
		int m_baudRate;
		string m_outputLogFileName;
		bool m_isAutoClose; // Stop 후 자동으로 시리얼 통신과 끊기게 된다.
		cSerialAsync m_serial;
		vector<cFormatParser*> m_formats;
	};

}
