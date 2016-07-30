
#include "stdafx.h"
#include "qrtablemap.h"

using namespace ar;
using namespace graphic;

cQRTablemap::cQRTablemap()
	: m_patt_width(80.0f)
	, m_model(0)
	, m_text(NULL)
	, m_detectCubeCount(0)
	, m_nearQRCodeId(-1)
	, m_line(4)
	, m_tableW(8)
	, m_tableH(6)
	, m_qrTablePos(64)
{
}

cQRTablemap::~cQRTablemap()
{
	for each(auto &qr in m_qrMap)
		delete qr;
	m_qrMap.clear();

	SAFE_DELETE(m_text);
}


void cQRTablemap::Init(graphic::cRenderer &renderer)
{
	ar::Init();


	// QR Maker 위치를 설정한다.
	const float sizeW = 100;
	const float sizeH = 100;
	for (int i = 0; i < m_tableW; ++i)
	{
		for (int k = 0; k < m_tableH; ++k)
		{
			const int id = m_tableW * k + i;
			m_qrTablePos[id] = Vector3(i*sizeW, k*sizeH, 0);
		}
	}


	const float size = 20.f;
	m_cube.SetCube(renderer, Vector3(-size, -size, -size), Vector3(size, size, size));
	m_cube.GetMaterial().InitGreen();

//		m_model.Create(renderer, "cube.dat");
//
// 	Matrix44 t;
// 	t.SetTranslate(Vector3(0, size, 0));
// 	Matrix44 m;
// 	m.SetScale(Vector3(5, 5, 5));
// 	m_model.SetTransform(m*t);

	m_text = new graphic::cText(renderer, "", 0, 0);
}


void cQRTablemap::Update(graphic::cRenderer &renderer, ARUint8 *dataPtr)
{
	const int contF = 0;
	static int contF2 = 0;
	static ARdouble patt_trans[3][4];
	ARdouble err;


	// detect the markers in the video frame
	if (arDetectMarker(cAREngine::Get()->m_arHandle, dataPtr) < 0)
	{
		//cleanup();
		return;
	}

	const int markerNum = arGetMarkerNum(cAREngine::Get()->m_arHandle);
	if (markerNum == 0)
	{
		argSwapBuffers();
		return;
	}

	// check for object visibility
	ARMarkerInfo *markerInfo = arGetMarker(cAREngine::Get()->m_arHandle);
	int k = -1;

	int detectCount = 0;

	for (int j = 0; j < markerNum; j++)
	{
		//ARLOG("ID=%d, CF = %f\n", markerInfo[j].id, markerInfo[j].cf);
		dbg::Print("ID=%d, CF = %f\n", markerInfo[j].id, markerInfo[j].cf);

		if (markerInfo[j].id >= 0)
		{
			if (k == -1)
			{
				if (markerInfo[j].cf >= 0.7) k = j;
			}
			else if (markerInfo[j].cf > markerInfo[k].cf)
			{
				k = j;
			}

			if (contF && contF2)
			{
				err = arGetTransMatSquareCont(cAREngine::Get()->m_ar3DHandle, &(markerInfo[j]), patt_trans, m_patt_width, patt_trans);
			}
			else
			{
				err = arGetTransMatSquare(cAREngine::Get()->m_ar3DHandle, &(markerInfo[j]), m_patt_width, patt_trans);
			}

			contF2 = 1;

			ARdouble gl_para[16];
			argConvGlpara(patt_trans, gl_para);

			float gl_para2[16];
			for (int i = 0; i < 16; ++i)
				gl_para2[i] = gl_para[i];

			Matrix44 viewM;
			D3DConvMatrixView(gl_para2, (D3DXMATRIXA16*)&viewM);

			if ((int)m_qrMap.size() <= detectCount)
			{
				cQRCode *code = new cQRCode();
				//code->Init(renderer, &m_model, m_text);
				code->Init2(renderer, &m_cube, m_text);
				code->m_id = markerInfo[j].id;
				code->m_tm = viewM;
				m_qrMap.push_back(code);
			}
			else
			{
				m_qrMap[detectCount]->m_id = markerInfo[j].id;
				m_qrMap[detectCount]->m_tm = viewM;
			}

			++detectCount;
		}
	}

	m_detectCubeCount = detectCount;


	if (!m_homograpyMat.empty())
	{
		std::vector<Point2f> camera_corners;
		camera_corners.push_back(Point2f(640/2, 480/2));
		std::vector<Point2f> world_corners;
		world_corners.reserve(1);
		perspectiveTransform(camera_corners, world_corners, m_homograpyMat);
 
		Mat dst(500, 700, CV_8SC3);
		dst.setTo(Scalar(100, 100, 100));
		//circle(dst, world_corners[0], 10, Scalar(0, 0, 100), CV_FILLED);

		const float u = world_corners[0].x / 400.f;
		const float v = world_corners[0].y / 400.f;
		const Vector3 p0 = m_qrTablePos[m_nearQuad[m_quad.m_chIndices[0]]];
		const Vector3 p1 = m_qrTablePos[m_nearQuad[m_quad.m_chIndices[1]]];
		const Vector3 p2 = m_qrTablePos[m_nearQuad[m_quad.m_chIndices[3]]];
		Vector3 v1 = p1 - p0;
		v1.Normalize();
		Vector3 v2 = p2 - p0;
		v2.Normalize();
		Vector3 pos = (v1 * u * 100.f) + (v2 * v * 100.f) + p0;
		gridboard(dst, 700, 500, 5, 7);
		circle(dst, Point((int)pos.x, (int)pos.y), 5, Scalar(100, 0, 0), CV_FILLED);

		imshow("homograpy", dst);
		waitKey(1);
 	}


// 	if (k == -1)
// 	{
// 		contF2 = 0;
// 		argSwapBuffers();
// 		return;
// 	}
// 
// 
// 	sprintf_s(errValue, "err = %f", err);
// 	glColor3f(0.0f, 1.0f, 0.0f);
// 	argDrawStringsByIdealPos(fps, 10, ysize - 30);
// 	argDrawStringsByIdealPos(errValue, 10, ysize - 60);
// 	//ARLOG("err = %f\n", err);
// 
// 	argSwapBuffers();
}


