//
// 2017-03-16, jjuiddong
// Renderable Object Interface
//
#pragma once

namespace graphic
{
	class cRenderer;

	interface iRenderable
	{
		virtual void Render(cRenderer &renderer, const int opt) = 0;
	};

}
