//------------------------------------------------------------------------
// Name:    cluster
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace clusterAsync {
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
	export type Welcome_Packet = {
		msg: string, 
	}
	export type AckLogin_Packet = {
		name: string, 
		result: number, 
	}
	export type AckLogout_Packet = {
		name: string, 
		result: number, 
	}
	export type AckRun_Packet = {
		projectName: string, 
		mapFileName: string, 
		pathFileName: string, 
		nodeFileNames: string[], 
		taskName: string, 
		result: number, 
	}
	export type AckStop_Packet = {
		result: number, 
	}
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
	export type RobotPath_Packet = {
		robotName: string, 
		path: Uint16Array | null, 
		wtimes: Uint16Array | null, 
	}
	export type AckLogFileName_Packet = {
		logFileName: string, 
	}
	export type ReqPing_Packet = {
		id: number, 
	}


	// c2s Protocol Packet Data
	export type ReqLogin_Packet = {
		name: string, 
	}
	export type ReqLogout_Packet = {
		name: string, 
	}
	export type ReqRun_Packet = {
		projectName: string, 
		mapFileName: string, 
		pathFileName: string, 
		nodeFileNames: string[], 
		taskName: string, 
	}
	export type ReqStop_Packet = {
	}
	export type ReqLogFileName_Packet = {
	}
	export type AckPing_Packet = {
		id: number, 
		result: number, 
	}


//------------------------------------------------------------------------
// clusterAsync s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(8200)
	}
	dispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {
		switch (packetId) {
			case 1281093745: // Welcome
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.Welcome(parsePacket)
					})
				}
				break;

			case 851424104: // AckLogin
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckLogin(parsePacket)
					})
				}
				break;

			case 2822050476: // AckLogout
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckLogout(parsePacket)
					})
				}
				break;

			case 4148808214: // AckRun
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckRun(parsePacket)
					})
				}
				break;

			case 114435221: // AckStop
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckStop(parsePacket)
					})
				}
				break;

			case 2663605951: // SpawnRobot
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.SpawnRobot(parsePacket)
					})
				}
				break;

			case 27410734: // RemoveRobot
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.RemoveRobot(parsePacket)
					})
				}
				break;

			case 2061430798: // RobotWorkFinish
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.RobotWorkFinish(parsePacket)
					})
				}
				break;

			case 1079519326: // RealtimeRobotState
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.RealtimeRobotState(parsePacket)
					})
				}
				break;

			case 896947492: // RobotPath
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.RobotPath(parsePacket)
					})
				}
				break;

			case 4096031841: // AckLogFileName
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckLogFileName(parsePacket)
					})
				}
				break;

			case 357726072: // ReqPing
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.ReqPing(parsePacket)
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
// clusterAsync c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(8300)
	}
	dispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {
		switch (packetId) {
			case 1956887904: // ReqLogin
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqLogin(parsePacket)
					})
				}
				break;

			case 1095604361: // ReqLogout
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqLogout(parsePacket)
					})
				}
				break;

			case 923151823: // ReqRun
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqRun(parsePacket)
					})
				}
				break;

			case 1453251868: // ReqStop
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqStop(parsePacket)
					})
				}
				break;

			case 3127197809: // ReqLogFileName
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqLogFileName(parsePacket)
					})
				}
				break;

			case 3275360863: // AckPing
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.AckPing(parsePacket)
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
// clusterAsync s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			this.node?.sendPacketBinary(8200, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(8200, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(8200, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(8200, 851424104, packet)
		}
	}
	
	// Protocol: AckLogout
	AckLogout(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(8200, 2822050476, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(8200, 2822050476, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(isBinary: boolean, projectName: string, mapFileName: string, pathFileName: string, nodeFileNames: string[], taskName: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(8200, 4148808214, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
				result,
			}
			this.node?.sendPacket(8200, 4148808214, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(isBinary: boolean, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(8200, 114435221, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			this.node?.sendPacket(8200, 114435221, packet)
		}
	}
	
	// Protocol: SpawnRobot
	SpawnRobot(isBinary: boolean, robotName: string, modelName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushStr(modelName)
			this.node?.sendPacketBinary(8200, 2663605951, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				modelName,
			}
			this.node?.sendPacket(8200, 2663605951, packet)
		}
	}
	
	// Protocol: RemoveRobot
	RemoveRobot(isBinary: boolean, robotName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			this.node?.sendPacketBinary(8200, 27410734, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
			}
			this.node?.sendPacket(8200, 27410734, packet)
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
			this.node?.sendPacketBinary(8200, 2061430798, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				taskId,
				totalWorkCount,
				finishWorkCount,
			}
			this.node?.sendPacket(8200, 2061430798, packet)
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
			this.node?.sendPacketBinary(8200, 1079519326, packet.buff, packet.offset)
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
			this.node?.sendPacket(8200, 1079519326, packet)
		}
	}
	
	// Protocol: RobotPath
	RobotPath(isBinary: boolean, robotName: string, path: number[], wtimes: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushUint16Array(path)
			packet.pushUint16Array(wtimes)
			this.node?.sendPacketBinary(8200, 896947492, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				path,
				wtimes,
			}
			this.node?.sendPacket(8200, 896947492, packet)
		}
	}
	
	// Protocol: AckLogFileName
	AckLogFileName(isBinary: boolean, logFileName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(logFileName)
			this.node?.sendPacketBinary(8200, 4096031841, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				logFileName,
			}
			this.node?.sendPacket(8200, 4096031841, packet)
		}
	}
	
	// Protocol: ReqPing
	ReqPing(isBinary: boolean, id: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(id)
			this.node?.sendPacketBinary(8200, 357726072, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			this.node?.sendPacket(8200, 357726072, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// clusterAsync c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqLogin
	ReqLogin(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(8300, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(8300, 1956887904, packet)
		}
	}
	
	// Protocol: ReqLogout
	ReqLogout(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(8300, 1095604361, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(8300, 1095604361, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(isBinary: boolean, projectName: string, mapFileName: string, pathFileName: string, nodeFileNames: string[], taskName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			this.node?.sendPacketBinary(8300, 923151823, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
			}
			this.node?.sendPacket(8300, 923151823, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(8300, 1453251868, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(8300, 1453251868, packet)
		}
	}
	
	// Protocol: ReqLogFileName
	ReqLogFileName(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(8300, 3127197809, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(8300, 3127197809, packet)
		}
	}
	
	// Protocol: AckPing
	AckPing(isBinary: boolean, id: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(id)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(8300, 3275360863, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			this.node?.sendPacket(8300, 3275360863, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// clusterAsync s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: clusterAsync.Welcome_Packet) => { }
	AckLogin = (packet: clusterAsync.AckLogin_Packet) => { }
	AckLogout = (packet: clusterAsync.AckLogout_Packet) => { }
	AckRun = (packet: clusterAsync.AckRun_Packet) => { }
	AckStop = (packet: clusterAsync.AckStop_Packet) => { }
	SpawnRobot = (packet: clusterAsync.SpawnRobot_Packet) => { }
	RemoveRobot = (packet: clusterAsync.RemoveRobot_Packet) => { }
	RobotWorkFinish = (packet: clusterAsync.RobotWorkFinish_Packet) => { }
	RealtimeRobotState = (packet: clusterAsync.RealtimeRobotState_Packet) => { }
	RobotPath = (packet: clusterAsync.RobotPath_Packet) => { }
	AckLogFileName = (packet: clusterAsync.AckLogFileName_Packet) => { }
	ReqPing = (packet: clusterAsync.ReqPing_Packet) => { }
}


//------------------------------------------------------------------------
// clusterAsync c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqLogin = (packet: clusterAsync.ReqLogin_Packet) => { }
	ReqLogout = (packet: clusterAsync.ReqLogout_Packet) => { }
	ReqRun = (packet: clusterAsync.ReqRun_Packet) => { }
	ReqStop = (packet: clusterAsync.ReqStop_Packet) => { }
	ReqLogFileName = (packet: clusterAsync.ReqLogFileName_Packet) => { }
	AckPing = (packet: clusterAsync.AckPing_Packet) => { }
}

}

