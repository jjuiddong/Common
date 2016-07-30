//
// 2015-12-08, jjuiddong
//
// MachineGun Camera Server(TCP/IP)에 접속해 필요한 정보를 주고 받는 역할을 한다.
//
//
#pragma once


class cMGClient
{
public:
	cMGClient();
	virtual ~cMGClient();
	
	bool Init(cvproc::cSearchPoint *searchPoint1, cvproc::cSearchPoint *searchPoint2);
	bool ConnectAsync(const bool clientMode, const string &ip, const int port);
	int Update();
	bool IsConnect();
	void Close();


protected:
	void RcvReqGiveMGConfig(const mgnetwork::sSC_ReqGiveMGConfig &mgConfig);
	void RcvReqGiveCameraContour(const mgnetwork::sSC_ReqGiveCameraContour &camContour);
	void RcvReqGiveCameraRecognitionConfig(const mgnetwork::sSC_ReqGiveCameraRecognitionConfig &recogConfig);
	void RcvReqGiveRoi(const mgnetwork::sSC_ReqGiveRoi &roi);

	void RcvReqSetMGConfig(const mgnetwork::sSC_ReqSetMGConfig &mgConfig);
	void RcvReqSetCameraContour(const mgnetwork::sSC_ReqSetCameraContour &camContour);
	void RcvReqSetCameraRecognitionConfig(const mgnetwork::sSC_ReqSetCameraRecognitionConfig &recogConfig);
	void RcvReqSetRoi(const mgnetwork::sSC_ReqSetRoi &roi);


public:
	enum {MAX_BUFFLEN = 128, };
	network::cTCPClient m_tcpClient;
	network::cTCPServer m_tcpServer;
	cvproc::cSearchPoint *m_searchPoint[2];
	bool m_clientMode; // true:client, false:server
	BYTE *m_recvBuffer;
	BYTE *m_sendBuffer;
	HANDLE m_handle;
	DWORD m_threadId;
};
