//
// 2021-02-21, jjuiddong
// variable
//	- symboltable variable
//		- primitive variable : variant_t 
//		- array 
//
#pragma once


namespace common
{
	namespace script
	{
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
			variant_t& GetMapElement(const string &key);
			bool SetMapElement(const string &key, const variant_t &v);
			bool HasMapElement(const string &key);
			uint GetMapSize();

			sVariable& operator=(const sVariable &rhs);
			void Clear();
			void ClearArray();
			void ClearArrayMemory();
			void ClearMap();
			void ClearMapMemory();


			int id; // unique id
			string type; // special type string, Bool, Int, Float, String, Array, Map
			variant_t var; // value

			// array type
			int subType0; // array element type, map key type (variant_t::VARTYPE)
			int subType1; // map value type (variant_t::VARTYPE)
			uint arSize; // array size
			uint arCapacity; // array capacity
			variant_t *ar; // array

			// map type
			map<string, variant_t> *m; // map
		};

	}
}
