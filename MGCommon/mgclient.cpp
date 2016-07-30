
#include "stdafx.h"
#include "mgclient.h"

using namespace std;
using namespace network;


//-----------------------------------------------------------------------------------
// 비동기 서버 접속 쓰레드
unsigned WINAPI MGClientConnectAsyncThreadFunction(void* arg)
{
	cMGClient *mgClient = (cMGClient*)arg;
	const string ip = mgClient->m_tcpClient.m_ip;
	const int port = mgClient->m_tcpClient.m_port;

	if (mgClient->m_clientMode)
	{
		if (mgClient->m_tcpClient.Init(ip, port, cMGClient::MAX_BUFFLEN, 20))
		{
			cout << endl << "Success MGClient Connect Server ip = " << ip << ", port = " << port << endl << endl;
		}
		else
		{
			cout << endl << "Fail!! MGClient Connect Server ip = " << ip << ", port = " << port << endl << endl;
		}
	}
	else
	{
		if (mgClient->m_tcpServer.Init(port, cMGClient::MAX_BUFFLEN, 20))
		{
			cout << endl << "Success MGClient Bind Server port = " << port << endl << endl;
		}
		else
		{
			cout << endl << "Fail!! MGClient Bind Server " << port << endl << endl;
		}
	}

	return 0;
}
//-----------------------------------------------------------------------------------


cMGClient::cMGClient() :
	m_recvBuffer(NULL)
	, m_sendBuffer(NULL)
{
	m_searchPoint[0] = NULL;
	m_searchPoint[1] = NULL;
}

cMGClient::~cMGClient()
{
	Close();

	SAFE_DELETEA(m_recvBuffer);
	SAFE_DELETEA(m_sendBuffer);
}


bool cMGClient::Init(cvproc::cSearchPoint *searchPoint1, cvproc::cSearchPoint *searchPoint2)
{
	m_searchPoint[0] = searchPoint1;
	m_searchPoint[1] = searchPoint2;

	return true;
}


// 비동기로 서버에 접속을 시도한다.
// 서버에 접속하는 동안, 게임이 느려지는 것을 방지하기 위함이다.
bool cMGClient::ConnectAsync(const bool clientMode, const string &ip, const int port)
{
	m_clientMode = clientMode;
	m_tcpClient.m_ip = ip;
	m_tcpClient.m_port = port;
	if (!m_sendBuffer)
		m_sendBuffer = new BYTE[MAX_BUFFLEN];
	if (!m_recvBuffer)
		m_recvBuffer = new BYTE[MAX_BUFFLEN];

	m_tcpClient.Close();
	m_tcpServer.Close();
	m_handle = (HANDLE)_beginthreadex(NULL, 0, MGClientConnectAsyncThreadFunction, this, 0, (unsigned*)&m_threadId);

	return true;
}


bool cMGClient::IsConnect()
{
	if (m_clientMode)
		return m_tcpClient.IsConnect();
	return m_tcpServer.IsConnect();
}


void cMGClient::Close()
{
	if (m_handle)
	{
		::WaitForSingleObject(m_handle, 1000);
		m_handle = NULL;
	}
	m_tcpClient.Close();
	m_tcpServer.Close();
}


