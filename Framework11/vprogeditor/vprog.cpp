
#include "stdafx.h"
#include "vprog.h"

using namespace framework;
using namespace framework::vprog;


ImColor vprog::GetIconColor(PinType::Enum type)
{
	switch (type)
	{
	default:
	case PinType::Flow:     return ImColor(255, 255, 255);
	case PinType::Bool:     return ImColor(220, 48, 48);
	case PinType::Int:      return ImColor(68, 201, 156);
	case PinType::Float:    return ImColor(147, 226, 74);
	case PinType::String:   return ImColor(124, 21, 153);
	case PinType::Object:   return ImColor(51, 150, 215);
	case PinType::Function: return ImColor(218, 0, 183);
	case PinType::Delegate: return ImColor(255, 48, 48);
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
	case PinType::Flow:     iconType = IconType::Flow;   break;
	case PinType::Bool:     iconType = IconType::Circle; break;
	case PinType::Int:      iconType = IconType::Circle; break;
	case PinType::Float:    iconType = IconType::Circle; break;
	case PinType::String:   iconType = IconType::Circle; break;
	case PinType::Object:   iconType = IconType::Circle; break;
	case PinType::Function: iconType = IconType::Circle; break;
	case PinType::Delegate: iconType = IconType::Square; break;
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
