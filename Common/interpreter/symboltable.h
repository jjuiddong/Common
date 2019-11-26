//
// 2019-10-29, jjuiddong
// interpreter symboltable
//
#pragma once


namespace common
{
	namespace script
	{

		class cSymbolTable
		{
		public:
			enum class eType { None, Enum, };

			// symbol information
			struct sEnum {
				string name;
				int value;
			};
			struct sSymbol {
				eType type;
				string name;
				vector<sEnum> enums;
			};

			// variable information
			struct sVar {
				string type;
				variant_t var;

				sVar();
				sVar(const sVar &rhs);
				~sVar();
				sVar& operator=(const sVar &rhs);
			};


			cSymbolTable();
			cSymbolTable(const cSymbolTable &rhs);
			virtual ~cSymbolTable();

			// variable
			template <class T>
			bool Set(const string &scopeName, const string &symbolName, const T &var);
			bool Set(const string &scopeName, const string &symbolName, const variant_t &var);
			template <class T>
			T Get(const string &scopeName, const string &symbolName);
			bool Get(const string &scopeName, const string &symbolName, OUT variant_t &out);
			bool IsExist(const string &scopeName, const string &symbolName);

			// symbol
			bool AddSymbol(const sSymbol &type);
			bool RemoveSymbol(const string &typeName);
			sSymbol* FindSymbol(const string &typeName);

			void Clear();

			static string MakeScopeName(const string &name, const int id);
			static std::pair<string,int> ParseScopeName(const string &scopeName);

			cSymbolTable& operator=(const cSymbolTable &rhs);


		public:
			map<string, map<string, sVar>> m_vars; // key: scopeName
												   // value: varName, value
			map<string, sSymbol*> m_symbols; // key: symbol name
		};


		//---------------------------------------------------------------------------------------
		// template function
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName, const string &symbolName, const T &var)
		{
			variant_t v((T)var);
			const bool r = Set(scopeName, symbolName, v);
			common::clearvariant(v);
			return r;
		}

		// template specialization (string)
		template <>
		inline bool cSymbolTable::Set<string>(const string &scopeName, const string &symbolName
			, const string &var)
		{
			variant_t v;
			v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(common::str2wstr(var).c_str());

			// to avoid bstr memory move bug
			common::clearvariant(m_vars[scopeName][symbolName].var);
			m_vars[scopeName][symbolName].var = v;
			return true;
		}

		template <class T>
		inline T cSymbolTable::Get(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return variant_t((T)0);
			return (T)var;
		}

		// template specialization (string)
		template <>
		inline string cSymbolTable::Get<string>(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return "";
			return WStr128(var.bstrVal).str().c_str();
		}

	}
}
