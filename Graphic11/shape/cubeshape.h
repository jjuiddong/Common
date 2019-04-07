//
// 2107-08-04, jjuiddong
// Cube Shape
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCubeShape : public cShape
	{
	public:
		cCubeShape();
		virtual ~cCubeShape();

		bool Create1(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK);

		bool Create2(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK);

		bool Create2(cRenderer &renderer
			, const cColor colors[8]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR));

		//bool InitCube1(cRenderer &renderer
		//	, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
		//	, const cColor &color = cColor::BLACK);

		//bool InitCube2(cRenderer &renderer
		//	, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
		//	, const cColor &color = cColor::BLACK);

		void SetCube(cRenderer &renderer, const Vector3 vertices[8]
			, const cColor &color = cColor::BLACK);

		void SetCube(cRenderer &renderer, const Vector3 vertices[8]
			, const cColor colors[8] );

		virtual void Render(cRenderer &renderer) override;
		virtual void RenderInstancing(cRenderer &renderer, const int count) override;


	protected:
		bool CreateShape1(cRenderer &renderer, const Vector3 vertices[24]
			, const Vector3 normals[24], const Vector2 uvs[24], const WORD indices[36]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK);

		bool CreateShape2(cRenderer &renderer, const Vector3 vertices[8], const WORD indices[36]
			, const Vector2 uvs[36]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK);

		bool CreateShape2(cRenderer &renderer, const Vector3 vertices[8], const WORD indices[36]
			, const Vector2 uvs[36], const cColor colors[8]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR));

		void SetShape2(cRenderer &renderer, const Vector3 vertices[8]
			, const WORD indices[36], const cColor &color = cColor::BLACK);

		void SetShape2(cRenderer &renderer, const Vector3 vertices[8]
			, const WORD indices[36], const cColor colors[8] );


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};




	//-----------------------------------------------------------------------------
	//     4 --- 5
	//    / |   /|
	//   0 --- 1 |
	//   |  6--|-7
	//   | /   |/
	//   2 --- 3
	//
	static const Vector3 g_cubeVertices1[24] = {
		Vector3(-1.f, 1.f, -1.f),//0
		Vector3(1.f, -1.f, -1.f),//3
		Vector3(-1.f, -1.f, -1.f),//2
		Vector3(1.f, 1.f, -1.f),//1
		Vector3(-1.f, -1.f, -1.f),//2
		Vector3(1.f, -1.f, 1.f),// 7
		Vector3(-1.f, -1.f, 1.f),//6
		Vector3(1.f, -1.f, -1.f),//3
		Vector3(-1.f, -1.f, 1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(-1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, -1.f, 1.f),
		Vector3(-1.f, 1.f, -1.f),
		Vector3(-1.f, -1.f, -1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, 1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(1.f, -1.f, -1.f),
	};

	static const  Vector3 g_cubeNormals1[24] = {
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, 0.f, -1.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, -1.f, 0.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 0.f, 1.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(0.f, 1.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(-1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
	};
	static const Vector2 g_cubeUV1[24] = {
		Vector2(0.278796f, 0.399099f),
		Vector2(0.008789f, 0.598055f),
		Vector2(0.008789f, 0.399099f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.987252f, 0.399099f),
		Vector2(0.768028f, 0.598055f),
		Vector2(0.768028f, 0.399099f),
		Vector2(0.987252f, 0.598055f),
		Vector2(0.768028f, 0.399099f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.768028f, 0.598055f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.278796f, 0.399099f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.49802f, 0.198324f),
		Vector2(0.278796f, 0.399099f),
		Vector2(0.278796f, 0.198324f),
		Vector2(0.49802f, 0.399099f),
		Vector2(0.49802f, 0.79883f),
		Vector2(0.278796f, 0.598055f),
		Vector2(0.49802f, 0.598055f),
		Vector2(0.278796f, 0.79883f),
	};
	static const WORD g_cubeIndices1[36] = {
		0, 1, 2,
		1, 0, 3,
		4, 5, 6,
		5, 4, 7,
		8, 9, 10,
		9, 8, 11,
		12, 13, 14,
		13, 12, 15,
		16, 17, 18,
		17, 16, 19,
		20, 21, 22,
		21, 20, 23,
	};

	//-----------------------------------------------------------------------------
	//     4 --- 5
	//    / |   /|
	//   0 --- 1 |
	//   |  6--|-7
	//   | /   |/
	//   2 --- 3
	//
	// vertices
	static const Vector3 g_cubeVertices2[8] = {
		Vector3(-1,1,-1)
		, Vector3(1,1,-1)
		, Vector3(-1,-1,-1)
		, Vector3(1,-1,-1)
		, Vector3(-1,1, 1)
		, Vector3(1,1, 1)
		, Vector3(-1,-1,1)
		, Vector3(1,-1,1)
	};
	static const Vector3 g_cubeNormals2[6] = {
		Vector3(0,0,-1), // front
		Vector3(0,0,1), // back
		Vector3(0,1,0), // top
		Vector3(0,-1,0), // bottom
		Vector3(-1,0,0), // left
		Vector3(1,0,0) // right
	};

	static const Vector2 g_cubeUV2[36] = {
		// 032 013
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(0, 1),

		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),

		// 567 546
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(0, 1),

		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),

		// 410 451
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(0, 1),

		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),

		// 276 237
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(0, 1),

		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),

		// 426 402
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(0, 1),

		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),

		// 173 157
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(0, 1),

		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
	};

	static const WORD g_cubeIndices2[36] = {
		// front
		0, 3, 2,
		0 ,1, 3,
		// back
		5, 6, 7,
		5, 4, 6,
		// top
		4, 1, 0,
		4, 5, 1,
		// bottom
		2, 7, 6,
		2, 3, 7,
		// left
		4, 2, 6,
		4, 0, 2,
		// right
		1, 7, 3,
		1, 5, 7,
	};

}
