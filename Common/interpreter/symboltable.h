//
// 2019-10-29, jjuiddong
// interpreter symboltable
//
// 2021-02-21
//	- add array variable type
//	- reference https://codemuri.tistory.com/5
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

			cSymbolTable();
			cSymbolTable(const cSymbolTable &rhs);
			virtual ~cSymbolTable();

			// variable
			template <class T>
			bool Set(const string &scopeName, const string &symbolName, const T &var
				, const string &typeStr="");

			// vector<string> specialization
			bool Set(const string &scopeName, const string &symbolName
				, const vector<string> &var, const string &typeStr = "");

			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const vector<T> &var, const string &typeStr = "");

			template <class T, size_t N>
			bool Set(const string &scopeName, const string &symbolName, const T(&var)[N]
				, const string &typeStr = "");

			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const map<string,T> &var, const string &typeStr = "");

			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const map<string, vector<T>> &var, const string &typeStr = "");

			bool Set(const string &scopeName, const string &symbolName, const variant_t &var
				, const string &typeStr = "");

			bool InitArray(const string &scopeName, const string &symbolName
				, const string &typeStr );
			bool CopyArray(const string &scopeName, const string &symbolName
				, const variant_t &var);
			bool InitMap(const string &scopeName, const string &symbolName
				, const string &typeStr);
			bool CopyMap(const string &scopeName, const string &symbolName
				, const variant_t &var);

			template <class T>
			T Get(const string &scopeName, const string &symbolName);
			bool Get(const string &scopeName, const string &symbolName, OUT variant_t &out);
			
			sVariable* FindVarInfo(const string &scopeName, const string &symbolName);
			bool IsExist(const string &scopeName, const string &symbolName);
			bool RemoveVar(const string &scopeName, const string &symbolName);
			
			// symbol
			bool AddSymbol(const sSymbol &type);
			bool RemoveSymbol(const string &typeName);
			sSymbol* FindSymbol(const string &typeName);

			bool CopySymbols(const cSymbolTable &rhs);
			void Clear();
			cSymbolTable& operator=(const cSymbolTable &rhs);

			static int GenID();
			static string MakeScopeName(const string &name, const int id);
			static string MakeScopeName2(const string &name, const int id, const string &name2);
			static std::pair<string, int> ParseScopeName(const string &scopeName);
			static std::tuple<string, int, string> ParseScopeName2(const string &scopeName);


		public:
			map<string, map<string, sVariable>> m_vars; // key: scopeName
														// value: varName, value
			map<string, sSymbol*> m_symbols; // key: symbol name
			map<int, std::pair<string,string>> m_varMap; // key: variable id, value: scopeName, varName
														 // for array type

			static std::atomic_int s_genId;
		};


		//---------------------------------------------------------------------------------------
		// template function
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName, const string &symbolName
			, const T &var, const string &typeStr //= ""
			)
		{
			variant_t v = var;
			const bool r = Set(scopeName, symbolName, v, typeStr);
			common::clearvariant(v);
			return r;
		}

		// template specialization (string)
		template <>
		inline bool cSymbolTable::Set<string>(const string &scopeName, const string &symbolName
			, const string &var, const string &typeStr //= ""
			)
		{
			variant_t v;
			v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(common::str2wstr(var).c_str());

			// to avoid bstr memory move bug
			common::clearvariant(m_vars[scopeName][symbolName].var);
			m_vars[scopeName][symbolName].var = v;
			m_vars[scopeName][symbolName].type = typeStr;
			return true;
		}

		// vector<string> specialization
		inline bool cSymbolTable::Set(const string &scopeName
			, const string &symbolName
			, const vector<string> &var
			, const string &typeStr //= ""
		)
		{
			InitArray(scopeName, symbolName, typeStr);
			sVariable &variable = m_vars[scopeName][symbolName];
			variable.ReserveArray(var.size());
			for (auto &v : var)
				variable.PushArrayElement((bstr_t)v.c_str());
			return true;
		}

		// vector<T>
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName
			, const string &symbolName
			, const vector<T> &var
			, const string &typeStr //= ""
			)
		{
			InitArray(scopeName, symbolName, typeStr);
			sVariable &variable = m_vars[scopeName][symbolName];
			variable.ReserveArray(var.size());
			variable.ClearArray();
			for (auto &v : var)
				variable.PushArrayElement(v);
			return true;
		}

		// initialize array type
		template <class T, size_t N>
		inline bool cSymbolTable::Set(const string &scopeName
			, const string &symbolName
			, const T(&var)[N]
			, const string &typeStr //= ""
			)
		{			
			InitArray(scopeName, symbolName, typeStr);
			sVariable &variable = m_vars[scopeName][symbolName];
			variable.ReserveArray(N);
			variable.ClearArray();
			for (uint i = 0; i < N; ++i)
				variable.PushArrayElement(var[i]);
			return true;
		}

		// map<string,T>
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName
			, const string &symbolName
			, const map<string, T> &var
			, const string &typeStr //= ""
		)
		{
			InitMap(scopeName, symbolName, typeStr);
			sVariable &mapVar = m_vars[scopeName][symbolName];
			for (auto &kv : var)
				mapVar.SetMapValue(kv.first, kv.second);
			return true;
		}

		// map<string,T>, vector<T> specialization
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName
			, const string &symbolName
			, const map<string, vector<T>> &var
			, const string &typeStr //= ""
		)
		{
			InitMap(scopeName, symbolName, typeStr);

			sVariable &mapVar = m_vars[scopeName][symbolName];
			const string &subTypeStr = mapVar.GetMapValueTypeStr();

			for (auto &kv : var)
			{
				const string &key = kv.first;
				stringstream ss;
				ss << symbolName << "[" << key << "]";
				const string varName = ss.str();
				Set(scopeName, varName, kv.second, subTypeStr);
				sVariable &arVar = m_vars[scopeName][varName];
				mapVar.SetMapValue(key, arVar.var);
			}
			return true;
		}

		template <class T>
		inline T cSymbolTable::Get(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return variant_t(T(0));
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
