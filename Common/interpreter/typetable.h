//
// 2019-11-20, jjuiddong
// type table for interpreter
//
#pragma once


namespace common
{
	namespace script
	{

		class cTypeTable
		{
		public:
			enum class eType { None, Enum, };

			struct sEnum {
				string name;
				int value;
			};
			struct sType {
				eType type;
				string name;
				vector<sEnum> enums;
			};

			cTypeTable();
			cTypeTable(const cTypeTable &rhs);
			virtual ~cTypeTable();

			bool AddType(const sType &type);
			bool RemoveType(const string &typeName);
			sType* FindType(const string &typeName);
			void Clear();

			cTypeTable& operator=(const cTypeTable &rhs);


		public:
			map<string, sType*> m_types; // key: typename
		};

	}
}
