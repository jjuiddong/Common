//------------------------------------------------------------------------
// Name:    gps_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace gps {

using namespace network2;
using namespace marshalling;


	struct GPSInfo_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		double lon;
		double lat;
		float altitude;
		float speed;
	};

	struct AddLandMark_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		double lon;
		double lat;
	};



}
