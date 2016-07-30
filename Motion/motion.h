//
// 2016-03-28, jjuiddong
// 
// Motion 처리에 관련된 라이브러리들을 선언한다.
// 

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "../Common/common.h"
#include "../Network/network.h"
#include "../CamCommon/camcommon.h"
using namespace common;


// BITCON Serial Communication Protocol
namespace VITCON_SER
{
	enum TYPE
	{
		START = 1,
		STOP = 2,
		ORIGIN = 5,
		EMERGENCY = 6,
		SERVOON = 7,
		SERVOOFF = 8,
	};
}



// Motion
#include "mixingconfig.h"
#include "spline.h"
#include "modulator.h"
#include "motionwavemodulator.h"
#include "motionmonitorconfig.h"
#include "simplemodulator.h"
#include "motionwave.h"
#include "vitconmotionsim.h"
#include "vitconmotionsim2.h"
#include "vitconmotionsim3.h"
#include "joypad.h"


#include "script/script.h"
#include "MotionController.h"

#include "udpinput.h"
#include "normalmixer.h"
#include "serialoutput.h"
#include "udpoutput.h"
#include "shmeminput.h"
#include "simplemodule.h"
#include "amplitudemodule.h"
#include "motioncontroller2.h"
#include "joystickinput.h"
