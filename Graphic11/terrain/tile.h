//
// 2017-05-13, jjuiddong
// terrain tile
//
// 2017-08-11
//	- Upgrade DX11
//
//
// rect =  3D-axis
//
//  z (rect.top, bottom)
//  |
//  |
//  |
//  + ---------> x (rect.left, right)
// 
//
//   rect.left, top (X-Z axis)  
//   * ---------------------------------- * rect.right, top (X-Z axis)
//   |                                    |
//   |                                    |
//   |                 +                  |
//   |               center               |
//   |                                    |
//   * ---------------------------------- * rect.right, bottom (X-Z axis)
//   rect.left, bottom (X-Z axis)
//
#pragma once


namespace graphic
{

	class cTile : public cNode2
	{
	public:
		cTile();
		virtual ~cTile();

		virtual bool Create(cRenderer &renderer
			, const int id
			, const Str64 &name
			, const int row
			, const int col
			, const sRectf &rect
			, const char *textureFileName = g_defaultTexture
			, const float uvFactor=1.f
			, const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1)
			);

		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual void PreRender(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;
		virtual void DebugRender(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);

		virtual float CullingTest(const cFrustum &frustum, const XMMATRIX &tm = XMIdentity, const bool isModel = true);
		virtual bool AddChild(cNode2 *node) override;
		virtual bool RemoveChild(cNode2 *rmNode, const bool rmInstance = true) override;
		virtual const cNode2* FindNode(const int id) const override;
		void UpdatePosition(const sRectf &rect);
		virtual void Clear();


	public:
		Vector2i m_location; // Tile Location row, col
		cGrid *m_ground;
		bool m_isHilight;
		bool m_isDbgRender;	
	};

}
