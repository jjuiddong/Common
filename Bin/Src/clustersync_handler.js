//------------------------------------------------------------------------
// Name:    clustersync
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import Dbg from "../../dbg/dbg";
import WsSockServer from "../wsserver";
import Packet from "../packet"

export default class clustersync {}

function Parse_Vector3(packet) {
	return {
			x: packet.getFloat32(),
			y: packet.getFloat32(),
			z: packet.getFloat32(),
	}
}
function Parse_Vector3Vector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_Vector3(packet))
	return ar
}
function Make_Vector3(packet, data) {
}
function Make_Vector3Vector(packet, data) {
}

//------------------------------------------------------------------------
// clustersync s2c Protocol Dispatcher
//------------------------------------------------------------------------
clustersync.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 8000
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
				if (handler instanceof clustersync.s2c_ProtocolHandler)
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
		default:
			Dbg.Log(0, 1, `RemoteDbg2 receive not defined packet bin:${option}, ${packetId}`)
			break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// clustersync c2s Protocol Dispatcher
//------------------------------------------------------------------------
clustersync.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 8100
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
				if (handler instanceof clustersync.c2s_ProtocolHandler)
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
			case 2663605951: // SpawnRobot
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const modelName = packet.getStr()
						const parsePacket = {
							robotName,
							modelName,
						}
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.SpawnRobot(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.SpawnRobot(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 27410734: // RemoveRobot
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const parsePacket = {
							robotName,
						}
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.RemoveRobot(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.RemoveRobot(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2061430798: // RobotWorkFinish
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const taskId = packet.getUint32()
						const totalWorkCount = packet.getUint32()
						const finishWorkCount = packet.getUint32()
						const parsePacket = {
							robotName,
							taskId,
							totalWorkCount,
							finishWorkCount,
						}
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.RobotWorkFinish(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.RobotWorkFinish(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1079519326: // RealtimeRobotState
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const state1 = packet.getUint8()
						const state2 = packet.getUint8()
						const state3 = packet.getUint8()
						const state4 = packet.getUint8()
						const pos = Parse_Vector3(packet)
						const dir = Parse_Vector3(packet)
						const parsePacket = {
							robotName,
							state1,
							state2,
							state3,
							state4,
							pos,
							dir,
						}
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.RealtimeRobotState(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof clustersync.c2s_ProtocolHandler)
								handler.RealtimeRobotState(wss, ws, parsePacket)
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
// clustersync s2c Protocol 
//------------------------------------------------------------------------
clustersync.s2c_Protocol = class {
	constructor() { }

}


//------------------------------------------------------------------------
// clustersync c2s Protocol 
//------------------------------------------------------------------------
clustersync.c2s_Protocol = class {
	constructor() { }

	// Protocol: SpawnRobot
	SpawnRobot(ws, isBinary, robotName, modelName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(robotName)
			packet.pushStr(modelName)
			WsSockServer.sendPacketBinary(ws, 8100, 2663605951, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				modelName,
			}
			WsSockServer.sendPacket(ws, 8100, 2663605951, packet)
		}
	}
	
	// Protocol: RemoveRobot
	RemoveRobot(ws, isBinary, robotName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(robotName)
			WsSockServer.sendPacketBinary(ws, 8100, 27410734, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
			}
			WsSockServer.sendPacket(ws, 8100, 27410734, packet)
		}
	}
	
	// Protocol: RobotWorkFinish
	RobotWorkFinish(ws, isBinary, robotName, taskId, totalWorkCount, finishWorkCount, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(robotName)
			packet.pushUint32(taskId)
			packet.pushUint32(totalWorkCount)
			packet.pushUint32(finishWorkCount)
			WsSockServer.sendPacketBinary(ws, 8100, 2061430798, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				taskId,
				totalWorkCount,
				finishWorkCount,
			}
			WsSockServer.sendPacket(ws, 8100, 2061430798, packet)
		}
	}
	
	// Protocol: RealtimeRobotState
	RealtimeRobotState(ws, isBinary, robotName, state1, state2, state3, state4, pos, dir, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(robotName)
			packet.pushUint8(state1)
			packet.pushUint8(state2)
			packet.pushUint8(state3)
			packet.pushUint8(state4)
			Make_Vector3(packet,pos)
			Make_Vector3(packet,dir)
			WsSockServer.sendPacketBinary(ws, 8100, 1079519326, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				state1,
				state2,
				state3,
				state4,
				pos,
				dir,
			}
			WsSockServer.sendPacket(ws, 8100, 1079519326, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// clustersync s2c Protocol Handler
//------------------------------------------------------------------------
clustersync.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
}


//------------------------------------------------------------------------
// clustersync c2s Protocol Handler
//------------------------------------------------------------------------
clustersync.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	SpawnRobot(wss, ws, packet) {}
	RemoveRobot(wss, ws, packet) {}
	RobotWorkFinish(wss, ws, packet) {}
	RealtimeRobotState(wss, ws, packet) {}
}