int cMGClient::Update()
{
	RETV(!m_recvBuffer, 0);
	RETV(!m_sendBuffer, 0);
	RETV(!m_searchPoint[0], 0);
	RETV(m_clientMode && !m_tcpClient.IsConnect(), 0);
	RETV(!m_clientMode && !m_tcpServer.IsConnect(), 0);

	sPacket tmpPacket;

	if (m_clientMode)
	{
		if (!m_tcpClient.m_recvQueue.Front(tmpPacket))
			return 0;

		memcpy(m_recvBuffer, tmpPacket.buffer, tmpPacket.actualLen);
		m_tcpClient.m_recvQueue.Pop();
	}
	else
	{
		if (!m_tcpServer.m_recvQueue.Front(tmpPacket))
			return 0;

		memcpy(m_recvBuffer, tmpPacket.buffer, tmpPacket.actualLen);
		m_tcpServer.m_recvQueue.Pop();
	}

	mgnetwork::sMG_NetProtocol_Header *header = (mgnetwork::sMG_NetProtocol_Header*)m_recvBuffer;
	if ((header->head[0] != '&') || (header->head[1] != '@'))
		return 0;


	//----------------------------------------------------------------------------------------------
	// 패킷 처리
	const mgnetwork::PROTOCOL::TYPE protocol = (mgnetwork::PROTOCOL::TYPE)header->protocol;
	BYTE *data = m_recvBuffer + sizeof(mgnetwork::sMG_NetProtocol_Header);

	switch (protocol)
	{
	case mgnetwork::PROTOCOL::SC_REQ_GIVE_MGCONFIG:
		RcvReqGiveMGConfig(*(mgnetwork::sSC_ReqGiveMGConfig*)data);
		break;
	case mgnetwork::PROTOCOL::SC_REQ_GIVE_CAMERA_CONTOUR:
		RcvReqGiveCameraContour(*(mgnetwork::sSC_ReqGiveCameraContour*)data);
		break;
	case mgnetwork::PROTOCOL::SC_REQ_GIVE_CAMERA_RECOGNITION_CONFIG:
		RcvReqGiveCameraRecognitionConfig(*(mgnetwork::sSC_ReqGiveCameraRecognitionConfig*)data);
		break;
	case mgnetwork::PROTOCOL::SC_REQ_GIVE_ROI:
		RcvReqGiveRoi(*(mgnetwork::sSC_ReqGiveRoi*)data);
		break;	

	case mgnetwork::PROTOCOL::SC_REQ_SET_MGCONFIG: 
		RcvReqSetMGConfig(*(mgnetwork::sSC_ReqSetMGConfig*)data);
		break;
	case mgnetwork::PROTOCOL::SC_REQ_SET_CAMERA_CONTOUR:
		RcvReqSetCameraContour(*(mgnetwork::sSC_ReqSetCameraContour*)data);
		break;
	case mgnetwork::PROTOCOL::SC_REQ_SET_CAMERA_RECOGNITION_CONFIG:
		RcvReqSetCameraRecognitionConfig(*(mgnetwork::sSC_ReqSetCameraRecognitionConfig*)data);
		break;
	case mgnetwork::PROTOCOL::SC_REQ_SET_ROI:
		RcvReqSetRoi(*(mgnetwork::sSC_ReqSetRoi*)data);
		break;
	default:
		cout << "MG Network Protocol Error!!" << endl;
		break;
	}
	//----------------------------------------------------------------------------------------------

	return 0;
}


void cMGClient::RcvReqGiveMGConfig(const mgnetwork::sSC_ReqGiveMGConfig &mgConfig)
{
	mgnetwork::sMG_NetProtocol_Header *header = (mgnetwork::sMG_NetProtocol_Header*)m_sendBuffer;
	header->head[0] = '&';
	header->head[1] = '@';
	header->protocol = mgnetwork::PROTOCOL::CS_ACK_MGCONFIG;

	BYTE *dst = m_sendBuffer + sizeof(mgnetwork::sMG_NetProtocol_Header);
	memcpy(dst, &g_config.m_conf, sizeof(g_config.m_conf));

	if (m_clientMode)
		m_tcpClient.Send(m_sendBuffer, MAX_BUFFLEN);
	else if (!m_tcpServer.m_sessions.empty())
		m_tcpServer.m_sendQueue.Push(m_tcpServer.m_sessions[0].socket, m_sendBuffer, MAX_BUFFLEN);
}


