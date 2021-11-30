//------------------------------------------------------------------------
// Name:    clustersync
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace clustersyncAsync {
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
// clustersyncAsync s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(8000)
	}
	dispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {
		switch (packetId) {
			default:
				console.log(`not found packet ${protocolId}, ${packetId}`)
				break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// clustersyncAsync c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(8100)
	}
	dispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {
		switch (packetId) {
			case 2663605951: // SpawnRobot
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.SpawnRobot(parsePacket)
					})
				}
				break;

			case 27410734: // RemoveRobot
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.RemoveRobot(parsePacket)
					})
				}
				break;

			case 2061430798: // RobotWorkFinish
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.RobotWorkFinish(parsePacket)
					})
				}
				break;

			case 1079519326: // RealtimeRobotState
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.RealtimeRobotState(parsePacket)
					})
				}
				break;

			default:
				console.log(`not found packet ${protocolId}, ${packetId}`)
				break;
		}//~switch
	}//~dispatch()
}


//------------------------------------------------------------------------
// clustersyncAsync s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

}


//------------------------------------------------------------------------
// clustersyncAsync c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: SpawnRobot
	SpawnRobot(isBinary: boolean, robotName: string, modelName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushStr(modelName)
			this.node?.sendPacketBinary(8100, 2663605951, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				modelName,
			}
			this.node?.sendPacket(8100, 2663605951, packet)
		}
	}
	
	// Protocol: RemoveRobot
	RemoveRobot(isBinary: boolean, robotName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			this.node?.sendPacketBinary(8100, 27410734, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
			}
			this.node?.sendPacket(8100, 27410734, packet)
		}
	}
	
	// Protocol: RobotWorkFinish
	RobotWorkFinish(isBinary: boolean, robotName: string, taskId: number, totalWorkCount: number, finishWorkCount: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushUint32(taskId)
			packet.pushUint32(totalWorkCount)
			packet.pushUint32(finishWorkCount)
			this.node?.sendPacketBinary(8100, 2061430798, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				taskId,
				totalWorkCount,
				finishWorkCount,
			}
			this.node?.sendPacket(8100, 2061430798, packet)
		}
	}
	
	// Protocol: RealtimeRobotState
	RealtimeRobotState(isBinary: boolean, robotName: string, state1: number, state2: number, state3: number, state4: number, pos: Vector3, dir: Vector3, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushUint8(state1)
			packet.pushUint8(state2)
			packet.pushUint8(state3)
			packet.pushUint8(state4)
			Make_Vector3(packet,pos)
			Make_Vector3(packet,dir)
			this.node?.sendPacketBinary(8100, 1079519326, packet.buff, packet.offset)
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
			this.node?.sendPacket(8100, 1079519326, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// clustersyncAsync s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

}


//------------------------------------------------------------------------
// clustersyncAsync c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	SpawnRobot = (packet: clustersyncAsync.SpawnRobot_Packet) => { }
	RemoveRobot = (packet: clustersyncAsync.RemoveRobot_Packet) => { }
	RobotWorkFinish = (packet: clustersyncAsync.RobotWorkFinish_Packet) => { }
	RealtimeRobotState = (packet: clustersyncAsync.RealtimeRobotState_Packet) => { }
}

}

