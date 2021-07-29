//
// 2019-10-26, jjuiddong
// visual programming editor
//		- ui definition
//
// 2021-07-27
//	- rename vprog -> vpl
//
#pragma once

#include "pin.h"
#include "node.h"
#include "link.h"
#include "nodefile.h"
#include "editmanager.h"


namespace vpl {

	ImColor GetIconColor(ePinType::Enum type);
 	void DrawPinIcon(const sPin& pin, bool connected, int alpha);
	bool CanCreateLink(sPin* a, sPin* b);

}

