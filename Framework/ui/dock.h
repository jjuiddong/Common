//
// 2017-04-21, jjuiddong
// docking system define
//
#pragma once


namespace framework
{

	struct eDockState
	{
		enum Enum
		{
			DOCK,
			//FLOATING,
			VIRTUAL,
		};
	};

	struct eDockType
	{
		enum Enum
		{
			NONE, 
			TAB, LEFT, TOP, RIGHT, BOTTOM,
			LEFT_MOST, TOP_MOST, RIGHT_MOST, BOTTOM_MOST,
		};

		static Enum GetOpposite(const Enum &type);
		static bool IsOpposite(const Enum &type1, const Enum &type2);
	};

	struct eDockSizingType
	{
		enum Enum
		{
			NONE
			, VERTICAL
			, HORIZONTAL
			, TOPLEFT_BTTOMRIGHT
			, BOTTOMLEFT_TOPRIGHT
		};
	};

	struct eDockResize
	{
		enum Enum
		{
			RENDER_WINDOW, DOCK_WINDOW
		};
	};


	const float TITLEBAR_HEIGHT = 37.f;
	const float TITLEBAR_HEIGHT2 = TITLEBAR_HEIGHT + 3.f; // add frame gap

}
