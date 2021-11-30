//------------------------------------------------------------------------
// Name:    controlserver
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace controlserverAsync {
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
	export type ReqStartCluster_Packet = {
		name: string, 
		url: string, 
		projectName: string, 
		mapFileName: string, 
		pathFileName: string, 
		nodeFileNames: string[], 
		taskName: string, 
	}
	export type ReqCloseCluster_Packet = {
		name: string, 
	}


	// c2s Protocol Packet Data
	export type ReqLogin_Packet = {
		name: string, 
	}
	export type ReqLogout_Packet = {
		name: string, 
	}
	export type AckStartCluster_Packet = {
		name: string, 
		url: string, 
		result: number, 
	}
	export type AckCloseCluster_Packet = {
		name: string, 
		result: number, 
	}
	export type RealtimeRobotState_Packet = {
		robotName: string, 
		state1: number, 
		state2: number, 
		state3: number, 
		state4: number, 
		pos: Vector3, 
	}


//------------------------------------------------------------------------
// controlserverAsync s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(6000)
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

			case 841234034: // ReqStartCluster
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.ReqStartCluster(parsePacket)
					})
				}
				break;

			case 2908314736: // ReqCloseCluster
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.ReqCloseCluster(parsePacket)
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
// controlserverAsync c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(6100)
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

			case 1748961021: // AckStartCluster
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.AckStartCluster(parsePacket)
					})
				}
				break;

			case 3666782746: // AckCloseCluster
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.AckCloseCluster(parsePacket)
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
// controlserverAsync s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			this.node?.sendPacketBinary(6000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(6000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(6000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(6000, 851424104, packet)
		}
	}
	
	// Protocol: AckLogout
	AckLogout(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(6000, 2822050476, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(6000, 2822050476, packet)
		}
	}
	
	// Protocol: ReqStartCluster
	ReqStartCluster(isBinary: boolean, name: string, url: string, projectName: string, mapFileName: string, pathFileName: string, nodeFileNames: string[], taskName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			this.node?.sendPacketBinary(6000, 841234034, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
			}
			this.node?.sendPacket(6000, 841234034, packet)
		}
	}
	
	// Protocol: ReqCloseCluster
	ReqCloseCluster(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(6000, 2908314736, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(6000, 2908314736, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// controlserverAsync c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqLogin
	ReqLogin(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(6100, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(6100, 1956887904, packet)
		}
	}
	
	// Protocol: ReqLogout
	ReqLogout(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(6100, 1095604361, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(6100, 1095604361, packet)
		}
	}
	
	// Protocol: AckStartCluster
	AckStartCluster(isBinary: boolean, name: string, url: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(6100, 1748961021, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
				result,
			}
			this.node?.sendPacket(6100, 1748961021, packet)
		}
	}
	
	// Protocol: AckCloseCluster
	AckCloseCluster(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(6100, 3666782746, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(6100, 3666782746, packet)
		}
	}
	
	// Protocol: RealtimeRobotState
	RealtimeRobotState(isBinary: boolean, robotName: string, state1: number, state2: number, state3: number, state4: number, pos: Vector3, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(robotName)
			packet.pushUint8(state1)
			packet.pushUint8(state2)
			packet.pushUint8(state3)
			packet.pushUint8(state4)
			Make_Vector3(packet,pos)
			this.node?.sendPacketBinary(6100, 1079519326, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				state1,
				state2,
				state3,
				state4,
				pos,
			}
			this.node?.sendPacket(6100, 1079519326, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// controlserverAsync s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: controlserverAsync.Welcome_Packet) => { }
	AckLogin = (packet: controlserverAsync.AckLogin_Packet) => { }
	AckLogout = (packet: controlserverAsync.AckLogout_Packet) => { }
	ReqStartCluster = (packet: controlserverAsync.ReqStartCluster_Packet) => { }
	ReqCloseCluster = (packet: controlserverAsync.ReqCloseCluster_Packet) => { }
}


//------------------------------------------------------------------------
// controlserverAsync c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqLogin = (packet: controlserverAsync.ReqLogin_Packet) => { }
	ReqLogout = (packet: controlserverAsync.ReqLogout_Packet) => { }
	AckStartCluster = (packet: controlserverAsync.AckStartCluster_Packet) => { }
	AckCloseCluster = (packet: controlserverAsync.AckCloseCluster_Packet) => { }
	RealtimeRobotState = (packet: controlserverAsync.RealtimeRobotState_Packet) => { }
}

}

