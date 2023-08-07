//
// 2019-10-29, jjuiddong
// interpreter global definition
//
#pragma once


namespace common
{
	namespace script
	{
		class cEvent
		{
		public:
			cEvent();
			cEvent(const StrId &name
				, const map<string, variant_t> &vars = {}
				, const bool isUnique = false
				, const int vmId = -1);
			virtual ~cEvent();


		public:
			StrId m_name;
			map<string, variant_t> m_vars;
			map<string, map<string, vector<string>>> m_vars2; // special purpose variables
			map<string, vector<string>> m_vars3; // tricky code, string array
			bool m_isUnique; // unique event? if true, event queue has only one this event
			float m_delayTime; // delay time (seconds unit), if less than zero or zero, trigger event
			int m_vmId; // trigger virutal machine id, -1: all vm
		};

	}
}

