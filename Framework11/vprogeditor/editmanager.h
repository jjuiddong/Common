//
// 2019-10-26, jjuiddong
// Visual Programming Node Edit Manager
//
#pragma once


namespace framework
{
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
			sPin* FindPin(const ed::PinId id);
			bool IsPinLinked(const ed::PinId id);
			void BuildNodes();
			void Clear();

			// Generate Node Function
			cNode* Generate_InputActionNode();
			cNode* Generate_BranchNode();
			cNode* Generate_DoNNode();
			cNode* Generate_OutputActionNode();
			cNode* Generate_PrintStringNode();
			cNode* Generate_MessageNode();
			cNode* Generate_SetTimerNode();
			cNode* Generate_LessNode();
			cNode* Generate_WeirdNode();
			cNode* Generate_TraceByChannelNode();
			cNode* Generate_TreeSequenceNode();
			cNode* Generate_TreeTaskNode();
			cNode* Generate_TreeTask2Node();
			cNode* Generate_Comment();
			cNode* Generate_ReservedDefinition(const StrId &name);

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
}
