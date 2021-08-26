//------------------------------------------------------------------------
// Name:    controlserver
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace controlserver {
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
// controlserver s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(6000)
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
			case 1281093745: // Welcome
				{
					if (option == 1) { // binary?
						const msg = packet.getStr()
						const parsePacket: Welcome_Packet = {
							msg,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					} else { // json?
						const parsePacket: Welcome_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					}
				}
				break;

			case 851424104: // AckLogin
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckLogin_Packet = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckLogin(parsePacket)
						})
					} else { // json?
						const parsePacket: AckLogin_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckLogin(parsePacket)
						})
					}
				}
				break;

			case 2822050476: // AckLogout
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckLogout_Packet = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckLogout(parsePacket)
						})
					} else { // json?
						const parsePacket: AckLogout_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckLogout(parsePacket)
						})
					}
				}
				break;

			case 841234034: // ReqStartCluster
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const url = packet.getStr()
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const taskName = packet.getStr()
						const parsePacket: ReqStartCluster_Packet = {
							name,
							url,
							projectName,
							mapFileName,
							pathFileName,
							taskName,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqStartCluster(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqStartCluster_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqStartCluster(parsePacket)
						})
					}
				}
				break;

			case 2908314736: // ReqCloseCluster
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const parsePacket: ReqCloseCluster_Packet = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqCloseCluster(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqCloseCluster_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqCloseCluster(parsePacket)
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
// controlserver c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(6100)
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
			case 1956887904: // ReqLogin
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const parsePacket: ReqLogin_Packet = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqLogin(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqLogin_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqLogin(parsePacket)
						})
					}
				}
				break;

			case 1095604361: // ReqLogout
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const parsePacket: ReqLogout_Packet = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqLogout(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqLogout_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqLogout(parsePacket)
						})
					}
				}
				break;

			case 1748961021: // AckStartCluster
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const url = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckStartCluster_Packet = {
							name,
							url,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckStartCluster(parsePacket)
						})
					} else { // json?
						const parsePacket: AckStartCluster_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckStartCluster(parsePacket)
						})
					}
				}
				break;

			case 3666782746: // AckCloseCluster
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckCloseCluster_Packet = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckCloseCluster(parsePacket)
						})
					} else { // json?
						const parsePacket: AckCloseCluster_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckCloseCluster(parsePacket)
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
						const parsePacket: RealtimeRobotState_Packet = {
							robotName,
							state1,
							state2,
							state3,
							state4,
							pos,
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
// controlserver s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			Network.sendPacketBinary(this.ws, 6000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			Network.sendPacket(this.ws, 6000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(isBinary: boolean, name: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 6000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			Network.sendPacket(this.ws, 6000, 851424104, packet)
		}
	}
	
	// Protocol: AckLogout
	AckLogout(isBinary: boolean, name: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 6000, 2822050476, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			Network.sendPacket(this.ws, 6000, 2822050476, packet)
		}
	}
	
	// Protocol: ReqStartCluster
	ReqStartCluster(isBinary: boolean, name: string, url: string, projectName: string, mapFileName: string, pathFileName: string, taskName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStr(taskName)
			Network.sendPacketBinary(this.ws, 6000, 841234034, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
				projectName,
				mapFileName,
				pathFileName,
				taskName,
			}
			Network.sendPacket(this.ws, 6000, 841234034, packet)
		}
	}
	
	// Protocol: ReqCloseCluster
	ReqCloseCluster(isBinary: boolean, name: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			Network.sendPacketBinary(this.ws, 6000, 2908314736, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			Network.sendPacket(this.ws, 6000, 2908314736, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// controlserver c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqLogin
	ReqLogin(isBinary: boolean, name: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			Network.sendPacketBinary(this.ws, 6100, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			Network.sendPacket(this.ws, 6100, 1956887904, packet)
		}
	}
	
	// Protocol: ReqLogout
	ReqLogout(isBinary: boolean, name: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			Network.sendPacketBinary(this.ws, 6100, 1095604361, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			Network.sendPacket(this.ws, 6100, 1095604361, packet)
		}
	}
	
	// Protocol: AckStartCluster
	AckStartCluster(isBinary: boolean, name: string, url: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 6100, 1748961021, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
				result,
			}
			Network.sendPacket(this.ws, 6100, 1748961021, packet)
		}
	}
	
	// Protocol: AckCloseCluster
	AckCloseCluster(isBinary: boolean, name: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 6100, 3666782746, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			Network.sendPacket(this.ws, 6100, 3666782746, packet)
		}
	}
	
	// Protocol: RealtimeRobotState
	RealtimeRobotState(isBinary: boolean, robotName: string, state1: number, state2: number, state3: number, state4: number, pos: Vector3, ) {
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
			Network.sendPacketBinary(this.ws, 6100, 1079519326, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				state1,
				state2,
				state3,
				state4,
				pos,
			}
			Network.sendPacket(this.ws, 6100, 1079519326, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// controlserver s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: Welcome_Packet) => { }
	AckLogin = (packet: AckLogin_Packet) => { }
	AckLogout = (packet: AckLogout_Packet) => { }
	ReqStartCluster = (packet: ReqStartCluster_Packet) => { }
	ReqCloseCluster = (packet: ReqCloseCluster_Packet) => { }
}


//------------------------------------------------------------------------
// controlserver c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqLogin = (packet: ReqLogin_Packet) => { }
	ReqLogout = (packet: ReqLogout_Packet) => { }
	AckStartCluster = (packet: AckStartCluster_Packet) => { }
	AckCloseCluster = (packet: AckCloseCluster_Packet) => { }
	RealtimeRobotState = (packet: RealtimeRobotState_Packet) => { }
}

}

