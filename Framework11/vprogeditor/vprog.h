//
// 2019-10-26, jjuiddong
// visual programming editor
//		- ui definition
//
#pragma once

#include "pin.h"
#include "node.h"
#include "link.h"
#include "symboltable.h"
#include "nodefile.h"
#include "editmanager.h"


namespace vprog {

	ImColor GetIconColor(ePinType::Enum type);
 	void DrawPinIcon(const sPin& pin, bool connected, int alpha);
	bool CanCreateLink(sPin* a, sPin* b);

}

