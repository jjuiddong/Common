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
			DOCKWINDOW,
			VIRTUAL,
		};
	};

	struct eDockSlot
	{
		enum Enum
		{
			NONE, 
			TAB, LEFT, TOP, RIGHT, BOTTOM,
			LEFT_MOST, TOP_MOST, RIGHT_MOST, BOTTOM_MOST,
		};

		static Enum GetOpposite(const Enum &type)
		{
			switch (type)
			{
			case TAB: return TAB;
			case LEFT: return RIGHT;
			case TOP:  return BOTTOM;
			case RIGHT: return LEFT;
			case BOTTOM: return TOP;
			case LEFT_MOST: return RIGHT_MOST;
			case TOP_MOST: return BOTTOM_MOST;
			case RIGHT_MOST: return LEFT_MOST;
			case BOTTOM_MOST: return TOP_MOST;
			default: assert(0);
			}
			return TAB;
		}

		static bool IsOpposite(const Enum &type1, const Enum &type2)
		{
			return (GetOpposite(type1) == type2);
		}
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

	struct eDockSizingOption {
		enum Enum {
			RATE
			, PIXEL
		};
	};



	const float TITLEBAR_HEIGHT = 37.f;
	const float TITLEBAR_HEIGHT2 = TITLEBAR_HEIGHT + 3.f; // add frame gap
	const float TAB_H = 32;
}
