#pragma once

#include "../ui/window.h"


namespace framework
{

	class cScene : public cWindow
	{
	public:
		cScene(const int id, const string &scenName="scene");
		virtual ~cScene();

		virtual bool Init(graphic::cRenderer &renderer) override;
		virtual bool Update(const float deltaSeconds) override;
		virtual void Render(graphic::cRenderer &renderer, const Matrix44 &parentTm) override;
		virtual bool MessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;

		virtual void Show() {}
		virtual void Hide() {}


	public:
	};

}
