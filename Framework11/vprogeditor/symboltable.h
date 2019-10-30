//
// 2019-10-29, jjuiddong
// visual programming editor symboltable
//
#pragma once


namespace framework
{
	namespace vprog
	{
		namespace ed = ax::NodeEditor;

		class cSymbolTable
		{
		public:
			struct sValue {
				variant_t var;
				string str;
			};

			cSymbolTable();
			virtual ~cSymbolTable();

			bool AddSymbolStr(const sPin &pin, const string &value);
			bool AddSymbol(const sPin &pin, const variant_t &value);
			sValue* FindSymbol(const ed::PinId id);
			void Clear();


		public:
			map<int, sValue> m_symbols; // key:PinId
		};

	}
}
