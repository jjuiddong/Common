//
// 2016-06-13, jjuiddong
// image match script parser
//
//	2016-07-26
//		- script variable 추가
//		- symbol tree 추가
//
// id = id
// id:
//		id, id, id, {id = id}
//			id, id
//				id
//
// - assigned_list
// - assigend -> id = id | number
// - tree_list
// - tree_head -> id:
// - tree -> id, id, {id = id}
//
#pragma once

#include "imagematch.h"


namespace cvproc {
	namespace imagematch {

		class cParser2
		{
		public:
			cParser2();
			virtual ~cParser2();
			bool Read(const string &fileName);
			void Clear();
			string GetSymbol(const string &symbol, const bool recursive=false, const int index=-1);
			int GetSymbolType(const string &symbol);
			vector<string>* GetSymbol2(const string &symbol);
			bool SetSymbol(const string &key, const string data);
			bool SetSymbol(const string &key, const vector<string> &table);
			bool AddSymbolTable();
			bool RemoveSymbolTable();


		public:
			bool ScanLine();
			void UnScanLine();
			bool IsBlank(const char *str, const int size);
			char* passBlank(const char *str);
			const char* id();
			const char* number();
			char op();
			char comma();
			bool match(const char c);
			int assigned(const char *var);
			bool assigned_list(const sParseTree *node);
			void attr_list(sParseTree *current);
			int attrs(const string &str, OUT string &out);
			string parse_attrs_symbol(const string &values);
			void collectTree(const sParseTree *current, set<sParseTree*> &out) const;
			void removeTree(const sParseTree *current) const;
			//sParseTree* cloneTree(const sParseTree *current) const;
			sParseTree* tree(sParseTree *current, const int depth);


		public:
			sParseTree *m_treeRoot;
			sParseTree *m_execRoot;
			std::ifstream m_ifs;
			
			struct sSymbolData
			{
				int type; // 0:string, 1:vector<string>
				string str;
				vector<string> *table; // reference g_stringTable
			};

			vector<map<string, sSymbolData>> m_symbolTree;
			char *m_lineStr;
			char m_scanLine[256];
			char m_tmpBuffer[256];
			bool m_isReverseScan = false; // 한 라인을 다시 읽는다.
			bool m_isError = false;
			int m_lineNum = 0;
		};

	}
}
