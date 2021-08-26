//------------------------------------------------------------------------
// Name:    editor
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import Dbg from "../../dbg/dbg";
import WsSockServer from "../wsserver";
import Packet from "../packet"

export default class editor {}


//------------------------------------------------------------------------
// editor s2c Protocol Dispatcher
//------------------------------------------------------------------------
editor.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 9000
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
				if (handler instanceof editor.s2c_ProtocolHandler)
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
			case 207562622: // AckRunSimulation
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof editor.s2c_ProtocolHandler)
								handler.AckRunSimulation(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof editor.s2c_ProtocolHandler)
								handler.AckRunSimulation(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1279498047: // AckRunProject
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							projectName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof editor.s2c_ProtocolHandler)
								handler.AckRunProject(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof editor.s2c_ProtocolHandler)
								handler.AckRunProject(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4276051529: // AckStopSimulation
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof editor.s2c_ProtocolHandler)
								handler.AckStopSimulation(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof editor.s2c_ProtocolHandler)
								handler.AckStopSimulation(wss, ws, parsePacket)
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
// editor c2s Protocol Dispatcher
//------------------------------------------------------------------------
editor.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 9100
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
				if (handler instanceof editor.c2s_ProtocolHandler)
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
			case 2415569453: // ReqRunSimulation
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const parsePacket = {
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
						}
						handlers.forEach(handler => {
							if (handler instanceof editor.c2s_ProtocolHandler)
								handler.ReqRunSimulation(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof editor.c2s_ProtocolHandler)
								handler.ReqRunSimulation(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2272961170: // ReqRunProject
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const parsePacket = {
							projectName,
						}
						handlers.forEach(handler => {
							if (handler instanceof editor.c2s_ProtocolHandler)
								handler.ReqRunProject(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof editor.c2s_ProtocolHandler)
								handler.ReqRunProject(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3081823622: // ReqStopSimulation
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof editor.c2s_ProtocolHandler)
								handler.ReqStopSimulation(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof editor.c2s_ProtocolHandler)
								handler.ReqStopSimulation(wss, ws, parsePacket)
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
// editor s2c Protocol 
//------------------------------------------------------------------------
editor.s2c_Protocol = class {
	constructor() { }

	// Protocol: AckRunSimulation
	AckRunSimulation(ws, isBinary, projectName, mapFileName, pathFileName, nodeFileNames, taskName, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 9000, 207562622, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
				result,
			}
			WsSockServer.sendPacket(ws, 9000, 207562622, packet)
		}
	}
	
	// Protocol: AckRunProject
	AckRunProject(ws, isBinary, projectName, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(projectName)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 9000, 1279498047, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				result,
			}
			WsSockServer.sendPacket(ws, 9000, 1279498047, packet)
		}
	}
	
	// Protocol: AckStopSimulation
	AckStopSimulation(ws, isBinary, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 9000, 4276051529, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			WsSockServer.sendPacket(ws, 9000, 4276051529, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// editor c2s Protocol 
//------------------------------------------------------------------------
editor.c2s_Protocol = class {
	constructor() { }

	// Protocol: ReqRunSimulation
	ReqRunSimulation(ws, isBinary, projectName, mapFileName, pathFileName, nodeFileNames, taskName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			WsSockServer.sendPacketBinary(ws, 9100, 2415569453, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
			}
			WsSockServer.sendPacket(ws, 9100, 2415569453, packet)
		}
	}
	
	// Protocol: ReqRunProject
	ReqRunProject(ws, isBinary, projectName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(projectName)
			WsSockServer.sendPacketBinary(ws, 9100, 2272961170, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
			}
			WsSockServer.sendPacket(ws, 9100, 2272961170, packet)
		}
	}
	
	// Protocol: ReqStopSimulation
	ReqStopSimulation(ws, isBinary, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			WsSockServer.sendPacketBinary(ws, 9100, 3081823622, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			WsSockServer.sendPacket(ws, 9100, 3081823622, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// editor s2c Protocol Handler
//------------------------------------------------------------------------
editor.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	AckRunSimulation(wss, ws, packet) {}
	AckRunProject(wss, ws, packet) {}
	AckStopSimulation(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// editor c2s Protocol Handler
//------------------------------------------------------------------------
editor.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	ReqRunSimulation(wss, ws, packet) {}
	ReqRunProject(wss, ws, packet) {}
	ReqStopSimulation(wss, ws, packet) {}
}

