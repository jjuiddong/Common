#pragma once

namespace motion
{

	// 입출력 미디어 타입
	namespace MOTION_MEDIA 
	{
		enum TYPE 
		{
			UDP = 0,
			TCP,
			SERIAL,
			JOYSTICK, // only input
			MWAVE, // only input
			SHARED_MEM,
		};
	}

	
	namespace MODULE_STATE
	{
		enum TYPE
		{
			STOP,
			START,
		};
	};

}
