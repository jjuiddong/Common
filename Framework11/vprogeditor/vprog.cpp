
#include "stdafx.h"
#include "vprog.h"

using namespace framework;
using namespace vprog;


ImColor vprog::GetIconColor(ePinType::Enum type)
{
	switch (type)
	{
	default:
	case ePinType::Flow:     return ImColor(255, 255, 255);
	case ePinType::Bool:     return ImColor(220, 48, 48);
	case ePinType::Int:      return ImColor(68, 201, 156);
	case ePinType::Float:    return ImColor(147, 226, 74);
	case ePinType::String:   return ImColor(124, 21, 153);
	case ePinType::Object:   return ImColor(51, 150, 215);
	case ePinType::Function: return ImColor(218, 0, 183);
	case ePinType::Delegate: return ImColor(255, 48, 48);
	}
};


void vprog::DrawPinIcon(const sPin& pin, bool connected, int alpha)
{
	using ax::Widgets::IconType;
	const int PinIconSize = 24;

	IconType iconType;
	ImColor  color = GetIconColor(pin.type);
	color.Value.w = alpha / 255.0f;
	switch (pin.type)
	{
	case ePinType::Flow:     iconType = IconType::Flow;   break;
	case ePinType::Bool:     iconType = IconType::Circle; break;
	case ePinType::Int:      iconType = IconType::Circle; break;
	case ePinType::Float:    iconType = IconType::Circle; break;
	case ePinType::String:   iconType = IconType::Circle; break;
	case ePinType::Object:   iconType = IconType::Circle; break;
	case ePinType::Function: iconType = IconType::Circle; break;
	case ePinType::Delegate: iconType = IconType::Square; break;
	default:
		return;
	}

	ax::Widgets::Icon(ImVec2(PinIconSize, PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
}


bool vprog::CanCreateLink(vprog::sPin* a, vprog::sPin* b)
{
	if (!a || !b || a == b || a->kind == b->kind
		|| a->type != b->type || a->node == b->node)
		return false;
	return true;
}
