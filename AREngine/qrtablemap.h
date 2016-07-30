//
// 2016-03-17, jjuiddong
//
// QRCode 가 격자형태로 있을 때, 격자 무늬를 출력하고, 위치 정보를 계산하는 기능을 한다.
//
//
#pragma once



namespace ar
{
	class cQRCode;

	class cQRTablemap
	{
	public:
		cQRTablemap();
		virtual ~cQRTablemap();

		void Init(graphic::cRenderer &renderer);
		void Update(graphic::cRenderer &renderer, ARUint8 *dataPtr);
		void GetCameraPos();
		void Render(graphic::cRenderer &renderer);


	public:
		vector<ar::cQRCode*> m_qrMap;
		sQRPos m_codes[64];
		vector<Vector3> m_qrTablePos; // 64개
		int m_nearQRCodeId;
		int m_detectCubeCount;
		int m_nearQuad[4];
		cRectContour2 m_quad;
		int m_tableW;
		int m_tableH;
		double m_patt_width;
		Mat m_homograpyMat;

		graphic::cCube2 m_cube;
		graphic::cModel m_model;
		graphic::cText *m_text;
		graphic::cLine2d m_line;
	};

}
