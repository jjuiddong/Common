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
static const int r2h_Dispatcher_ID = 5301;

// Protocol Dispatcher
class r2h_Dispatcher: public network2::cProtocolDispatcher
{
public:
	r2h_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	static cPacketHeaderJson s_packetHeader;
};
static r2h_Dispatcher g_remotedbg2_r2h_Dispatcher;


// ProtocolHandler
class r2h_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class r2h_Dispatcher;
	r2h_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool UploadIntermediateCode(remotedbg2::UploadIntermediateCode_Packet &packet) { return true; }
	virtual bool ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet) { return true; }
	virtual bool ReqRun(remotedbg2::ReqRun_Packet &packet) { return true; }
	virtual bool ReqOneStep(remotedbg2::ReqOneStep_Packet &packet) { return true; }
	virtual bool ReqResumeRun(remotedbg2::ReqResumeRun_Packet &packet) { return true; }
	virtual bool ReqBreak(remotedbg2::ReqBreak_Packet &packet) { return true; }
	virtual bool ReqBreakPoint(remotedbg2::ReqBreakPoint_Packet &packet) { return true; }
	virtual bool ReqStop(remotedbg2::ReqStop_Packet &packet) { return true; }
	virtual bool ReqInput(remotedbg2::ReqInput_Packet &packet) { return true; }
	virtual bool ReqEvent(remotedbg2::ReqEvent_Packet &packet) { return true; }
	virtual bool ReqStepDebugType(remotedbg2::ReqStepDebugType_Packet &packet) { return true; }
	virtual bool ReqDebugInfo(remotedbg2::ReqDebugInfo_Packet &packet) { return true; }
	virtual bool ReqVariableInfo(remotedbg2::ReqVariableInfo_Packet &packet) { return true; }
	virtual bool ReqHeartBeat(remotedbg2::ReqHeartBeat_Packet &packet) { return true; }
};


static const int h2r_Dispatcher_ID = 5300;

// Protocol Dispatcher
class h2r_Dispatcher: public network2::cProtocolDispatcher
{
public:
	h2r_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	static cPacketHeaderJson s_packetHeader;
};
static h2r_Dispatcher g_remotedbg2_h2r_Dispatcher;


// ProtocolHandler
class h2r_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class h2r_Dispatcher;
	h2r_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool Welcome(remotedbg2::Welcome_Packet &packet) { return true; }
	virtual bool AckUploadIntermediateCode(remotedbg2::AckUploadIntermediateCode_Packet &packet) { return true; }
	virtual bool AckIntermediateCode(remotedbg2::AckIntermediateCode_Packet &packet) { return true; }
	virtual bool AckRun(remotedbg2::AckRun_Packet &packet) { return true; }
	virtual bool AckOneStep(remotedbg2::AckOneStep_Packet &packet) { return true; }
	virtual bool AckResumeRun(remotedbg2::AckResumeRun_Packet &packet) { return true; }
	virtual bool AckBreak(remotedbg2::AckBreak_Packet &packet) { return true; }
	virtual bool AckBreakPoint(remotedbg2::AckBreakPoint_Packet &packet) { return true; }
	virtual bool AckStop(remotedbg2::AckStop_Packet &packet) { return true; }
	virtual bool AckInput(remotedbg2::AckInput_Packet &packet) { return true; }
	virtual bool AckEvent(remotedbg2::AckEvent_Packet &packet) { return true; }
	virtual bool AckStepDebugType(remotedbg2::AckStepDebugType_Packet &packet) { return true; }
	virtual bool AckDebugInfo(remotedbg2::AckDebugInfo_Packet &packet) { return true; }
	virtual bool SyncVMInstruction(remotedbg2::SyncVMInstruction_Packet &packet) { return true; }
	virtual bool SyncVMRegister(remotedbg2::SyncVMRegister_Packet &packet) { return true; }
	virtual bool SyncVMSymbolTable(remotedbg2::SyncVMSymbolTable_Packet &packet) { return true; }
	virtual bool SyncVMOutput(remotedbg2::SyncVMOutput_Packet &packet) { return true; }
	virtual bool SyncVMWidgets(remotedbg2::SyncVMWidgets_Packet &packet) { return true; }
	virtual bool SyncVMArray(remotedbg2::SyncVMArray_Packet &packet) { return true; }
	virtual bool SyncVMArrayBool(remotedbg2::SyncVMArrayBool_Packet &packet) { return true; }
	virtual bool SyncVMArrayNumber(remotedbg2::SyncVMArrayNumber_Packet &packet) { return true; }
	virtual bool SyncVMArrayString(remotedbg2::SyncVMArrayString_Packet &packet) { return true; }
	virtual bool AckHeartBeat(remotedbg2::AckHeartBeat_Packet &packet) { return true; }
};


}
