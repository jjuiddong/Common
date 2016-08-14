//
// 2016-05-31, jjuiddong
// 탭으로 구분된, 텍스트 문서를 분석해, 그래프 구조를 만든다.
//
//	item1
//		item2
//			item3
//			item4
//		item5
//
#pragma once



namespace cvproc {
	namespace imagematch {

		class cGraphScript
		{
		public:
			cGraphScript();
			virtual ~cGraphScript();
			
			struct sNode
			{
				string id;
				string tag; // 공통 label_id 처리
				int key; // keyboard
				bool check; // use traverse node
				bool noUpperTraverse; // use find route
				bool noProc; // use traverse node, no match scene
				bool isAuto; // use traverse node, no keyboard enter menu
				bool isSideMenu; // use traverse node, side menu operation
				bool isEnterChild; // use traverse node, enter to next child Scene
				bool isNoMenu; // use traverse node, does not show menu item
				bool isSideSubmenu;// use traverse node, left, right sub menu, 자식에게 영향을 미친다.
				bool isSceneIdInherit;
				bool isSceneIdChildInherit;
				bool isCircularMenu;
				int sceneId;// use traverse node
				float delay; // use move scene delay time seconds
				vector<sNode*> out;
				vector<sNode*> in;
			};

			bool Read(const string &fileName);
			sNode* Find(const string &id);
			sNode* FindHead(const string &id);
			bool FindRoute(const string &from, const string &to, OUT vector<sNode*> &out);
			bool FindRoute(sNode*current, const string &to, OUT vector<sNode*> &out);
			void PrintGraph(const string &rootName);
			void CheckClearAllNode();
			void Clear();


		protected:
			sNode* build(sParseTree *parent, sParseTree *current, sNode *parentNode);
			sNode* FindParent(sNode *current, const string &id);
			sNode* FindParentRec(sNode *current, const string &id);
			bool FindRouteRec(sNode*current, const string &id, OUT vector<sNode*> &out);
			void buildAttributes(const sParseTree *node, const string &str, vector<string> &attributes);
			void setTreeAttribute(sParseTree *node, vector<string> &attribs);


		public:
			cParser2 m_parser;
			sNode *m_root;
			vector<sNode*> m_nodes;
			vector<sNode*> m_heads; // head node 저장, reference
			int m_sceneIdGen; // from 1 to N
		};

	}
}