void cQRTablemap::GetCameraPos()
{

}


void cQRTablemap::Render(graphic::cRenderer &renderer)
{

	//----------------------------------------------------------------------------------------------------
	// QRCode 격자무늬 출력
	//sQRPos m_codes[64];
	bool codeEdge[64][64];
	const int startId = 0;

	ZeroMemory(m_codes, sizeof(m_codes));
	ZeroMemory(codeEdge, sizeof(codeEdge));

	// QRCode 스크린좌표 계산
	for (int i = 0; i < m_detectCubeCount; ++i)
	{
		const int id = m_qrMap[i]->m_id;
		m_codes[id].used = true;

		GetMainCamera()->SetViewMatrix(m_qrMap[i]->m_tm);
		renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_qrMap[i]->m_tm);
		Vector3 textPos = GetMainCamera()->GetScreenPos(640, 480, Vector3(0, 0, 0));
		m_codes[id].pos = Point((int)textPos.x, (int)textPos.y);
	}


	//----------------------------------------------------------------------------------------------------
	// Edge 그리기
	for (int i = 0; i < m_detectCubeCount; ++i)
	{
		const int id = m_qrMap[i]->m_id;

		// left, top, right, bottom
		const int surroundIds[] = { id - 1, id - m_tableW, id + 1, id + m_tableW };

		for (int k = 0; k < 4; ++k)
		{
			const int sid = surroundIds[k];
			if (sid < 0)
				continue;
			if (sid >= (m_tableH*m_tableW))
				continue;
			if (!m_codes[sid].used)
				continue;
			if (codeEdge[id][sid])
				continue;

			if ((k == 0) && ((sid % m_tableW) == (m_tableW - 1))) // left 가 테이블의 가장 오른쪽 일경우
			{
				continue;
			}
			if ((k == 2) && ((sid % m_tableW) == 0)) // right 가 테이블의 가장 왼쪽 일경우
			{
				continue;
			}

			// edge 그리기
			Vector2 vList[] = {
				Vector2(m_codes[id].pos.x, m_codes[id].pos.y),
				Vector2(m_codes[sid].pos.x, m_codes[sid].pos.y)
			};

			m_line.m_color = D3DCOLOR_XRGB(0, 0, 255);
			m_line.Render(renderer, vList, 2);

			codeEdge[id][sid] = true;
			codeEdge[sid][id] = true;
		}
	}


	//----------------------------------------------------------------------------------------------------
	// QRCode Cube 출력
	for (int i = 0; i < m_detectCubeCount; ++i)
	{
		GetMainCamera()->SetViewMatrix(m_qrMap[i]->m_tm);
		renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_qrMap[i]->m_tm);
		const Vector3 textPos = GetMainCamera()->GetScreenPos(640, 480, Vector3(0, 0, 0));

		const int id = m_qrMap[i]->m_id;
		const DWORD color = (m_nearQRCodeId == id) ? D3DXCOLOR(1, 0, 0, 1) : D3DXCOLOR(1, 1, 0, 1);
		m_qrMap[i]->m_text->SetText((int)textPos.x, (int)textPos.y, common::format("%d", id));
		m_qrMap[i]->m_text->SetColor(color);

		m_qrMap[i]->Render(renderer);
	}

	
	//----------------------------------------------------------------------------------------------------
	// 카메라 중점에서 가장 가까운 QRCode 4점을 찾아서 출력한다.

	// 중점과 가장 가까운 QRCode를 찾아서, 가장 가까운 4점을 찾는다.
	int nearId = -1;
	float minLenght = 100000;
	Point camPos(640 / 2, 480 / 2);
	for (int i = 0; i < m_detectCubeCount; ++i)
	{
		const int id = m_qrMap[i]->m_id;
		const Point diff = m_codes[id].pos - camPos;
		const float len = norm(diff);
		if (minLenght > len)
		{
			minLenght = len;
			nearId = id;
		}
	}

	m_nearQRCodeId = nearId;

	const bool isFindQuad = ar::FindQuadrant(m_tableW, m_tableH, nearId, (const sQRPos*)m_codes, m_nearQuad);

	// m_nearQRQuad
	// 0 ---- 1
	// |         |
	// |         |
	// 2 ---- 3
	//
	if (isFindQuad)
	{
		Vector2 vList[] = {
			Vector2(m_codes[m_nearQuad[0]].pos.x, m_codes[m_nearQuad[0]].pos.y),
			Vector2(m_codes[m_nearQuad[1]].pos.x, m_codes[m_nearQuad[1]].pos.y),
			Vector2(m_codes[m_nearQuad[1]].pos.x, m_codes[m_nearQuad[1]].pos.y),
			Vector2(m_codes[m_nearQuad[3]].pos.x, m_codes[m_nearQuad[3]].pos.y),
			Vector2(m_codes[m_nearQuad[2]].pos.x, m_codes[m_nearQuad[2]].pos.y),
			Vector2(m_codes[m_nearQuad[3]].pos.x, m_codes[m_nearQuad[3]].pos.y),
			Vector2(m_codes[m_nearQuad[2]].pos.x, m_codes[m_nearQuad[2]].pos.y),
			Vector2(m_codes[m_nearQuad[0]].pos.x, m_codes[m_nearQuad[0]].pos.y),
		};
		m_line.m_color = D3DCOLOR_XRGB(255, 0, 0);
		m_line.Render(renderer, vList, 8);
	}


	// 카메라 위치 계산
	if (isFindQuad)
	{
		Point contour[4] = {
			Point(m_codes[m_nearQuad[0]].pos.x, m_codes[m_nearQuad[0]].pos.y),
			Point(m_codes[m_nearQuad[1]].pos.x, m_codes[m_nearQuad[1]].pos.y),
			Point(m_codes[m_nearQuad[2]].pos.x, m_codes[m_nearQuad[2]].pos.y),
			Point(m_codes[m_nearQuad[3]].pos.x, m_codes[m_nearQuad[3]].pos.y),
		};

		// rect contour
		// 0 ------ 1
		// |			 |
		// |			 |
		// 3 ------ 2
		m_quad.Init(contour);
		m_homograpyMat = SkewTransform(m_quad.m_contours, 400, 400, 1.f);
	}

}
