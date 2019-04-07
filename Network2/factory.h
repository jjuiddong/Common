//
// 2019-01-08, jjuiddong
// factory 
//	- session factory
//
#pragma once


namespace network2
{

	interface iSessionFactory
	{
		virtual cSession* New() = 0;
	};

	class cSessionFactory : public iSessionFactory
	{
		virtual cSession* New() override { return new cSession(); }
	};

}
