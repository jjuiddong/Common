//
// 2020-01-18, jjuiddong
// physics engine utility header
//
#pragma once


namespace phys
{

	struct sPvdParameters
	{
		string ip;
		int port;
		int timeout;
		bool useFullPvdConnection;
		sPvdParameters()
			: port(5425)
			, timeout(10)
			, useFullPvdConnection(true)
			, ip("127.0.0.1")
		{
		}
	};

}
