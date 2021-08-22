//------------------------------------------------------------------------
// Name:    remotedbg2_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "remotedbg2_ProtocolData.h"

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;
static const int h2r_Dispatcher_ID = 5300;

// Protocol Dispatcher
class h2r_Dispatcher: public network2::cProtocolDispatcher
{
public:
	h2r_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static h2r_Dispatcher g_remotedbg2_h2r_Dispatcher;


// ProtocolHandler
class h2r_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class h2r_Dispatcher;
	h2r_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool AckUploadIntermediateCode(remotedbg2::AckUploadIntermediateCode_Packet &packet) { return true; }
	virtual bool AckIntermediateCode(remotedbg2::AckIntermediateCode_Packet &packet) { return true; }
	virtual bool AckRun(remotedbg2::AckRun_Packet &packet) { return true; }
	virtual bool AckOneStep(remotedbg2::AckOneStep_Packet &packet) { return true; }
	virtual bool AckResumeRun(remotedbg2::AckResumeRun_Packet &packet) { return true; }
	virtual bool AckBreak(remotedbg2::AckBreak_Packet &packet) { return true; }
	virtual bool AckBreakPoint(remotedbg2::AckBreakPoint_Packet &packet) { return true; }
	virtual bool AckStop(remotedbg2::AckStop_Packet &packet) { return true; }
	virtual bool AckInput(remotedbg2::AckInput_Packet &packet) { return true; }
	virtual bool AckStepDebugType(remotedbg2::AckStepDebugType_Packet &packet) { return true; }
	virtual bool SyncVMInstruction(remotedbg2::SyncVMInstruction_Packet &packet) { return true; }
	virtual bool SyncVMRegister(remotedbg2::SyncVMRegister_Packet &packet) { return true; }
	virtual bool SyncVMSymbolTable(remotedbg2::SyncVMSymbolTable_Packet &packet) { return true; }
	virtual bool SyncVMOutput(remotedbg2::SyncVMOutput_Packet &packet) { return true; }
	virtual bool AckHeartBeat(remotedbg2::AckHeartBeat_Packet &packet) { return true; }
};


}
