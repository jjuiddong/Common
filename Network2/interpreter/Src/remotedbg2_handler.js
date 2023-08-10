//------------------------------------------------------------------------
// Name:    remotedbg2
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import Dbg from "../../dbg/dbg";
import WsSockServer from "../wsserver";
import Packet from "../packet"

export default class remotedbg2 {}

function Parse_sSyncSymbol(packet) {
	return {
			scope: packet.getStr(),
			name: packet.getStr(),
			var: packet.getTypeVariant(),
	}
}
function Parse_sSyncSymbolVector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_sSyncSymbol(packet))
	return ar
}
function Make_sSyncSymbol(packet, data) {
		packet.pushStr(data.scope)
		packet.pushStr(data.name)
		packet.pushTypeVariant(data.var)
}
function Make_sSyncSymbolVector(packet, data) {
}
function Parse_sRegister(packet) {
	return {
			idx: packet.getUint32(),
			cmp: packet.getBool(),
			val: packet.getTypeVariantVector(),
	}
}
function Parse_sRegisterVector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_sRegister(packet))
	return ar
}
function Make_sRegister(packet, data) {
		packet.pushUint32(data.idx)
		packet.pushBool(data.cmp)
		packet.pushTypeVariantVector(data.val)
}
function Make_sRegisterVector(packet, data) {
}

