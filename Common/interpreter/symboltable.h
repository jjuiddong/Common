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

			// set variable
			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const T &var, const string &typeStr="");

			// variant_t overloading
			bool Set(const string &scopeName, const string &symbolName
				, const variant_t &var, const string &typeStr = "");

			// vector<T> specialization
			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const vector<T> &var, const string &typeStr = "");

			// T[N] specialization
			template <class T, size_t N>
			bool Set(const string &scopeName, const string &symbolName
				, const T(&var)[N], const string &typeStr = "");

			// T* specialization
			template <class T>
			bool Set(const string& scopeName, const string& symbolName
				, const T* var, const uint size, const string& typeStr = "");

			// map<string,T> specialization
			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const map<string,T> &var, const string &typeStr = "");

			// map<string,vector<T>> specialization
			template <class T>
			bool Set(const string &scopeName, const string &symbolName
				, const map<string, vector<T>> &var, const string &typeStr = "");

			bool InitArray(const string &scopeName, const string &symbolName
				, const string &typeStr );
			bool CopyArray(const string &scopeName, const string &symbolName
				, const variant_t &var);
			bool ArrayInitializer(const string& scopeName, const string& symbolName
				, const string& initStr);
			bool InitMap(const string &scopeName, const string &symbolName
				, const string &typeStr);
			bool CopyMap(const string &scopeName, const string &symbolName
				, const variant_t &var);

			template <class T>
			T Get(const string &scopeName, const string &symbolName);

			// variant_t overloading
			bool Get(const string &scopeName, const string &symbolName
				, OUT variant_t &out);

			bool GetString(const string& scopeName, const string& symbolName
				, OUT string& out, const bool isAddQuotes = false);
			
			sVariable* FindVarInfo(const string &scopeName, const string &symbolName);
			sVariable* FindRealVarInfo(const string& scopeName, const string& symbolName);
			bool IsExist(const string &scopeName, const string &symbolName);
			bool RemoveVar(const string &scopeName, const string &symbolName);
			bool SetVarFlag(const string& scopeName, const string& symbolName, const int flags);
			
			// symbol (enum)
			bool AddSymbol(const sSymbol &type);
			bool RemoveSymbol(const string &typeName);
			sSymbol* FindSymbol(const string &typeName);

			bool CopySymbols(const cSymbolTable &rhs);
			void SyncChange() { m_isChange = false; }
			void Clear();
			cSymbolTable& operator=(const cSymbolTable &rhs);

			static int GenID();
			static string MakeScopeName(const string &name, const int id);
			static string MakeScopeName2(const string &name, const int id, const string &name2);
			static string MakeScopeName3(const string &scopeName, const string &varName);
			static std::pair<string, int> ParseScopeName(const string &scopeName);
			static std::tuple<string, int, string> ParseScopeName2(const string &scopeName);


		public:
			map<string, map<string, sVariable>> m_vars; // key: scopeName
														// value: varName, value
			map<string, sSymbol*> m_symbols; // key: symbol name
			map<int, std::pair<string,string>> m_varMap; // key: variable id, value: scopeName, varName
														 // for array type

			bool m_isChange; // add variable?
			static std::atomic_int s_genId;
		};


		//--------------------------------------------------------------------------------
		// set symboltable variable
		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName
			, const string &symbolName
			, const T &var
			, const string &typeStr //= ""
			)
		{
			variant_t v = var;
			const bool r = Set(scopeName, symbolName, v, typeStr);
			common::clearvariant(v);
			return r;
		}

		// template specialization (string)
		template <>
		inline bool cSymbolTable::Set<string>(const string &scopeName
			, const string &symbolName
			, const string &var
			, const string &typeStr //= ""
			)
		{
			variant_t v;
			v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(common::str2wstr(var).c_str());

			if (!IsExist(scopeName, symbolName))
				m_isChange = true;

			// to avoid bstr memory move bug
			common::clearvariant(m_vars[scopeName][symbolName].var);
			m_vars[scopeName][symbolName].var = v;
			m_vars[scopeName][symbolName].type = typeStr;
			return true;
		}

		// vector<string> specialization
		template <>
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

		// vector<T> specialization
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

		// T[N] specialization
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

		// T* specialization
		template <class T>
		inline bool cSymbolTable::Set(const string& scopeName
			, const string& symbolName
			, const T* var
			, const uint size
			, const string& typeStr //= ""
		)
		{
			InitArray(scopeName, symbolName, typeStr);
			sVariable& variable = m_vars[scopeName][symbolName];
			variable.ReserveArray(size);
			variable.ClearArray();
			for (uint i = 0; i < size; ++i)
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

		//--------------------------------------------------------------------------------
		// get symboltable variable
		template <class T>
		inline vector<T> GetVector(cSymbolTable &symbolTable
			, const string &scopeName, const string &symbolName)
		{
			vector<T> ar;
			sVariable *arVar = symbolTable.FindVarInfo(scopeName, symbolName);
			if (!arVar)
				return ar; // not found variable

			// find array source variable
			const int arId = arVar->var.intVal;
			auto it = symbolTable.m_varMap.find(arId);
			if (symbolTable.m_varMap.end() == it)
				return ar; // error, not found source value
			if ((it->second.first != scopeName) || (it->second.second != symbolName))
				arVar = symbolTable.FindVarInfo(it->second.first, it->second.second);
			if (!arVar || !arVar->ar)
				return ar; // not found variable or no map instance
			//

			if ((arVar->type != "Array") && (arVar->type != "array"))
				return ar; // no array type
			for (uint i = 0; i < arVar->GetArraySize(); ++i)
			{
				variant_t v = arVar->GetArrayElement(i);
				ar.push_back((T)v);
			}
			return ar;
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
		inline string cSymbolTable::Get<string>(const string &scopeName
			, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return "";
			return WStr128(var.bstrVal).str().c_str();
		}

		// vector<string> template specialization 
		template <>
		inline vector<string> cSymbolTable::Get<vector<string>>(const string &scopeName
			, const string &symbolName)
		{
			vector<string> ar;
			sVariable *arVar = FindVarInfo(scopeName, symbolName);
			if (!arVar)
				return ar; // not found variable

			// find array source variable
			const int arId = arVar->var.intVal;
			auto it = m_varMap.find(arId);
			if (m_varMap.end() == it)
				return ar; // error, not found source value
			if ((it->second.first != scopeName) || (it->second.second != symbolName))
				arVar = FindVarInfo(it->second.first, it->second.second);
			if (!arVar || !arVar->ar)
				return ar; // not found variable or no map instance
			//

			if ((arVar->type != "Array") && (arVar->type != "array"))
				return ar; // no array type
			for (uint i = 0; i < arVar->GetArraySize(); ++i)
			{
				variant_t v = arVar->GetArrayElement(i);
				ar.push_back(common::variant2str(v));
			}
			return ar;
		}

		// vector<float> template specialization 
		template <>
		inline vector<float> cSymbolTable::Get<vector<float>>(const string &scopeName, const string &symbolName)
		{
			return GetVector<float>(*this, scopeName, symbolName);
		}
		// vector<bool> template specialization 
		template <>
		inline vector<bool> cSymbolTable::Get<vector<bool>>(const string &scopeName, const string &symbolName)
		{
			return GetVector<bool>(*this, scopeName, symbolName);
		}

		// map<string,vector<string>> template specialization 
		template <>
		inline map<string,vector<string>> cSymbolTable::Get<map<string,vector<string>>>
			(const string &scopeName, const string &symbolName)
		{
			map<string, vector<string>> ret;
			sVariable *srcVar = FindVarInfo(scopeName, symbolName);
			if (!srcVar)
				return ret; // not found variable

			// find map source variable
			const int mapId = srcVar->var.intVal;
			auto it = m_varMap.find(mapId);
			if (m_varMap.end() == it)
				return ret; // error, not found source value
			if ((it->second.first != scopeName) || (it->second.second != symbolName))
				srcVar = FindVarInfo(it->second.first, it->second.second);
			if (!srcVar || !srcVar->m)
				return ret; // not found variable or no map instance
			//

			// traverse map variable
			for (auto &kv : *srcVar->m)
			{
				// traverse array variable
				// val: array<string>
				variant_t val = srcVar->GetMapValue(*this, kv.first);
				if (!(val.vt & (VT_BYREF | VT_INT)))
					continue; // error, no array type

				const int varId = val.intVal;
				auto it2 = m_varMap.find(varId);
				if (m_varMap.end() == it2)
					continue; // error, not found array variable

				vector<string> strAr = Get<vector<string>>(
					it2->second.first, it2->second.second);
				ret[kv.first] = strAr;
			}
			return ret;
		}

	}
}
