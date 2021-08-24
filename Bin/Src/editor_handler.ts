//------------------------------------------------------------------------
// Name:    editor
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace editor {

	// s2c Protocol Packet Data
	export type AckRunSimulation_Packet = {
		projectName: string, 
		mapFileName: string, 
		pathFileName: string, 
		nodeFileNames: string[], 
		taskName: string, 
		result: number, 
	}
	export type AckRunProject_Packet = {
		projectName: string, 
		result: number, 
	}
	export type AckStopSimulation_Packet = {
		result: number, 
	}


	// c2s Protocol Packet Data
	export type ReqRunSimulation_Packet = {
		projectName: string, 
		mapFileName: string, 
		pathFileName: string, 
		nodeFileNames: string[], 
		taskName: string, 
	}
	export type ReqRunProject_Packet = {
		projectName: string, 
	}
	export type ReqStopSimulation_Packet = {
	}


//------------------------------------------------------------------------
// editor s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(9000)
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
			case 207562622: // AckRunSimulation
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckRunSimulation_Packet = {
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckRunSimulation(parsePacket)
						})
					} else { // json?
						const parsePacket: AckRunSimulation_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckRunSimulation(parsePacket)
						})
					}
				}
				break;

			case 1279498047: // AckRunProject
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckRunProject_Packet = {
							projectName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckRunProject(parsePacket)
						})
					} else { // json?
						const parsePacket: AckRunProject_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckRunProject(parsePacket)
						})
					}
				}
				break;

			case 4276051529: // AckStopSimulation
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket: AckStopSimulation_Packet = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckStopSimulation(parsePacket)
						})
					} else { // json?
						const parsePacket: AckStopSimulation_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckStopSimulation(parsePacket)
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
// editor c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(9100)
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
			case 2415569453: // ReqRunSimulation
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const parsePacket: ReqRunSimulation_Packet = {
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqRunSimulation(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqRunSimulation_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqRunSimulation(parsePacket)
						})
					}
				}
				break;

			case 2272961170: // ReqRunProject
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const parsePacket: ReqRunProject_Packet = {
							projectName,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqRunProject(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqRunProject_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqRunProject(parsePacket)
						})
					}
				}
				break;

			case 3081823622: // ReqStopSimulation
				{
					if (option == 1) { // binary?
						const parsePacket: ReqStopSimulation_Packet = {
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqStopSimulation(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqStopSimulation_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.ReqStopSimulation(parsePacket)
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
// editor s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: AckRunSimulation
	AckRunSimulation(isBinary: boolean, projectName: string, mapFileName: string, pathFileName: string, nodeFileNames: string[], taskName: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 9000, 207562622, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
				result,
			}
			Network.sendPacket(this.ws, 9000, 207562622, packet)
		}
	}
	
	// Protocol: AckRunProject
	AckRunProject(isBinary: boolean, projectName: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(projectName)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 9000, 1279498047, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				result,
			}
			Network.sendPacket(this.ws, 9000, 1279498047, packet)
		}
	}
	
	// Protocol: AckStopSimulation
	AckStopSimulation(isBinary: boolean, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 9000, 4276051529, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			Network.sendPacket(this.ws, 9000, 4276051529, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// editor c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqRunSimulation
	ReqRunSimulation(isBinary: boolean, projectName: string, mapFileName: string, pathFileName: string, nodeFileNames: string[], taskName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			Network.sendPacketBinary(this.ws, 9100, 2415569453, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
				mapFileName,
				pathFileName,
				nodeFileNames,
				taskName,
			}
			Network.sendPacket(this.ws, 9100, 2415569453, packet)
		}
	}
	
	// Protocol: ReqRunProject
	ReqRunProject(isBinary: boolean, projectName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(projectName)
			Network.sendPacketBinary(this.ws, 9100, 2272961170, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				projectName,
			}
			Network.sendPacket(this.ws, 9100, 2272961170, packet)
		}
	}
	
	// Protocol: ReqStopSimulation
	ReqStopSimulation(isBinary: boolean, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Network.sendPacketBinary(this.ws, 9100, 3081823622, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			Network.sendPacket(this.ws, 9100, 3081823622, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// editor s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	AckRunSimulation = (packet: AckRunSimulation_Packet) => { }
	AckRunProject = (packet: AckRunProject_Packet) => { }
	AckStopSimulation = (packet: AckStopSimulation_Packet) => { }
}


//------------------------------------------------------------------------
// editor c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqRunSimulation = (packet: ReqRunSimulation_Packet) => { }
	ReqRunProject = (packet: ReqRunProject_Packet) => { }
	ReqStopSimulation = (packet: ReqStopSimulation_Packet) => { }
}

}

