//------------------------------------------------------------------------
// Name:    test
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import Dbg from "../../dbg/dbg";
import WsSockServer from "../wsserver";
import Packet from "../packet"

export default class test {}


//------------------------------------------------------------------------
// test s2c Protocol Dispatcher
//------------------------------------------------------------------------
test.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 100000
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
				if (handler instanceof test.s2c_ProtocolHandler)
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
			case 3846277350: // AckResult
				{
					if (option == 1) { // binary?
						const packetName = packet.getStr()
						const result = packet.getFloat32()
						const vars = packet.getMapStrArray()
						const parsePacket = {
							packetName,
							result,
							vars,
						}
						handlers.forEach(handler => {
							if (handler instanceof test.s2c_ProtocolHandler)
								handler.AckResult(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof test.s2c_ProtocolHandler)
								handler.AckResult(wss, ws, parsePacket)
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
// test c2s Protocol Dispatcher
//------------------------------------------------------------------------
test.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 100001
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
				if (handler instanceof test.c2s_ProtocolHandler)
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
			case 2900479863: // PacketName1
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const data = packet.getStr()
						const num = packet.getFloat32()
						const parsePacket = {
							id,
							data,
							num,
						}
						handlers.forEach(handler => {
							if (handler instanceof test.c2s_ProtocolHandler)
								handler.PacketName1(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof test.c2s_ProtocolHandler)
								handler.PacketName1(wss, ws, parsePacket)
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
// test s2c Protocol 
//------------------------------------------------------------------------
test.s2c_Protocol = class {
	constructor() { }

	// Protocol: AckResult
	AckResult(ws, isBinary, packetName, result, vars, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(packetName)
			packet.pushFloat32(result)
			packet.pushMapStrArray(vars)
			WsSockServer.sendPacketBinary(ws, 100000, 3846277350, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				packetName,
				result,
				vars,
			}
			WsSockServer.sendPacket(ws, 100000, 3846277350, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// test c2s Protocol 
//------------------------------------------------------------------------
test.c2s_Protocol = class {
	constructor() { }

	// Protocol: PacketName1
	PacketName1(ws, isBinary, id, data, num, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(id)
			packet.pushStr(data)
			packet.pushFloat32(num)
			WsSockServer.sendPacketBinary(ws, 100001, 2900479863, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				data,
				num,
			}
			WsSockServer.sendPacket(ws, 100001, 2900479863, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// test s2c Protocol Handler
//------------------------------------------------------------------------
test.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	AckResult(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// test c2s Protocol Handler
//------------------------------------------------------------------------
test.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	PacketName1(wss, ws, packet) {}
}

