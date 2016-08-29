//
// Protocol
//	- (byte) chunk size (이미지 용량 때문에 나눠진 데이타 청크의 갯수)
//	- (byte) chunk index
//	- (byte) gray 0 : 1
//	- (byte) compressed 0 : 1
//	- (int) image size
//
#pragma once

#include "streaming.h"


namespace cvproc
{
	class cStreamingReceiver
	{
	public:
		cStreamingReceiver();
		virtual ~cStreamingReceiver();

		bool Init(const bool isUDP, const string &ip, const int port, const int networkCardIdx=0,
			const int gray = 1, const int compressed = 1, const int jpgComQuality = 40, const int fps=20);
		int Update();
		bool DownLoadState(OUT int &maxLen, OUT int &readLen);
		string CheckError();
		bool IsConnect();
		void Close();


		network::cUDPServer m_udpServer;
		network::cTCPClient m_tcpClient;
		bool m_isUDP;
		cv::Mat m_src;
		vector<uchar> m_compBuffer;
		cv::Mat m_finalImage;		// 최종 이미지
		cv::Mat m_cloneImage;		// 최종 이미지가 만들어지고 난 후, 복사된 이미지
		BYTE *m_rcvBuffer;
		bool m_checkRcv[512]; // debug 용
		int m_chunkSize; // debug 용
		int m_currentChunkIdx;
		bool m_isBeginDownload;
		bool m_isLog;

		int m_oldCompressed = -1;
		int m_oldGray = -1;
		unsigned short m_oldRows = 0;
		unsigned short m_oldCols = 0;
		int m_oldFlags = 0;
		unsigned char m_oldId = -1;
		string m_rcvUDPIp;
	};
}
