//
// 2019-10-26, jjuiddong
// visual programming node ui module
//
#pragma once


namespace vprog
{
	namespace ed = ax::NodeEditor;
	namespace util = ax::NodeEditor::Utilities;

	class cEditManager;

	class cNode
	{
	public:
		cNode(int id0, const StrId &name, ImColor color0 = ImColor(255, 255, 255));
		virtual ~cNode();

		bool Render(cEditManager &editMgr
			, util::BlueprintNodeBuilder &builder
			, sPin* newLinkPin = nullptr);

		void Clear();


	protected:
		bool RenderSetInputPin(cEditManager &editMgr
			, util::BlueprintNodeBuilder &builder
			, sPin &pin
			, sPin* newLinkPin = nullptr);

		bool RenderStringPin(cEditManager &editMgr
			, util::BlueprintNodeBuilder &builder
			, sPin &pin
			, sPin* newLinkPin = nullptr);

		bool RenderEnumPin(cEditManager &editMgr
			, util::BlueprintNodeBuilder &builder
			, sPin &pin
			, sPin* newLinkPin = nullptr);

		bool RenderSwitchCaseNode(cEditManager &editMgr
			, util::BlueprintNodeBuilder &builder
			, sPin* newLinkPin = nullptr);


	public:
		ed::NodeId m_id;
		eNodeType::Enum m_type;
		StrId m_name;
		StrId m_desc; // description
		ImColor m_color;
		ImVec2 m_pos; // use only initialize 
		ImVec2 m_size;
		vector<sPin> m_inputs;
		vector<sPin> m_outputs;
	};

}

