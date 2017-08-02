//
// 2017-07-28, jjuiddong
// DX11 Cube Class
// Vertex = Position + Normal + Diffuse + TextureUV
// VertexBuffer + IndexBuffer
//
#pragma once


namespace graphic
{

	struct eCubeType {
		enum Enum {
			POSITION = 1 << 1
			, NORMAL = 1 << 2
			, DIFFUSE = 1 << 3
			, TEXTURE = 1 << 4
		};
	};

	class cRenderer;
	class cCube : public cNode2
	{
	public:
		cCube();
		cCube(cRenderer &renderer, const cBoundingBox &bbox
			, const int cubeType = (eCubeType::POSITION | eCubeType::NORMAL | eCubeType::DIFFUSE)
			, const cColor &color = cColor::WHITE);

		bool Create(cRenderer &renderer, const cBoundingBox &bbox
			, const int cubeType = (eCubeType::POSITION | eCubeType::NORMAL | eCubeType::DIFFUSE)
			, const cColor &color = cColor::WHITE);
		void InitCube(cRenderer &renderer, const int cubeType, const cColor &color = cColor::WHITE);
		void SetCube(const cBoundingBox &bbox);
		void SetCube(cRenderer &renderer, const cBoundingBox &bbox);
		void SetCube(cRenderer &renderer, const cCube &cube);
		void SetColor( const cColor &color );
		//const float Length() const; // length(min - max)

		//void RenderSolid(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cTexture *m_texture; // reference
		int m_cubeType; // eCubeType::Enum
	};	

	//inline const float cCube::Length() const { return m_boundingBox.Length(); }
}
