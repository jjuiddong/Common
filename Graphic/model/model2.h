//
// 2017-02-10, jjuiddong
//
#pragma once


namespace graphic
{

	class cModel2
	{
	public:
		cModel2();
		virtual ~cModel2();

		virtual bool Create(cRenderer &renderer, const string &modelName, MODEL_TYPE::TYPE type = MODEL_TYPE::AUTO, const bool isLoadShader = true);
		void Clear();


	public:
		cMesh2 *m_mesh;
	};

}
