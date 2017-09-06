//
// 2107-08-04, jjuiddong
// Cube Shape
//
#pragma once


namespace graphic
{

	class cCubeShape : public cShape
	{
	public:
		cCubeShape();
		virtual ~cCubeShape();

		bool Create1(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		bool Create2(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		bool InitCube1(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		bool InitCube2(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		void SetCube(cRenderer &renderer, const Vector3 vertices[8]
			, const cColor &color = cColor::BLACK);

		void Render(cRenderer &renderer) override;


	protected:
		bool CreateShape1(cRenderer &renderer, const Vector3 vertices[24]
			, const Vector3 normals[24], const Vector2 uvs[24], const WORD indices[36]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		bool CreateShape2(cRenderer &renderer, const Vector3 vertices[8], const WORD indices[36]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		void SetShape2(cRenderer &renderer, const Vector3 vertices[8]
			, const WORD indices[36], const cColor &color = cColor::BLACK);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}
