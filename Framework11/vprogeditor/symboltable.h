//
// 2019-10-29, jjuiddong
// visual programming editor symboltable
//
#pragma once


namespace vprog
{
	namespace ed = ax::NodeEditor;

	class cSymbolTable
	{
	public:
		enum class eType { None, Enum, };

		struct sEnum {
			string name;
			int value;
		};
		struct sSymbol {
			eType type;
			string name;
			vector<sEnum> enums;
		};
		struct sValue {
			string type; // type name (not used)
			string str; // string data
			variant_t var; // binary or string data

			sValue();
			~sValue();
			sValue(const sValue &rhs);
			sValue& operator=(const sValue &rhs);
		};

		cSymbolTable();
		virtual ~cSymbolTable();

		// variable
		bool AddVarStr(const sPin &pin, const string &value, const string &type="");
		bool AddVar(const sPin &pin, const variant_t &value, const string &type = "");
		sValue* FindVar(const ed::PinId id);

		// symbol
		bool AddSymbol(const sSymbol &type);
		bool RemoveSymbol(const string &typeName);
		sSymbol* FindSymbol(const string &typeName);

		void Clear();

		cSymbolTable& operator=(const cSymbolTable &rhs);


	public:
		map<int, sValue> m_vars; // key:PinId
		map<string, sSymbol*> m_symbols; // key: symbol name
	};

}

