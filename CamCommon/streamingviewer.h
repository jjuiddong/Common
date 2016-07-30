//
// 2015-12-07, jjuiddong
//
// cStreamingReceiver와 비슷한 기능을 하지만, 클라이언트로 리시브 서버에
// 접속해, 이미지 정보를 받아오는 기능을 한다.
//
#pragma once


namespace cvproc
{

	class cStreamingViewer
	{
	public:
		cStreamingViewer();
		virtual ~cStreamingViewer();

		bool Init(const bool isUDP, const string &serverIp, const int port);
		cv::Mat& Update();


		//network::cUDPServer m_udpServer;
		network::cTCPClient	m_tcpServer;
		bool m_isUDP;
		cv::Mat m_src;
		cv::Mat m_finalImage;		// 최종 이미지
		BYTE *m_rcvBuffer;
	};

}
