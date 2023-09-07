//
// 2019-10-29, jjuiddong
// interpreter global definition
//
#pragma once


namespace common
{
	namespace script
	{

		// event option flag enumeration
		enum class eEventFlags 
		{
			None		= 0x0000,
			Unique		= 0x0001, // check duplicate, event queue has only one this event
			SyncFlow	= 0x0002, // remote interpreter synchronize flow animation
		};

		class cVirtualMachine;

		class cEvent
		{
		public:
			cEvent();
			cEvent(const StrId& name
				, const map<string, variant_t>& vars = {}
				, const int flags = 0 // composite of eEventFlags
			);
			virtual ~cEvent();

			virtual bool ProcessVM(cVirtualMachine &vm);


		public:
			StrId m_name;
			map<string, variant_t> m_vars;
			map<string, map<string, vector<string>>> m_vars2; // special purpose variables
			map<string, vector<string>> m_vars3; // tricky code, string array
			float m_delayTime; // delay time (seconds unit), if less than zero or zero, trigger event
			int m_vmId; // trigger virutal machine id, -1: all vm
			uint m_flags; // composite of eEventFlags
		};

	}
}

