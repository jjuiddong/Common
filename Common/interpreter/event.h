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
				, const map<StrId, variant_t> &vars = {});
			virtual ~cEvent();


		public:
			StrId m_name;
			map<StrId, variant_t> m_vars;
		};

	}
}