void cMGClient::RcvReqGiveCameraContour(const mgnetwork::sSC_ReqGiveCameraContour &camContour)
{
	mgnetwork::sMG_NetProtocol_Header *header = (mgnetwork::sMG_NetProtocol_Header*)m_sendBuffer;
	header->head[0] = '&';
	header->head[1] = '@';
	header->protocol = mgnetwork::PROTOCOL::CS_ACK_CAMERA_CONTOUR;

	BYTE *dst = m_sendBuffer + sizeof(mgnetwork::sMG_NetProtocol_Header);
	mgnetwork::sCS_AckCameraContour *ack = (mgnetwork::sCS_AckCameraContour*)dst;

	ack->playerIndex = camContour.playerIndex;

	if ((camContour.playerIndex == 0) || (camContour.playerIndex == 1))
	{
		if (m_searchPoint[camContour.playerIndex])
		{
			ack->scale = m_searchPoint[camContour.playerIndex]->m_skewDetect.m_scale;
			ack->pos[0] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[0].x;
			ack->pos[1] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[0].y;
			ack->pos[2] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[1].x;
			ack->pos[3] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[1].y;
			ack->pos[4] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[2].x;
			ack->pos[5] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[2].y;
			ack->pos[6] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[3].x;
			ack->pos[7] = (float)m_searchPoint[camContour.playerIndex]->m_skewDetect.m_contour.m_contours[3].y;

			if (m_clientMode)
				m_tcpClient.Send(m_sendBuffer, MAX_BUFFLEN);
			else if (!m_tcpServer.m_sessions.empty())
				m_tcpServer.m_sendQueue.Push(m_tcpServer.m_sessions[0].socket, m_sendBuffer, MAX_BUFFLEN);
		}
	}
}


void cMGClient::RcvReqGiveCameraRecognitionConfig(const mgnetwork::sSC_ReqGiveCameraRecognitionConfig &recogConfig)
{
	mgnetwork::sMG_NetProtocol_Header *header = (mgnetwork::sMG_NetProtocol_Header*)m_sendBuffer;
	header->head[0] = '&';
	header->head[1] = '@';
	header->protocol = mgnetwork::PROTOCOL::CS_ACK_CAMERA_RECOGNITION_CONFIG;

	BYTE *dst = m_sendBuffer + sizeof(mgnetwork::sMG_NetProtocol_Header);
	mgnetwork::sCS_AckCameraRecognitionConfig *ack = (mgnetwork::sCS_AckCameraRecognitionConfig*)dst;

	if ((recogConfig.playerIndex == 0) || (recogConfig.playerIndex == 1))
	{
		if (m_searchPoint[recogConfig.playerIndex])
		{
			memcpy(&ack->config,
				&m_searchPoint[recogConfig.playerIndex]->m_recogConfig.m_attr, 
				sizeof(ack->config));

			if (m_clientMode)
				m_tcpClient.Send(m_sendBuffer, MAX_BUFFLEN);
			else if (!m_tcpServer.m_sessions.empty())
				m_tcpServer.m_sendQueue.Push(m_tcpServer.m_sessions[0].socket, m_sendBuffer, MAX_BUFFLEN);
		}
	}
}


void cMGClient::RcvReqGiveRoi(const mgnetwork::sSC_ReqGiveRoi &roi)
{
	mgnetwork::sMG_NetProtocol_Header *header = (mgnetwork::sMG_NetProtocol_Header*)m_sendBuffer;
	header->head[0] = '&';
	header->head[1] = '@';
	header->protocol = mgnetwork::PROTOCOL::CS_ACK_ROI;

	BYTE *dst = m_sendBuffer + sizeof(mgnetwork::sMG_NetProtocol_Header);
	mgnetwork::sCS_AckRoi *ack = (mgnetwork::sCS_AckRoi*)dst;

	if ((roi.playerIndex == 0) || (roi.playerIndex == 1))
	{
		if (m_searchPoint[roi.playerIndex])
		{
			ack->pos[0] = m_searchPoint[roi.playerIndex]->m_roi.m_rect.x;
			ack->pos[1] = m_searchPoint[roi.playerIndex]->m_roi.m_rect.y;
			ack->pos[2] = m_searchPoint[roi.playerIndex]->m_roi.m_rect.width;
			ack->pos[3] = m_searchPoint[roi.playerIndex]->m_roi.m_rect.height;

			if (m_clientMode)
				m_tcpClient.Send(m_sendBuffer, MAX_BUFFLEN);
			else if (!m_tcpServer.m_sessions.empty())
				m_tcpServer.m_sendQueue.Push(m_tcpServer.m_sessions[0].socket, m_sendBuffer, MAX_BUFFLEN);
		}
	}
}


