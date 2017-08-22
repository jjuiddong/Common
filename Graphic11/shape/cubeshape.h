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
		cCubeShape(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::WHITE);

		virtual ~cCubeShape();

		bool Create(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::WHITE);


		void Create(cRenderer &renderer, Vector3 vertices[8]
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		void InitCube(cRenderer &renderer, const int cubeType, const cColor &color = cColor::WHITE);

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}
