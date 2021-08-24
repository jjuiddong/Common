//------------------------------------------------------------------------
// Name:    clustersync
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace clustersync {
	export type Vector3 = {
		x: number, 
		y: number, 
		z: number, 
	}
	function Parse_Vector3(packet: Network.Packet) : Vector3 {
		return {
			x: packet.getFloat32(),
			y: packet.getFloat32(),
			z: packet.getFloat32(),
		}
	}
	function Parse_Vector3Vector(packet: Network.Packet) : Vector3[] {
		const size = packet.getUint32()
		if (size == 0) return []
		let ar: Vector3[] = []
		for(let i=0; i < size; ++i)
			ar.push(Parse_Vector3(packet))
		return ar
	}
	function Make_Vector3(packet: Network.Packet, data: Vector3) {
	}
	function Make_Vector3Vector(packet: Network.Packet, data: Vector3[]) {
	}

	// s2c Protocol Packet Data


	// c2s Protocol Packet Data
	export type SpawnRobot_Packet = {
		robotName: string, 
		modelName: string, 
	}
	export type RemoveRobot_Packet = {
		robotName: string, 
	}
	export type RobotWorkFinish_Packet = {
		robotName: string, 
		taskId: number, 
		totalWorkCount: number, 
		finishWorkCount: number, 
	}
	export type RealtimeRobotState_Packet = {
		robotName: string, 
		state1: number, 
		state2: number, 
		state3: number, 
		state4: number, 
		pos: Vector3, 
		dir: Vector3, 
	}


//------------------------------------------------------------------------
// clustersync s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(8000)
	}
	dispatch(ws: WebSocket, packet: Network.Packet, handlers: Network.Handler[]) {
		if (!packet.buff) return
		packet.init()
		// packet format
		// | protocol id (4) | packet id (4) | packet length (4) | option (4) |
		const protocolId = packet.getUint32()
		const packetId = packet.getUint32()
		const packetLength = packet.getUint32()
		const option = packet.getUint32()

		switch (packetId) {
			default:
				console.log(`not found packet ${protocolId}, ${packetId}`)
				break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// clustersync c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(8100)
	}
	dispatch(ws: WebSocket, packet: Network.Packet, handlers: Network.Handler[]) {
		if (!packet.buff) return
		packet.init()
		// packet format
		// | protocol id (4) | packet id (4) | packet length (4) | option (4) |
		const protocolId = packet.getUint32()
		const packetId = packet.getUint32()
		const packetLength = packet.getUint32()
		const option = packet.getUint32()

		switch (packetId) {
			case 2663605951: // SpawnRobot
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const modelName = packet.getStr()
						const parsePacket: SpawnRobot_Packet = {
							robotName,
							modelName,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.SpawnRobot(parsePacket)
						})
					} else { // json?
						const parsePacket: SpawnRobot_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.SpawnRobot(parsePacket)
						})
					}
				}
				break;

			case 27410734: // RemoveRobot
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const parsePacket: RemoveRobot_Packet = {
							robotName,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.RemoveRobot(parsePacket)
						})
					} else { // json?
						const parsePacket: RemoveRobot_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.RemoveRobot(parsePacket)
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
						const parsePacket: RobotWorkFinish_Packet = {
							robotName,
							taskId,
							totalWorkCount,
							finishWorkCount,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.RobotWorkFinish(parsePacket)
						})
					} else { // json?
						const parsePacket: RobotWorkFinish_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.RobotWorkFinish(parsePacket)
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
						const parsePacket: RealtimeRobotState_Packet = {
							robotName,
							state1,
							state2,
							state3,
							state4,
							pos,
							dir,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.RealtimeRobotState(parsePacket)
						})
					} else { // json?
						const parsePacket: RealtimeRobotState_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.RealtimeRobotState(parsePacket)
						})
					}
				}
				break;

			default:
				console.log(`not found packet ${protocolId}, ${packetId}`)
				break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// clustersync s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

}


//------------------------------------------------------------------------
// clustersync c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: SpawnRobot
	SpawnRobot(isBinary: boolean, robotName: string, modelName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushStr(modelName)
			Network.sendPacketBinary(this.ws, 8100, 2663605951, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				modelName,
			}
			Network.sendPacket(this.ws, 8100, 2663605951, packet)
		}
	}
	
	// Protocol: RemoveRobot
	RemoveRobot(isBinary: boolean, robotName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			Network.sendPacketBinary(this.ws, 8100, 27410734, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
			}
			Network.sendPacket(this.ws, 8100, 27410734, packet)
		}
	}
	
	// Protocol: RobotWorkFinish
	RobotWorkFinish(isBinary: boolean, robotName: string, taskId: number, totalWorkCount: number, finishWorkCount: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushUint32(taskId)
			packet.pushUint32(totalWorkCount)
			packet.pushUint32(finishWorkCount)
			Network.sendPacketBinary(this.ws, 8100, 2061430798, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				taskId,
				totalWorkCount,
				finishWorkCount,
			}
			Network.sendPacket(this.ws, 8100, 2061430798, packet)
		}
	}
	
	// Protocol: RealtimeRobotState
	RealtimeRobotState(isBinary: boolean, robotName: string, state1: number, state2: number, state3: number, state4: number, pos: Vector3, dir: Vector3, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushUint8(state1)
			packet.pushUint8(state2)
			packet.pushUint8(state3)
			packet.pushUint8(state4)
			Make_Vector3(packet,pos)
			Make_Vector3(packet,dir)
			Network.sendPacketBinary(this.ws, 8100, 1079519326, packet.buff, packet.offset)
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
			Network.sendPacket(this.ws, 8100, 1079519326, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// clustersync s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

}


//------------------------------------------------------------------------
// clustersync c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	SpawnRobot = (packet: SpawnRobot_Packet) => { }
	RemoveRobot = (packet: RemoveRobot_Packet) => { }
	RobotWorkFinish = (packet: RobotWorkFinish_Packet) => { }
	RealtimeRobotState = (packet: RealtimeRobotState_Packet) => { }
}

}

