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
			val: packet.getTypeVariant(),
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
}
function Make_sRegisterVector(packet, data) {
}

//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Dispatcher
//------------------------------------------------------------------------
remotedbg2.r2h_Dispatcher = class {
	constructor(isNoParseJSON = false) {
		this.isNoParseJSON = isNoParseJSON
	}
	//------------------------------------------------------------------------------
	// dispatch packet
	// wss: WebSocket Server
	// ws: WebSocket
	// message: ArrayBuffer
	// handlers: array of protocol handler
	dispatch(wss, ws, message, handlers) {
		// parse packet header, 16 bytes
		// | protocol id (4) | packet id (4) | packet length (4) | option (4) |
		const HeaderSize = 16
		let packet = new Packet()
		packet.initWithArrayBuffer(new Uint8Array(message).buffer)
		const protocolId = packet.getUint32()
		if (protocolId != 5301) return
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
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					}
				}
				break;

			case 1418562193: // UploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const code = packet.getStr()
						const parsePacket = {
							itprId,
							code,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.UploadIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.UploadIntermediateCode(parsePacket)
						})
					}
				}
				break;

			case 1644585100: // ReqIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqIntermediateCode(parsePacket)
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
								handler.ReqRun(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqRun(parsePacket)
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
								handler.ReqOneStep(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqOneStep(parsePacket)
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
								handler.ReqResumeRun(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqResumeRun(parsePacket)
						})
					}
				}
				break;

			case 784411795: // ReqBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreak(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreak(parsePacket)
						})
					}
				}
				break;

			case 2487089996: // ReqBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const parsePacket = {
							itprId,
							enable,
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreakPoint(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqBreakPoint(parsePacket)
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
								handler.ReqStop(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqStop(parsePacket)
						})
					}
				}
				break;

			case 3140751413: // ReqInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const eventName = packet.getStr()
						const parsePacket = {
							itprId,
							vmIdx,
							eventName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqInput(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqInput(parsePacket)
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
								handler.ReqStepDebugType(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqStepDebugType(parsePacket)
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
								handler.ReqHeartBeat(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.ReqHeartBeat(parsePacket)
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
	constructor(isNoParseJSON = false) {
		this.isNoParseJSON = isNoParseJSON
	}
	//------------------------------------------------------------------------------
	// dispatch packet
	// wss: WebSocket Server
	// ws: WebSocket
	// message: ArrayBuffer
	// handlers: array of protocol handler
	dispatch(wss, ws, message, handlers) {
		// parse packet header, 16 bytes
		// | protocol id (4) | packet id (4) | packet length (4) | option (4) |
		const HeaderSize = 16
		let packet = new Packet()
		packet.initWithArrayBuffer(new Uint8Array(message).buffer)
		const protocolId = packet.getUint32()
		if (protocolId != 5300) return
		const packetId = packet.getUint32()
		const packetLength = packet.getUint32()
		const option = packet.getUint32()

		// dispatch function
		switch (packetId) {
			case 4005257575: // AckUploadIntermediateCode
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
								handler.AckUploadIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckUploadIntermediateCode(parsePacket)
						})
					}
				}
				break;

			case 1397310616: // AckIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const count = packet.getUint32()
						const index = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket = {
							itprId,
							result,
							count,
							index,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckIntermediateCode(parsePacket)
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
								handler.AckRun(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckRun(parsePacket)
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
								handler.AckOneStep(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckOneStep(parsePacket)
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
								handler.AckResumeRun(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckResumeRun(parsePacket)
						})
					}
				}
				break;

			case 2129545277: // AckBreak
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
								handler.AckBreak(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreak(parsePacket)
						})
					}
				}
				break;

			case 2045074648: // AckBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							enable,
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreakPoint(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckBreakPoint(parsePacket)
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
								handler.AckStop(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckStop(parsePacket)
						})
					}
				}
				break;

			case 1658444570: // AckInput
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
								handler.AckInput(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckInput(parsePacket)
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
								handler.AckStepDebugType(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckStepDebugType(parsePacket)
						})
					}
				}
				break;

			case 4206107288: // SyncVMInstruction
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const indices = packet.getUint16Array()
						const parsePacket = {
							itprId,
							vmIdx,
							indices,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMInstruction(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMInstruction(parsePacket)
						})
					}
				}
				break;

			case 3001685594: // SyncVMRegister
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const infoType = packet.getInt32()
						const reg = Parse_sRegister(packet)
						const parsePacket = {
							itprId,
							vmIdx,
							infoType,
							reg,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMRegister(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMRegister(parsePacket)
						})
					}
				}
				break;

			case 3045798844: // SyncVMSymbolTable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const start = packet.getUint32()
						const count = packet.getUint32()
						const symbol = Parse_sSyncSymbolVector(packet)
						const parsePacket = {
							itprId,
							vmIdx,
							start,
							count,
							symbol,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMSymbolTable(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMSymbolTable(parsePacket)
						})
					}
				}
				break;

			case 1348120458: // SyncVMOutput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const output = packet.getStr()
						const parsePacket = {
							itprId,
							vmIdx,
							output,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMOutput(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.SyncVMOutput(parsePacket)
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
								handler.AckHeartBeat(parsePacket)
						})
					} else { // json?
						const parsePacket = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.AckHeartBeat(parsePacket)
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

	// Protocol: Welcome
	Welcome(isBinary, ws, msg, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(msg)
			WsSockServer.sendPacketBinary(ws, 5301, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			WsSockServer.sendPacket(ws, 5301, 1281093745, packet)
		}
	}
	
	// Protocol: UploadIntermediateCode
	UploadIntermediateCode(isBinary, ws, itprId, code, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(code)
			WsSockServer.sendPacketBinary(ws, 5301, 1418562193, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				code,
			}
			WsSockServer.sendPacket(ws, 5301, 1418562193, packet)
		}
	}
	
	// Protocol: ReqIntermediateCode
	ReqIntermediateCode(isBinary, ws, itprId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			WsSockServer.sendPacketBinary(ws, 5301, 1644585100, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			WsSockServer.sendPacket(ws, 5301, 1644585100, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(isBinary, ws, itprId, runType, ) {
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
	ReqOneStep(isBinary, ws, itprId, ) {
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
	ReqResumeRun(isBinary, ws, itprId, ) {
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
	ReqBreak(isBinary, ws, itprId, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			WsSockServer.sendPacketBinary(ws, 5301, 784411795, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			WsSockServer.sendPacket(ws, 5301, 784411795, packet)
		}
	}
	
	// Protocol: ReqBreakPoint
	ReqBreakPoint(isBinary, ws, itprId, enable, id, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			WsSockServer.sendPacketBinary(ws, 5301, 2487089996, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				enable,
				id,
			}
			WsSockServer.sendPacket(ws, 5301, 2487089996, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(isBinary, ws, itprId, ) {
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
	ReqInput(isBinary, ws, itprId, vmIdx, eventName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushStr(eventName)
			WsSockServer.sendPacketBinary(ws, 5301, 3140751413, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				eventName,
			}
			WsSockServer.sendPacket(ws, 5301, 3140751413, packet)
		}
	}
	
	// Protocol: ReqStepDebugType
	ReqStepDebugType(isBinary, ws, stepDbgType, ) {
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
	
	// Protocol: ReqHeartBeat
	ReqHeartBeat(isBinary, ws, ) {
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

	// Protocol: AckUploadIntermediateCode
	AckUploadIntermediateCode(isBinary, ws, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 4005257575, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 4005257575, packet)
		}
	}
	
	// Protocol: AckIntermediateCode
	AckIntermediateCode(isBinary, ws, itprId, result, count, index, data, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			packet.pushUint32(count)
			packet.pushUint32(index)
			packet.pushUint8Array(data)
			WsSockServer.sendPacketBinary(ws, 5300, 1397310616, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
				count,
				index,
				data,
			}
			WsSockServer.sendPacket(ws, 5300, 1397310616, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(isBinary, ws, itprId, result, ) {
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
	AckOneStep(isBinary, ws, itprId, result, ) {
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
	AckResumeRun(isBinary, ws, itprId, result, ) {
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
	AckBreak(isBinary, ws, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 2129545277, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 2129545277, packet)
		}
	}
	
	// Protocol: AckBreakPoint
	AckBreakPoint(isBinary, ws, itprId, enable, id, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 2045074648, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				enable,
				id,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 2045074648, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(isBinary, ws, itprId, result, ) {
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
	AckInput(isBinary, ws, itprId, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 5300, 1658444570, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			WsSockServer.sendPacket(ws, 5300, 1658444570, packet)
		}
	}
	
	// Protocol: AckStepDebugType
	AckStepDebugType(isBinary, ws, stepDbgType, result, ) {
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
	
	// Protocol: SyncVMInstruction
	SyncVMInstruction(isBinary, ws, itprId, vmIdx, indices, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushUint16Array(indices)
			WsSockServer.sendPacketBinary(ws, 5300, 4206107288, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				indices,
			}
			WsSockServer.sendPacket(ws, 5300, 4206107288, packet)
		}
	}
	
	// Protocol: SyncVMRegister
	SyncVMRegister(isBinary, ws, itprId, vmIdx, infoType, reg, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushInt32(infoType)
			Make_sRegister(packet,reg)
			WsSockServer.sendPacketBinary(ws, 5300, 3001685594, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				infoType,
				reg,
			}
			WsSockServer.sendPacket(ws, 5300, 3001685594, packet)
		}
	}
	
	// Protocol: SyncVMSymbolTable
	SyncVMSymbolTable(isBinary, ws, itprId, vmIdx, start, count, symbol, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushUint32(start)
			packet.pushUint32(count)
			Make_sSyncSymbolVector(packet,symbol)
			WsSockServer.sendPacketBinary(ws, 5300, 3045798844, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				start,
				count,
				symbol,
			}
			WsSockServer.sendPacket(ws, 5300, 3045798844, packet)
		}
	}
	
	// Protocol: SyncVMOutput
	SyncVMOutput(isBinary, ws, itprId, vmIdx, output, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushStr(output)
			WsSockServer.sendPacketBinary(ws, 5300, 1348120458, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				output,
			}
			WsSockServer.sendPacket(ws, 5300, 1348120458, packet)
		}
	}
	
	// Protocol: AckHeartBeat
	AckHeartBeat(isBinary, ws, ) {
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
	constructor() { } 
	Welcome(wss, ws, packet) {}
	UploadIntermediateCode(wss, ws, packet) {}
	ReqIntermediateCode(wss, ws, packet) {}
	ReqRun(wss, ws, packet) {}
	ReqOneStep(wss, ws, packet) {}
	ReqResumeRun(wss, ws, packet) {}
	ReqBreak(wss, ws, packet) {}
	ReqBreakPoint(wss, ws, packet) {}
	ReqStop(wss, ws, packet) {}
	ReqInput(wss, ws, packet) {}
	ReqStepDebugType(wss, ws, packet) {}
	ReqHeartBeat(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol Handler
//------------------------------------------------------------------------
remotedbg2.h2r_ProtocolHandler = class {
	constructor() { } 
	AckUploadIntermediateCode(wss, ws, packet) {}
	AckIntermediateCode(wss, ws, packet) {}
	AckRun(wss, ws, packet) {}
	AckOneStep(wss, ws, packet) {}
	AckResumeRun(wss, ws, packet) {}
	AckBreak(wss, ws, packet) {}
	AckBreakPoint(wss, ws, packet) {}
	AckStop(wss, ws, packet) {}
	AckInput(wss, ws, packet) {}
	AckStepDebugType(wss, ws, packet) {}
	SyncVMInstruction(wss, ws, packet) {}
	SyncVMRegister(wss, ws, packet) {}
	SyncVMSymbolTable(wss, ws, packet) {}
	SyncVMOutput(wss, ws, packet) {}
	AckHeartBeat(wss, ws, packet) {}
}

