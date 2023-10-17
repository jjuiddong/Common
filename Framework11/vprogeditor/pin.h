//
// 2019-10-26, jjuiddong
// Visual Programming Node Pin
//
#pragma once


namespace vpl
{
	namespace ed = ax::NodeEditor;
	class cNode;

	struct sPin
	{
		ed::PinId id;
		ed::NodeId nodeId;
		StrId name;
		StrId typeStr; // type name
		int value; // used definition type
		ePinType type;
		ePinKind kind;

		sPin(int id0 = 0, const StrId &name0 = ""
			, ePinType type0 = ePinType::Flow)
			: id(id0), nodeId(0), name(name0), type(type0)
			, kind(ePinKind::Input), value(0) {
		}
	};

}

