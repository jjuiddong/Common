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
	public:
		virtual cSession* New() override { return new cSession(); }
	};

	class cWebSessionFactory : public iSessionFactory
	{
	public:
		virtual cWebSession* New() override { return new cWebSession(); }
	};
}