void cMGClient::RcvReqSetMGConfig(const mgnetwork::sSC_ReqSetMGConfig &mgConfig)
{
	// 환경설정 파일 정보 업데이트
	memcpy(&g_config.m_conf, &mgConfig, sizeof(mgConfig));

	// 설정파일이 업데이트 되면, 파일에 저장한다.
	g_config.Write(g_configfileName);


	// 환경설정 파일 정보가 바뀌면, 스트리밍 옵션이 바꼈는지 확인한다.
	static bool firstCode = true;

	static int serverPort = mgConfig.config.stream_server_port;
	static bool gray = mgConfig.config.stream_send_gray;
	static bool compressed = mgConfig.config.stream_send_compressed;
	static int quality = mgConfig.config.streaming_send_quality;

	const bool checkChange = 
		(serverPort != mgConfig.config.stream_server_port) ||
		(gray != mgConfig.config.stream_send_gray) ||
		(compressed != mgConfig.config.stream_send_compressed) ||
		(quality != mgConfig.config.streaming_send_quality);

	// 정보가 바꼈거나, 처음으로 실행 될 때는 true가 된다.
	if (checkChange || firstCode)
	{
		firstCode = false;

		// 정보가 바꼈다면, true 체크
		g_config.m_changeStreamingOption = true;
	}
}


void cMGClient::RcvReqSetCameraContour(const mgnetwork::sSC_ReqSetCameraContour &camContour)
{
	vector<Point2f> pos(4);
	for (int i = 0; i < 4; ++i)
	{
		pos[i].x = camContour.pos[i * 2];
		pos[i].y = camContour.pos[i * 2 + 1];
	}

	cRectContour contour;
	contour.Init(pos);
	const int width = contour.Width();
	const int height = contour.Height();

	if ((camContour.playerIndex == 0) || (camContour.playerIndex == 1))
	{
		if (m_searchPoint[camContour.playerIndex])
		{
			m_searchPoint[camContour.playerIndex]->m_skewDetect.Init(
				contour, camContour.scale, width, height);

			m_searchPoint[camContour.playerIndex]->m_skewDetect.Write(
				(camContour.playerIndex == 0) ? g_contourfilename1 : g_contourfilename2);
		}
	}
}


void cMGClient::RcvReqSetCameraRecognitionConfig(const mgnetwork::sSC_ReqSetCameraRecognitionConfig &recogConfig)
{
	if ((recogConfig.playerIndex == 0) || (recogConfig.playerIndex == 1))
	{
		if (m_searchPoint[recogConfig.playerIndex])
		{
			memcpy(&m_searchPoint[recogConfig.playerIndex]->m_recogConfig.m_attr, 
				&recogConfig.config, sizeof(recogConfig.config));

			// 설정파일이 업데이트 되면, 파일에 저장한다.
			m_searchPoint[recogConfig.playerIndex]->m_recogConfig.Write(
				(recogConfig.playerIndex == 0) ? g_recognition_configfilename1 : g_recognition_configfilename2);
		}
	}
}


void cMGClient::RcvReqSetRoi(const mgnetwork::sSC_ReqSetRoi &roi)
{
	if ((roi.playerIndex == 0) || (roi.playerIndex == 1))
	{
		if (m_searchPoint[roi.playerIndex])
		{
			m_searchPoint[roi.playerIndex]->m_roi.m_rect.x = roi.pos[0];
			m_searchPoint[roi.playerIndex]->m_roi.m_rect.y = roi.pos[1];
			m_searchPoint[roi.playerIndex]->m_roi.m_rect.width = roi.pos[2];
			m_searchPoint[roi.playerIndex]->m_roi.m_rect.height = roi.pos[3];

			// 설정파일이 업데이트 되면, 파일에 저장한다.
			m_searchPoint[roi.playerIndex]->m_roi.Write(
				(roi.playerIndex == 0) ? g_roi_filename1 : g_roi_filename2);
		}
	}
}
