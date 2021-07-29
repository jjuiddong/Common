//
// 2019-10-26, jjuiddong
// Visual Programming Node Edit Manager
//
#pragma once


namespace vpl
{
	namespace ed = ax::NodeEditor;

	class cEditManager
	{
	public:
		cEditManager();
		virtual ~cEditManager();

		bool Init(graphic::cRenderer &renderer);
		bool Read(const StrPath &fileName);
		bool ReadDefinitionFile(const StrPath &fileName);
		bool Write(const StrPath &fileName);
		bool Render(graphic::cRenderer &renderer);

		cNode* FindNode(const ed::NodeId id);
		sLink* FindLink(const ed::LinkId id);
		sLink* FindLink(const ed::PinId from, const ed::PinId to);
		sPin* FindPin(const ed::PinId id);
		sPin* FindPin(const string& scopeName, const string pinName);
		cNode* FindContainNode(const ed::PinId id);
		bool IsPinLinked(const ed::PinId id);
		void BuildNodes();
		bool ShowFlow(const ed::LinkId id);
		int GetUniqueId();
		bool IsLoad();
		void Clear();

		cNode* Generate_ReservedDefinition(const StrId &name
			, const StrId &desc = "");

		// Link Function
		bool AddLink(const ed::PinId from, const ed::PinId to);

		// symbol table
		common::script::sVariable* FindVarInfo(const ed::PinId id);
		bool AddTemporalVar(const ed::PinId id);
		string GetScopeName(const ed::PinId id);


	protected:
		bool EditOperation();
		bool Proc_NewLink();
		bool RenderPopup(graphic::cRenderer &renderer);


	public:
		ed::EditorContext* m_editor;
		vector<cNode> m_nodes;
		vector<sLink> m_links;
		vector<cNode> m_definitions; // definition function, operator
		common::script::cSymbolTable m_symbTable;
		StrPath m_fileName;
		graphic::cTexture *m_headerBackground;

		// edit variable
		ed::NodeId m_contextNodeId = 0;
		ed::LinkId m_contextLinkId = 0;
		ed::PinId  m_contextPinId = 0;
		bool m_isCreateNewNode = false;
		sPin* m_newNodeLinkPin = nullptr;
		sPin* m_newLinkPin = nullptr;
	};

}
