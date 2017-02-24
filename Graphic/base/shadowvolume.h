//
// 2017-02-05, jjuiddong
// Volume Shadow Mesh class
// Direct3D9 Sample - VolumeShadow Source code
//
#pragma once


namespace graphic
{
	class cShadowVolume
	{
	public:
		struct sVertex
		{
			Vector3 Position;
			Vector3 Normal;
			const static D3DVERTEXELEMENT9 Decl[3];
		};

		cShadowVolume();
		virtual ~cShadowVolume();

		bool Create(cRenderer &renderer, ID3DXMesh *mesh);
		bool Create(cRenderer &renderer, cVertexBuffer &vtxBuff, cIndexBuffer &idxBuff, const Matrix44 &tm=Matrix44::Identity);
		void Render(cRenderer &renderer);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);


	public:
		ID3DXMesh *m_mesh;
	};
}

