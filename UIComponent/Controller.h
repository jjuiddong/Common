//
// 전역 제어 객체
//
// 큐브 모니터 프로그램을 제어한다.
//
#pragma once

#include "UpdateObserver.h"
#include "UDPObserver.h"


class cCubeCar;
class cSerialCommunication;
class cUDPCommunication;
class cController : public common::cSingleton<cController>
							, public cUpdateObservable
							, public cUDPObservable
{
public:
	cController();
	virtual ~cController();

	bool ConnectSerial(const int portNum, const int baudRate);
	void CloseSerial();
	void Update(const float deltaSeconds);

	float GetGlobalSeconds() const;

	//cSerialCommunication& GetSerialComm();
	cSerialAsync& GetSerialComm();
	network::cUDPServer& GetUDPComm();


protected:
	//cSerialCommunication *m_serialComm;
	cSerialAsync *m_serialComm;
	network::cUDPServer m_udpServer;
	float m_globalSeconds;
};


inline cSerialAsync& cController::GetSerialComm() { return *m_serialComm; }
inline network::cUDPServer& cController::GetUDPComm() { return m_udpServer; }
inline float cController::GetGlobalSeconds() const { return m_globalSeconds; }
