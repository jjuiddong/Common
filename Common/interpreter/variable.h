//
// 2021-02-21, jjuiddong
// variable
//	- symboltable variable
//		- primitive variable : variant_t 
//		- array 
//
// 2021-05-20
//	- add map type
//	- add enum symboltype
//
#pragma once


namespace common
{
	namespace script
	{
		class cSymbolTable;

		struct sVariable
		{
			sVariable();
			sVariable(const sVariable &rhs);
			~sVariable();

			variant_t& GetArrayElement(const uint index);
			bool SetArrayElement(const uint index, const variant_t &v);
			bool PushArrayElement(const variant_t &v);
			variant_t& PopArrayElement();
			bool ReserveArray(const uint size);
			uint GetArraySize();
			
			variant_t& GetMapValue(cSymbolTable &symbolTable, const string &key);
			bool SetMapValue(const string &key, const variant_t &v);
			bool HasMapValue(const string &key);
			uint GetMapSize();
			const string& GetMapValueTypeStr();
			const string& GetArrayElementTypeStr();

			sVariable& operator=(const sVariable &rhs);
			void Clear();
			void ClearArray();
			void ClearArrayMemory();
			void ClearMap();
			void ClearMapMemory();


			int id; // unique id
			string type; // special type string, Bool, Int, Float, String, Array, Map
			string subTypeStr; // array element, map value type string
			eSymbolType::Enum typeValues[4]; // maximum: map<string, vector<primitive type>>
			variant_t var; // value

			// array type
			uint arSize; // array size
			uint arCapacity; // array capacity
			variant_t *ar; // array

			// map type
			map<string, variant_t> *m;
		};

	}
}