//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Dispatcher
//------------------------------------------------------------------------
remotedbg2.r2h_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 5301
		this.isRelay = isRelay
	}
	//------------------------------------------------------------------------------
	// dispatch packet
	// wss: WebSocket Server
	// ws: WebSocket
	// packet: Packet class
	// handlers: array of protocol handler
	dispatch(wss, ws, packet, handlers) {
		if (this.isRelay) {
			handlers.forEach(handler => {
				if (handler instanceof remotedbg2.r2h_ProtocolHandler)
					handler.Relay(wss, ws, packet)
			})
			return
		}

		// parse packet header, 16 bytes
		// | protocol id (4) | packet id (4) | packet length (4) | option (4) |
		packet.init()
		const protocolId = packet.getUint32()
		const packetId = packet.getUint32()
		const packetLength = packet.getUint32()
		const option = packet.getUint32()

		// dispatch function
		switch (packetId) {
			case 1418562193: // UploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const code = packet.getStr()
						const parsePacket = {
							itprId,
							vmId,
							code,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.UploadIntermediateCode(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.UploadIntermediateCode(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1644585100: // ReqIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqIntermediateCode(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqIntermediateCode(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 923151823: // ReqRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const runType = packet.getStr()
						const parsePacket = {
							itprId,
							runType,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqRun(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqRun(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2884814738: // ReqOneStep
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqOneStep(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqOneStep(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 742173167: // ReqResumeRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqResumeRun(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqResumeRun(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 784411795: // ReqBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreak(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreak(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2487089996: // ReqBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const parsePacket = {
							itprId,
							vmId,
							enable,
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreakPoint(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreakPoint(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1453251868: // ReqStop
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqStop(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqStop(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3140751413: // ReqInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const eventName = packet.getStr()
						const parsePacket = {
							itprId,
							vmId,
							eventName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqInput(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqInput(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 186222094: // ReqEvent
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const eventName = packet.getStr()
						const values = packet.getMapStrArray()
						const parsePacket = {
							itprId,
							vmId,
							eventName,
							values,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqEvent(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqEvent(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3084593987: // ReqStepDebugType
				{
					if (option == 1) { // binary?
						const stepDbgType = packet.getInt32()
						const parsePacket = {
							stepDbgType,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqStepDebugType(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqStepDebugType(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2166551586: // ReqDebugInfo
				{
					if (option == 1) { // binary?
						const vmIds = packet.getInt32Array()
						const parsePacket = {
							vmIds,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqDebugInfo(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqDebugInfo(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4127584998: // ReqVariableInfo
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const parsePacket = {
							itprId,
							vmId,
							varName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqVariableInfo(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqVariableInfo(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1626832220: // ReqChangeVariable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const value = packet.getStr()
						const parsePacket = {
							itprId,
							vmId,
							varName,
							value,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqChangeVariable(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqChangeVariable(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2532286881: // ReqHeartBeat
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqHeartBeat(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqHeartBeat(wss, ws, parsePacket)
						})
					}
				}
				break;

		default:
			Dbg.Log(0, 1, `RemoteDbg2 receive not defined packet bin:${option}, ${packetId}`)
			break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol Dispatcher
//------------------------------------------------------------------------
remotedbg2.h2r_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 5300
		this.isRelay = isRelay
	}
	//------------------------------------------------------------------------------
	// dispatch packet
	// wss: WebSocket Server
	// ws: WebSocket
	// packet: Packet class
	// handlers: array of protocol handler
	dispatch(wss, ws, packet, handlers) {
		if (this.isRelay) {
			handlers.forEach(handler => {
				if (handler instanceof remotedbg2.h2r_ProtocolHandler)
					handler.Relay(wss, ws, packet)
			})
			return
		}

		// parse packet header, 16 bytes
		// | protocol id (4) | packet id (4) | packet length (4) | option (4) |
		packet.init()
		const protocolId = packet.getUint32()
		const packetId = packet.getUint32()
		const packetLength = packet.getUint32()
		const option = packet.getUint32()

		// dispatch function
		switch (packetId) {
			case 1281093745: // Welcome
				{
					if (option == 1) { // binary?
						const msg = packet.getStr()
						const parsePacket = {
							msg,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Welcome(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Welcome(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4005257575: // AckUploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckUploadIntermediateCode(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckUploadIntermediateCode(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1397310616: // AckIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getUint8()
						const vmId = packet.getInt32()
						const result = packet.getUint8()
						const count = packet.getUint8()
						const index = packet.getUint8()
						const totalBufferSize = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket = {
							itprId,
							vmId,
							result,
							count,
							index,
							totalBufferSize,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckIntermediateCode(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckIntermediateCode(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 977092268: // SpawnTotalInterpreterInfo
				{
					if (option == 1) { // binary?
						const totalCount = packet.getInt32()
						const index = packet.getInt32()
						const itprId = packet.getInt32()
						const vmIds = packet.getInt32Array()
						const parsePacket = {
							totalCount,
							index,
							itprId,
							vmIds,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SpawnTotalInterpreterInfo(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SpawnTotalInterpreterInfo(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 762776747: // SpawnInterpreterInfo
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parentVmId = packet.getInt32()
						const vmId = packet.getInt32()
						const nodeFileName = packet.getStr()
						const nodeName = packet.getStr()
						const parsePacket = {
							itprId,
							parentVmId,
							vmId,
							nodeFileName,
							nodeName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SpawnInterpreterInfo(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SpawnInterpreterInfo(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 940777945: // RemoveInterpreter
				{
					if (option == 1) { // binary?
						const vmId = packet.getInt32()
						const parsePacket = {
							vmId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.RemoveInterpreter(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.RemoveInterpreter(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4148808214: // AckRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckRun(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckRun(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3643391279: // AckOneStep
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckOneStep(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckOneStep(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1012496086: // AckResumeRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckResumeRun(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckResumeRun(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2129545277: // AckBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreak(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreak(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2045074648: // AckBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
							enable,
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreakPoint(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreakPoint(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 114435221: // AckStop
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckStop(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckStop(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1658444570: // AckInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckInput(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckInput(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1906481345: // AckEvent
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const eventName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
							eventName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckEvent(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckEvent(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4225702489: // AckStepDebugType
				{
					if (option == 1) { // binary?
						const stepDbgType = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							stepDbgType,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckStepDebugType(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckStepDebugType(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4276104084: // AckDebugInfo
				{
					if (option == 1) { // binary?
						const vmIds = packet.getInt32Array()
						const result = packet.getInt32()
						const parsePacket = {
							vmIds,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckDebugInfo(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckDebugInfo(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3660358812: // AckChangeVariable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							vmId,
							varName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckChangeVariable(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckChangeVariable(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4206107288: // SyncVMInstruction
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const indices = packet.getUint16Array()
						const parsePacket = {
							itprId,
							vmId,
							indices,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMInstruction(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMInstruction(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3001685594: // SyncVMRegister
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const infoType = packet.getInt32()
						const reg = Parse_sRegister(packet)
						const parsePacket = {
							itprId,
							vmId,
							infoType,
							reg,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMRegister(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMRegister(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3045798844: // SyncVMSymbolTable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const start = packet.getUint32()
						const count = packet.getUint32()
						const symbol = Parse_sSyncSymbolVector(packet)
						const parsePacket = {
							itprId,
							vmId,
							start,
							count,
							symbol,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMSymbolTable(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMSymbolTable(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1348120458: // SyncVMOutput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const output = packet.getStr()
						const parsePacket = {
							itprId,
							vmId,
							output,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMOutput(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMOutput(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 323195839: // SyncVMWidgets
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const widgetName = packet.getStr()
						const parsePacket = {
							itprId,
							vmId,
							widgetName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMWidgets(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMWidgets(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1209241191: // SyncVMArray
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const array = packet.getTypeVariantVector()
						const parsePacket = {
							itprId,
							vmId,
							varName,
							startIdx,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArray(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArray(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3278867969: // SyncVMArrayBool
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const array = packet.getUint8Array()
						const parsePacket = {
							itprId,
							vmId,
							varName,
							startIdx,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArrayBool(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArrayBool(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3822230413: // SyncVMArrayNumber
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const array = packet.getFloat32Array()
						const parsePacket = {
							itprId,
							vmId,
							varName,
							startIdx,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArrayNumber(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArrayNumber(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2291689449: // SyncVMArrayString
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const array = packet.getStrArray()
						const parsePacket = {
							itprId,
							vmId,
							varName,
							startIdx,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArrayString(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMArrayString(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1133387750: // AckHeartBeat
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckHeartBeat(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckHeartBeat(wss, ws, parsePacket)
						})
					}
				}
				break;

		default:
			Dbg.Log(0, 1, `RemoteDbg2 receive not defined packet bin:${option}, ${packetId}`)
			break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// remotedbg2 r2h Protocol 
//------------------------------------------------------------------------
remotedbg2.r2h_Protocol = class {
	constructor() { }

	// Protocol: UploadIntermediateCode
	UploadIntermediateCode(ws, isBinary, itprId, vmId, code, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(code)
			WsSockServer.sendPacketBinary(ws, 5301, 1418562193, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				code,
			}
			WsSockServer.sendPacket(ws, 5301, 1418562193, packet)
		}
	}
	
	// Protocol: ReqIntermediateCode
	ReqIntermediateCode(ws, isBinary, itprId, vmId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			WsSockServer.sendPacketBinary(ws, 5301, 1644585100, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			WsSockServer.sendPacket(ws, 5301, 1644585100, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(ws, isBinary, itprId, runType, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(runType)
			WsSockServer.sendPacketBinary(ws, 5301, 923151823, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				runType,
			}
			WsSockServer.sendPacket(ws, 5301, 923151823, packet)
		}
	}
	
	// Protocol: ReqOneStep
	ReqOneStep(ws, isBinary, itprId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			WsSockServer.sendPacketBinary(ws, 5301, 2884814738, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			WsSockServer.sendPacket(ws, 5301, 2884814738, packet)
		}
	}
	
	// Protocol: ReqResumeRun
	ReqResumeRun(ws, isBinary, itprId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			WsSockServer.sendPacketBinary(ws, 5301, 742173167, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			WsSockServer.sendPacket(ws, 5301, 742173167, packet)
		}
	}
	
	// Protocol: ReqBreak
	ReqBreak(ws, isBinary, itprId, vmId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			WsSockServer.sendPacketBinary(ws, 5301, 784411795, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			WsSockServer.sendPacket(ws, 5301, 784411795, packet)
		}
	}
	
	// Protocol: ReqBreakPoint
	ReqBreakPoint(ws, isBinary, itprId, vmId, enable, id, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			WsSockServer.sendPacketBinary(ws, 5301, 2487089996, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
			}
			WsSockServer.sendPacket(ws, 5301, 2487089996, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(ws, isBinary, itprId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			WsSockServer.sendPacketBinary(ws, 5301, 1453251868, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			WsSockServer.sendPacket(ws, 5301, 1453251868, packet)
		}
	}
	
	// Protocol: ReqInput
	ReqInput(ws, isBinary, itprId, vmId, eventName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			WsSockServer.sendPacketBinary(ws, 5301, 3140751413, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
			}
			WsSockServer.sendPacket(ws, 5301, 3140751413, packet)
		}
	}
	
	// Protocol: ReqEvent
	ReqEvent(ws, isBinary, itprId, vmId, eventName, values, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			packet.pushMapStrArray(values)
			WsSockServer.sendPacketBinary(ws, 5301, 186222094, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				values,
			}
			WsSockServer.sendPacket(ws, 5301, 186222094, packet)
		}
	}
	
	// Protocol: ReqStepDebugType
	ReqStepDebugType(ws, isBinary, stepDbgType, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(stepDbgType)
			WsSockServer.sendPacketBinary(ws, 5301, 3084593987, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
			}
			WsSockServer.sendPacket(ws, 5301, 3084593987, packet)
		}
	}
	
	// Protocol: ReqDebugInfo
	ReqDebugInfo(ws, isBinary, vmIds, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32Array(vmIds)
			WsSockServer.sendPacketBinary(ws, 5301, 2166551586, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
			}
			WsSockServer.sendPacket(ws, 5301, 2166551586, packet)
		}
	}
	
	// Protocol: ReqVariableInfo
	ReqVariableInfo(ws, isBinary, itprId, vmId, varName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			WsSockServer.sendPacketBinary(ws, 5301, 4127584998, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
			}
			WsSockServer.sendPacket(ws, 5301, 4127584998, packet)
		}
	}
	
	// Protocol: ReqChangeVariable
	ReqChangeVariable(ws, isBinary, itprId, vmId, varName, value, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushStr(value)
			WsSockServer.sendPacketBinary(ws, 5301, 1626832220, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				value,
			}
			WsSockServer.sendPacket(ws, 5301, 1626832220, packet)
		}
	}
	
	// Protocol: ReqHeartBeat
	ReqHeartBeat(ws, isBinary, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			WsSockServer.sendPacketBinary(ws, 5301, 2532286881, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			WsSockServer.sendPacket(ws, 5301, 2532286881, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol 
//------------------------------------------------------------------------
remotedbg2.h2r_Protocol = class {
	constructor() { }

	// Protocol: Welcome
	Welcome(ws, isBinary, msg, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(msg)
			WsSockServer.sendPacketBinary(ws, 5300, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			WsSockServer.sendPacket(ws, 5300, 1281093745, packet)
		}
	}
	
	// Protocol: AckUploadIntermediateCode
	AckUploadIntermediateCode(ws, isBinary, itprId, vmId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 4005257575, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 4005257575, packet)
		}
	}
	
	// Protocol: AckIntermediateCode
	AckIntermediateCode(ws, isBinary, itprId, vmId, result, count, index, totalBufferSize, data, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushUint8(itprId)
			packet.pushInt32(vmId)
			packet.pushUint8(result)
			packet.pushUint8(count)
			packet.pushUint8(index)
			packet.pushUint32(totalBufferSize)
			packet.pushUint8Array(data)
			WsSockServer.sendPacketBinary(ws, 5300, 1397310616, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
				count,
				index,
				totalBufferSize,
				data,
			}
			WsSockServer.sendPacket(ws, 5300, 1397310616, packet)
		}
	}
	
	// Protocol: SpawnTotalInterpreterInfo
	SpawnTotalInterpreterInfo(ws, isBinary, totalCount, index, itprId, vmIds, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(totalCount)
			packet.pushInt32(index)
			packet.pushInt32(itprId)
			packet.pushInt32Array(vmIds)
			WsSockServer.sendPacketBinary(ws, 5300, 977092268, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				totalCount,
				index,
				itprId,
				vmIds,
			}
			WsSockServer.sendPacket(ws, 5300, 977092268, packet)
		}
	}
	
	// Protocol: SpawnInterpreterInfo
	SpawnInterpreterInfo(ws, isBinary, itprId, parentVmId, vmId, nodeFileName, nodeName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(parentVmId)
			packet.pushInt32(vmId)
			packet.pushStr(nodeFileName)
			packet.pushStr(nodeName)
			WsSockServer.sendPacketBinary(ws, 5300, 762776747, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				parentVmId,
				vmId,
				nodeFileName,
				nodeName,
			}
			WsSockServer.sendPacket(ws, 5300, 762776747, packet)
		}
	}
	
	// Protocol: RemoveInterpreter
	RemoveInterpreter(ws, isBinary, vmId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(vmId)
			WsSockServer.sendPacketBinary(ws, 5300, 940777945, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmId,
			}
			WsSockServer.sendPacket(ws, 5300, 940777945, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(ws, isBinary, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 4148808214, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 4148808214, packet)
		}
	}
	
	// Protocol: AckOneStep
	AckOneStep(ws, isBinary, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 3643391279, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 3643391279, packet)
		}
	}
	
	// Protocol: AckResumeRun
	AckResumeRun(ws, isBinary, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 1012496086, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 1012496086, packet)
		}
	}
	
	// Protocol: AckBreak
	AckBreak(ws, isBinary, itprId, vmId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 2129545277, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 2129545277, packet)
		}
	}
	
	// Protocol: AckBreakPoint
	AckBreakPoint(ws, isBinary, itprId, vmId, enable, id, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 2045074648, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 2045074648, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(ws, isBinary, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 114435221, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 114435221, packet)
		}
	}
	
	// Protocol: AckInput
	AckInput(ws, isBinary, itprId, vmId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 1658444570, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 1658444570, packet)
		}
	}
	
	// Protocol: AckEvent
	AckEvent(ws, isBinary, itprId, vmId, eventName, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 1906481345, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 1906481345, packet)
		}
	}
	
	// Protocol: AckStepDebugType
	AckStepDebugType(ws, isBinary, stepDbgType, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(stepDbgType)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 4225702489, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 4225702489, packet)
		}
	}
	
	// Protocol: AckDebugInfo
	AckDebugInfo(ws, isBinary, vmIds, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32Array(vmIds)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 4276104084, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 4276104084, packet)
		}
	}
	
	// Protocol: AckChangeVariable
	AckChangeVariable(ws, isBinary, itprId, vmId, varName, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 3660358812, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 3660358812, packet)
		}
	}
	
	// Protocol: SyncVMInstruction
	SyncVMInstruction(ws, isBinary, itprId, vmId, indices, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushUint16Array(indices)
			WsSockServer.sendPacketBinary(ws, 5300, 4206107288, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				indices,
			}
			WsSockServer.sendPacket(ws, 5300, 4206107288, packet)
		}
	}
	
	// Protocol: SyncVMRegister
	SyncVMRegister(ws, isBinary, itprId, vmId, infoType, reg, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(infoType)
			Make_sRegister(packet,reg)
			WsSockServer.sendPacketBinary(ws, 5300, 3001685594, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				infoType,
				reg,
			}
			WsSockServer.sendPacket(ws, 5300, 3001685594, packet)
		}
	}
	
	// Protocol: SyncVMSymbolTable
	SyncVMSymbolTable(ws, isBinary, itprId, vmId, start, count, symbol, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushUint32(start)
			packet.pushUint32(count)
			Make_sSyncSymbolVector(packet,symbol)
			WsSockServer.sendPacketBinary(ws, 5300, 3045798844, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				start,
				count,
				symbol,
			}
			WsSockServer.sendPacket(ws, 5300, 3045798844, packet)
		}
	}
	
	// Protocol: SyncVMOutput
	SyncVMOutput(ws, isBinary, itprId, vmId, output, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(output)
			WsSockServer.sendPacketBinary(ws, 5300, 1348120458, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				output,
			}
			WsSockServer.sendPacket(ws, 5300, 1348120458, packet)
		}
	}
	
	// Protocol: SyncVMWidgets
	SyncVMWidgets(ws, isBinary, itprId, vmId, widgetName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(widgetName)
			WsSockServer.sendPacketBinary(ws, 5300, 323195839, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				widgetName,
			}
			WsSockServer.sendPacket(ws, 5300, 323195839, packet)
		}
	}
	
	// Protocol: SyncVMArray
	SyncVMArray(ws, isBinary, itprId, vmId, varName, startIdx, array, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushTypeVariantVector(array)
			WsSockServer.sendPacketBinary(ws, 5300, 1209241191, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				array,
			}
			WsSockServer.sendPacket(ws, 5300, 1209241191, packet)
		}
	}
	
	// Protocol: SyncVMArrayBool
	SyncVMArrayBool(ws, isBinary, itprId, vmId, varName, startIdx, array, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushBoolArray(array)
			WsSockServer.sendPacketBinary(ws, 5300, 3278867969, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				array,
			}
			WsSockServer.sendPacket(ws, 5300, 3278867969, packet)
		}
	}
	
	// Protocol: SyncVMArrayNumber
	SyncVMArrayNumber(ws, isBinary, itprId, vmId, varName, startIdx, array, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushFloat32Array(array)
			WsSockServer.sendPacketBinary(ws, 5300, 3822230413, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				array,
			}
			WsSockServer.sendPacket(ws, 5300, 3822230413, packet)
		}
	}
	
	// Protocol: SyncVMArrayString
	SyncVMArrayString(ws, isBinary, itprId, vmId, varName, startIdx, array, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushStrArray(array)
			WsSockServer.sendPacketBinary(ws, 5300, 2291689449, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				array,
			}
			WsSockServer.sendPacket(ws, 5300, 2291689449, packet)
		}
	}
	
	// Protocol: AckHeartBeat
	AckHeartBeat(ws, isBinary, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			WsSockServer.sendPacketBinary(ws, 5300, 1133387750, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			WsSockServer.sendPacket(ws, 5300, 1133387750, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Handler
//------------------------------------------------------------------------
remotedbg2.r2h_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	UploadIntermediateCode(wss, ws, packet) {}
	ReqIntermediateCode(wss, ws, packet) {}
	ReqRun(wss, ws, packet) {}
	ReqOneStep(wss, ws, packet) {}
	ReqResumeRun(wss, ws, packet) {}
	ReqBreak(wss, ws, packet) {}
	ReqBreakPoint(wss, ws, packet) {}
	ReqStop(wss, ws, packet) {}
	ReqInput(wss, ws, packet) {}
	ReqEvent(wss, ws, packet) {}
	ReqStepDebugType(wss, ws, packet) {}
	ReqDebugInfo(wss, ws, packet) {}
	ReqVariableInfo(wss, ws, packet) {}
	ReqChangeVariable(wss, ws, packet) {}
	ReqHeartBeat(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol Handler
//------------------------------------------------------------------------
remotedbg2.h2r_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Welcome(wss, ws, packet) {}
	AckUploadIntermediateCode(wss, ws, packet) {}
	AckIntermediateCode(wss, ws, packet) {}
	SpawnTotalInterpreterInfo(wss, ws, packet) {}
	SpawnInterpreterInfo(wss, ws, packet) {}
	RemoveInterpreter(wss, ws, packet) {}
	AckRun(wss, ws, packet) {}
	AckOneStep(wss, ws, packet) {}
	AckResumeRun(wss, ws, packet) {}
	AckBreak(wss, ws, packet) {}
	AckBreakPoint(wss, ws, packet) {}
	AckStop(wss, ws, packet) {}
	AckInput(wss, ws, packet) {}
	AckEvent(wss, ws, packet) {}
	AckStepDebugType(wss, ws, packet) {}
	AckDebugInfo(wss, ws, packet) {}
	AckChangeVariable(wss, ws, packet) {}
	SyncVMInstruction(wss, ws, packet) {}
	SyncVMRegister(wss, ws, packet) {}
	SyncVMSymbolTable(wss, ws, packet) {}
	SyncVMOutput(wss, ws, packet) {}
	SyncVMWidgets(wss, ws, packet) {}
	SyncVMArray(wss, ws, packet) {}
	SyncVMArrayBool(wss, ws, packet) {}
	SyncVMArrayNumber(wss, ws, packet) {}
	SyncVMArrayString(wss, ws, packet) {}
	AckHeartBeat(wss, ws, packet) {}
}

