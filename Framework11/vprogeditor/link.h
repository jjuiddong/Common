//
// 2019-10-26, jjuiddong
// Visual Programming Node Link
//
#pragma once


namespace vprog
{
	namespace ed = ax::NodeEditor;

	struct sLink
	{
		ed::LinkId id;
		ed::PinId fromId;
		ed::PinId toId;
		ImColor color;

		sLink(ed::LinkId id0, ed::PinId fromPinId, ed::PinId toPinId) :
			id(id0), fromId(fromPinId), toId(toPinId)
			, color(255, 255, 255) {
		}
	};
}

