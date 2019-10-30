//
// 2019-10-26, jjuiddong
// Visual Programming Node Pin
//
#pragma once


namespace framework
{
	namespace vprog
	{
		namespace ed = ax::NodeEditor;
		class cNode;

		struct sPin
		{
			ed::PinId id;
			cNode* node;
			StrId name;
			ePinType::Enum type;
			ePinKind::Enum kind;

			sPin(int id0, const StrId &name0, ePinType::Enum type0)
				: id(id0), node(nullptr), name(name0), type(type0)
				, kind(ePinKind::Input) {
			}
		};

	}
}
