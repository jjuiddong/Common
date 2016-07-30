//
// 2015-12-08, jjuiddong
//
// 머신건 네트워크 통신에 관련된 전역 선언을 정의한다.
//
#pragma once


namespace mgnetwork
{

	namespace PROTOCOL
	{
		// SC : Server -> Client
		// CS : Client -> Server
		enum TYPE
		{
			SC_REQ_GIVE_MGCONFIG = 1000,
			CS_ACK_MGCONFIG,
			SC_REQ_SET_MGCONFIG,

			SC_REQ_GIVE_CAMERA_CONTOUR,
			CS_ACK_CAMERA_CONTOUR,
			SC_REQ_SET_CAMERA_CONTOUR,

			SC_REQ_GIVE_CAMERA_RECOGNITION_CONFIG,
			CS_ACK_CAMERA_RECOGNITION_CONFIG,
			SC_REQ_SET_CAMERA_RECOGNITION_CONFIG,

			SC_REQ_GIVE_ROI,
			CS_ACK_ROI,
			SC_REQ_SET_ROI,
		};
	}

	enum {MAX_BUFFLEN=128,};


	// Protocol
	struct sMG_NetProtocol_Header
	{
		char head[2]; // &@
		int protocol; // MG_PROTOCOL::TYPE
	};

	struct sSC_ReqGiveMGConfig
	{
		char non;
	};

	struct sCS_AckMGConfig
	{
		sCameraConfig config;
	};

	struct sSC_ReqSetMGConfig
	{
		sCameraConfig config;
	};



	struct sSC_ReqGiveCameraContour
	{
		int playerIndex; // 0=player1, 1=player2
	};

	struct sCS_AckCameraContour
	{
		int playerIndex; // 0=player1, 1=player2
		float scale;
		float pos[8]; // pos1xy, pos2xy ~ pos4xy
	};

	struct sSC_ReqSetCameraContour
	{
		int playerIndex; // 0=player1, 1=player2
		float scale;
		float pos[8]; // pos1xy, pos2xy ~ pos4xy
	};



	struct sSC_ReqGiveRoi
	{
		int playerIndex; // 0=player1, 1=player2
	};

	struct sCS_AckRoi
	{
		int playerIndex; // 0=player1, 1=player2
		int pos[4]; // x,y,width,height
	};

	struct sSC_ReqSetRoi
	{
		int playerIndex; // 0=player1, 1=player2
		int pos[4]; //x,y,width,height
	};


	struct sSC_ReqGiveCameraRecognitionConfig
	{
		int playerIndex; // 0=player1, 1=player2
	};

	struct sSC_ReqSetCameraRecognitionConfig
	{
		int playerIndex; // 0=player1, 1=player2
		sRecognitionConfigAttribute config;
	};

	struct sCS_AckCameraRecognitionConfig
	{
		int playerIndex; // 0=player1, 1=player2
		sRecognitionConfigAttribute config;
	};




	//--------------------------------------------------------------------
	// Shared Memory Communication
	struct sSharedMemData
	{
		bool tryConnectServer; // connect to tcp/ip server
								// true일 때 서버에 접속을 시도한다.
	};

}
