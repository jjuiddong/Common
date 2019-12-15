//
// 2019-12-15, jjuiddong
// compute shader
//
#pragma once


namespace graphic
{

	class cComputeShader
	{
	public:
		cComputeShader();
		virtual ~cComputeShader();

		bool Create(cRenderer &renderer, const StrPath &fileName);
		void Bind(cRenderer &renderer);
		void Unbind(cRenderer &renderer);
		void Clear();


	public:
		ID3D11ComputeShader *m_shader; // compute shader
	};

}
