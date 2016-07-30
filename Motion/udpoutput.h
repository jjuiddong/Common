//
// 2016-04-13, jjuiddong
// UDP 통신을 통해 모션정보를 보낸다.
//
#pragma once


namespace motion
{

	class cUDPOutput : public cOutput
	{
	public:
		cUDPOutput();
		virtual ~cUDPOutput();

		bool Init(const string &ip, const int port, const string &outputLog, const vector<string*> &formatScripts);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	public:
		string m_sendIp;
		int m_sendPort;
		string m_outputLogFileName;
		network::cUDPClient m_udpClient;
		vector<cFormatParser*> m_formats;
	};

}
