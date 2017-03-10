//
// 2017-03-07, jjuiddong
// shader rendering interface
//
#pragma once

namespace graphic
{

	class cShader;
	class cRenderer;

	interface cShaderRenderer
	{
	public:
		cShaderRenderer() : m_shader(NULL) {}
		virtual ~cShaderRenderer() {}

		void SetShader(cShader *shader) { m_shader = shader;  }
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) = 0;


	public:
		cShader *m_shader;
	};

}
