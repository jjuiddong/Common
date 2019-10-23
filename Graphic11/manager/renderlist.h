//
// 2019-09-18, jjuiddong
// model render list
//	  - simple version
//    - model instancing render manager
//
#pragma once


namespace graphic
{

	class cRenderList
	{
	public:
		struct sRenderInstancing {
			StrId name; // model name
			cModel *model; // reference
			int flags;
			vector<Matrix44> tms;
		};

		cRenderList();
		virtual ~cRenderList();

		void Render(cRenderer &renderer);
		bool AddRender(cModel *model, const Matrix44 &transformTm = Matrix44::Identity
			, const int flags = 1);
		void Clear();


	public:
		map<StrId, sRenderInstancing*> m_renderList;
	};

}
