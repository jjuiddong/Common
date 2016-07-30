//
// 2016-03-17, jjuiddong
//
// ARToolkit에서 인식한 QRCode를 저장한다.
// 
//

#pragma once


namespace ar
{

	class cQRCode
	{
	public:
		cQRCode();
		virtual ~cQRCode();

		void Init(graphic::cRenderer &renderer, graphic::cModel *model, graphic::cText *text);
		void Init2(graphic::cRenderer &renderer, graphic::cCube2 *cube, graphic::cText *text);
		void Render(graphic::cRenderer &renderer);


	public:
		int m_id; // QRCode ID 
		Matrix44 m_tm;	// QRCode를 인식한 후, 3차원 상에서의 카메라 행렬

		// display
		graphic::cCube2 *m_cube;
		graphic::cModel *m_model; // reference
		graphic::cText *m_text; // reference
	};

}
