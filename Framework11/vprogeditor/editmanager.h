//
// 2019-10-26, jjuiddong
// Visual Programming Node Edit Manager
//
#pragma once


namespace vprog
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
		cNode* FindContainNode(const ed::PinId id);
		bool IsPinLinked(const ed::PinId id);
		void BuildNodes();
		bool ShowFlow(const ed::LinkId id);
		bool IsLoad();
		void Clear();

		cNode* Generate_ReservedDefinition(const StrId &name
			, const StrId &varName="");

		// Link Function
		bool AddLink(const ed::PinId from, const ed::PinId to);


	protected:
		bool EditOperation();
		bool RenderPopup(graphic::cRenderer &renderer);
		int GetUniqueId();


	public:
		ed::EditorContext* m_editor;
		vector<cNode> m_nodes;
		vector<sLink> m_links;
		vector<cNode> m_definitions; // definition function, operator
		cSymbolTable m_symbTable;
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
