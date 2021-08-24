//------------------------------------------------------------------------
// Name:    visualprogram
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace visualprogram {
	export type sNodeFile = {
		name: string, 
	}
	function Parse_sNodeFile(packet: Network.Packet) : sNodeFile {
		return {
			name: packet.getStr(),
		}
	}
	function Parse_sNodeFileVector(packet: Network.Packet) : sNodeFile[] {
		const size = packet.getUint32()
		if (size == 0) return []
		let ar: sNodeFile[] = []
		for(let i=0; i < size; ++i)
			ar.push(Parse_sNodeFile(packet))
		return ar
	}
	function Make_sNodeFile(packet: Network.Packet, data: sNodeFile) {
	}
	function Make_sNodeFileVector(packet: Network.Packet, data: sNodeFile[]) {
	}

	// s2w Protocol Packet Data
	export type AckWebLogin_Packet = {
		id: string, 
		result: number, 
	}
	export type AckWebRunVisualProgStream_Packet = {
		nodeFileName: string, 
		result: number, 
	}
	export type AckWebStopVisualProg_Packet = {
		result: number, 
	}
	export type ReqWebConnectRIProxyServer_Packet = {
		url: string, 
		name: string, 
	}


	// w2s Protocol Packet Data
	export type ReqWebLogin_Packet = {
		id: string, 
	}
	export type ReqWebRunVisualProgStream_Packet = {
		nodeFileName: string, 
	}
	export type ReqWebStopVisualProg_Packet = {
	}


	// s2c Protocol Packet Data
	export type Welcome_Packet = {
		msg: string, 
	}
	export type AckLogin_Packet = {
		id: string, 
		result: number, 
	}
	export type ReqRunVisualProg_Packet = {
		nodeFile: sNodeFile, 
	}
	export type ReqRunVisualProgStream_Packet = {
		count: number, 
		index: number, 
		data: Uint8Array | null, 
	}
	export type ReqRunVisualProgFiles_Packet = {
		nodeFileNames: string[], 
	}
	export type ReqStopVisualProg_Packet = {
	}
	export type ReqConnectRIProxyServer_Packet = {
		url: string, 
		name: string, 
	}


	// c2s Protocol Packet Data
	export type ReqLogin_Packet = {
		id: string, 
	}
	export type AckRunVisualProg_Packet = {
		result: number, 
	}
	export type AckRunVisualProgStream_Packet = {
		result: number, 
	}
	export type AckRunVisualProgFiles_Packet = {
		result: number, 
	}
	export type AckStopVisualProg_Packet = {
		result: number, 
	}
	export type AckConnectRIProxyServer_Packet = {
		url: string, 
		name: string, 
		result: number, 
	}


//------------------------------------------------------------------------
// visualprogram s2w Protocol Dispatcher
//------------------------------------------------------------------------
export class s2w_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(2100)
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
			case 1933022727: // AckWebLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckWebLogin_Packet = {
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.AckWebLogin(parsePacket)
						})
					} else { // json?
						const parsePacket: AckWebLogin_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.AckWebLogin(parsePacket)
						})
					}
				}
				break;

			case 4236112087: // AckWebRunVisualProgStream
				{
					if (option == 1) { // binary?
						const nodeFileName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckWebRunVisualProgStream_Packet = {
							nodeFileName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.AckWebRunVisualProgStream(parsePacket)
						})
					} else { // json?
						const parsePacket: AckWebRunVisualProgStream_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.AckWebRunVisualProgStream(parsePacket)
						})
					}
				}
				break;

			case 4248820795: // AckWebStopVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket: AckWebStopVisualProg_Packet = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.AckWebStopVisualProg(parsePacket)
						})
					} else { // json?
						const parsePacket: AckWebStopVisualProg_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.AckWebStopVisualProg(parsePacket)
						})
					}
				}
				break;

			case 2874116538: // ReqWebConnectRIProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const parsePacket: ReqWebConnectRIProxyServer_Packet = {
							url,
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.ReqWebConnectRIProxyServer(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqWebConnectRIProxyServer_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2w_ProtocolHandler)
								handler.ReqWebConnectRIProxyServer(parsePacket)
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
// visualprogram w2s Protocol Dispatcher
//------------------------------------------------------------------------
export class w2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(2101)
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
			case 4200687688: // ReqWebLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const parsePacket: ReqWebLogin_Packet = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof w2s_ProtocolHandler)
								handler.ReqWebLogin(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqWebLogin_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof w2s_ProtocolHandler)
								handler.ReqWebLogin(parsePacket)
						})
					}
				}
				break;

			case 994683893: // ReqWebRunVisualProgStream
				{
					if (option == 1) { // binary?
						const nodeFileName = packet.getStr()
						const parsePacket: ReqWebRunVisualProgStream_Packet = {
							nodeFileName,
						}
						handlers.forEach(handler => {
							if (handler instanceof w2s_ProtocolHandler)
								handler.ReqWebRunVisualProgStream(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqWebRunVisualProgStream_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof w2s_ProtocolHandler)
								handler.ReqWebRunVisualProgStream(parsePacket)
						})
					}
				}
				break;

			case 1431744851: // ReqWebStopVisualProg
				{
					if (option == 1) { // binary?
						const parsePacket: ReqWebStopVisualProg_Packet = {
						}
						handlers.forEach(handler => {
							if (handler instanceof w2s_ProtocolHandler)
								handler.ReqWebStopVisualProg(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqWebStopVisualProg_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof w2s_ProtocolHandler)
								handler.ReqWebStopVisualProg(parsePacket)
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
// visualprogram s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(1000)
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
						const id = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckLogin_Packet = {
							id,
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

			case 2250021743: // ReqRunVisualProg
				{
					if (option == 1) { // binary?
						const nodeFile = Parse_sNodeFile(packet)
						const parsePacket: ReqRunVisualProg_Packet = {
							nodeFile,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqRunVisualProg(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqRunVisualProg_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqRunVisualProg(parsePacket)
						})
					}
				}
				break;

			case 3686541167: // ReqRunVisualProgStream
				{
					if (option == 1) { // binary?
						const count = packet.getUint32()
						const index = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket: ReqRunVisualProgStream_Packet = {
							count,
							index,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqRunVisualProgStream(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqRunVisualProgStream_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqRunVisualProgStream(parsePacket)
						})
					}
				}
				break;

			case 3721131371: // ReqRunVisualProgFiles
				{
					if (option == 1) { // binary?
						const nodeFileNames = packet.getStrArray()
						const parsePacket: ReqRunVisualProgFiles_Packet = {
							nodeFileNames,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqRunVisualProgFiles(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqRunVisualProgFiles_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqRunVisualProgFiles(parsePacket)
						})
					}
				}
				break;

			case 4258374867: // ReqStopVisualProg
				{
					if (option == 1) { // binary?
						const parsePacket: ReqStopVisualProg_Packet = {
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqStopVisualProg(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqStopVisualProg_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqStopVisualProg(parsePacket)
						})
					}
				}
				break;

			case 3492118321: // ReqConnectRIProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const parsePacket: ReqConnectRIProxyServer_Packet = {
							url,
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqConnectRIProxyServer(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqConnectRIProxyServer_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.ReqConnectRIProxyServer(parsePacket)
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
// visualprogram c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(2000)
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
						const id = packet.getStr()
						const parsePacket: ReqLogin_Packet = {
							id,
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

			case 3863877132: // AckRunVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket: AckRunVisualProg_Packet = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckRunVisualProg(parsePacket)
						})
					} else { // json?
						const parsePacket: AckRunVisualProg_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckRunVisualProg(parsePacket)
						})
					}
				}
				break;

			case 3454830338: // AckRunVisualProgStream
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket: AckRunVisualProgStream_Packet = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckRunVisualProgStream(parsePacket)
						})
					} else { // json?
						const parsePacket: AckRunVisualProgStream_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckRunVisualProgStream(parsePacket)
						})
					}
				}
				break;

			case 2713087572: // AckRunVisualProgFiles
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket: AckRunVisualProgFiles_Packet = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckRunVisualProgFiles(parsePacket)
						})
					} else { // json?
						const parsePacket: AckRunVisualProgFiles_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckRunVisualProgFiles(parsePacket)
						})
					}
				}
				break;

			case 1895439953: // AckStopVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket: AckStopVisualProg_Packet = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckStopVisualProg(parsePacket)
						})
					} else { // json?
						const parsePacket: AckStopVisualProg_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckStopVisualProg(parsePacket)
						})
					}
				}
				break;

			case 3286171390: // AckConnectRIProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckConnectRIProxyServer_Packet = {
							url,
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckConnectRIProxyServer(parsePacket)
						})
					} else { // json?
						const parsePacket: AckConnectRIProxyServer_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.AckConnectRIProxyServer(parsePacket)
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
// visualprogram s2w Protocol 
//------------------------------------------------------------------------
export class s2w_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: AckWebLogin
	AckWebLogin(isBinary: boolean, id: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2100, 1933022727, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			Network.sendPacket(this.ws, 2100, 1933022727, packet)
		}
	}
	
	// Protocol: AckWebRunVisualProgStream
	AckWebRunVisualProgStream(isBinary: boolean, nodeFileName: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(nodeFileName)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2100, 4236112087, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileName,
				result,
			}
			Network.sendPacket(this.ws, 2100, 4236112087, packet)
		}
	}
	
	// Protocol: AckWebStopVisualProg
	AckWebStopVisualProg(isBinary: boolean, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2100, 4248820795, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			Network.sendPacket(this.ws, 2100, 4248820795, packet)
		}
	}
	
	// Protocol: ReqWebConnectRIProxyServer
	ReqWebConnectRIProxyServer(isBinary: boolean, url: string, name: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			Network.sendPacketBinary(this.ws, 2100, 2874116538, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
			}
			Network.sendPacket(this.ws, 2100, 2874116538, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram w2s Protocol 
//------------------------------------------------------------------------
export class w2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqWebLogin
	ReqWebLogin(isBinary: boolean, id: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			Network.sendPacketBinary(this.ws, 2101, 4200687688, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			Network.sendPacket(this.ws, 2101, 4200687688, packet)
		}
	}
	
	// Protocol: ReqWebRunVisualProgStream
	ReqWebRunVisualProgStream(isBinary: boolean, nodeFileName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(nodeFileName)
			Network.sendPacketBinary(this.ws, 2101, 994683893, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileName,
			}
			Network.sendPacket(this.ws, 2101, 994683893, packet)
		}
	}
	
	// Protocol: ReqWebStopVisualProg
	ReqWebStopVisualProg(isBinary: boolean, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Network.sendPacketBinary(this.ws, 2101, 1431744851, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			Network.sendPacket(this.ws, 2101, 1431744851, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram s2c Protocol 
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
			Network.sendPacketBinary(this.ws, 1000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			Network.sendPacket(this.ws, 1000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(isBinary: boolean, id: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 1000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			Network.sendPacket(this.ws, 1000, 851424104, packet)
		}
	}
	
	// Protocol: ReqRunVisualProg
	ReqRunVisualProg(isBinary: boolean, nodeFile: sNodeFile, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Make_sNodeFile(packet,nodeFile)
			Network.sendPacketBinary(this.ws, 1000, 2250021743, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFile,
			}
			Network.sendPacket(this.ws, 1000, 2250021743, packet)
		}
	}
	
	// Protocol: ReqRunVisualProgStream
	ReqRunVisualProgStream(isBinary: boolean, count: number, index: number, data: number[], ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushUint32(count)
			packet.pushUint32(index)
			packet.pushUint8Array(data)
			Network.sendPacketBinary(this.ws, 1000, 3686541167, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				count,
				index,
				data,
			}
			Network.sendPacket(this.ws, 1000, 3686541167, packet)
		}
	}
	
	// Protocol: ReqRunVisualProgFiles
	ReqRunVisualProgFiles(isBinary: boolean, nodeFileNames: string[], ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStrArray(nodeFileNames)
			Network.sendPacketBinary(this.ws, 1000, 3721131371, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileNames,
			}
			Network.sendPacket(this.ws, 1000, 3721131371, packet)
		}
	}
	
	// Protocol: ReqStopVisualProg
	ReqStopVisualProg(isBinary: boolean, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Network.sendPacketBinary(this.ws, 1000, 4258374867, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			Network.sendPacket(this.ws, 1000, 4258374867, packet)
		}
	}
	
	// Protocol: ReqConnectRIProxyServer
	ReqConnectRIProxyServer(isBinary: boolean, url: string, name: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			Network.sendPacketBinary(this.ws, 1000, 3492118321, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
			}
			Network.sendPacket(this.ws, 1000, 3492118321, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqLogin
	ReqLogin(isBinary: boolean, id: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			Network.sendPacketBinary(this.ws, 2000, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			Network.sendPacket(this.ws, 2000, 1956887904, packet)
		}
	}
	
	// Protocol: AckRunVisualProg
	AckRunVisualProg(isBinary: boolean, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2000, 3863877132, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			Network.sendPacket(this.ws, 2000, 3863877132, packet)
		}
	}
	
	// Protocol: AckRunVisualProgStream
	AckRunVisualProgStream(isBinary: boolean, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2000, 3454830338, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			Network.sendPacket(this.ws, 2000, 3454830338, packet)
		}
	}
	
	// Protocol: AckRunVisualProgFiles
	AckRunVisualProgFiles(isBinary: boolean, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2000, 2713087572, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			Network.sendPacket(this.ws, 2000, 2713087572, packet)
		}
	}
	
	// Protocol: AckStopVisualProg
	AckStopVisualProg(isBinary: boolean, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2000, 1895439953, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			Network.sendPacket(this.ws, 2000, 1895439953, packet)
		}
	}
	
	// Protocol: AckConnectRIProxyServer
	AckConnectRIProxyServer(isBinary: boolean, url: string, name: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 2000, 3286171390, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
				result,
			}
			Network.sendPacket(this.ws, 2000, 3286171390, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram s2w Protocol Handler
//------------------------------------------------------------------------
export class s2w_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	AckWebLogin = (packet: AckWebLogin_Packet) => { }
	AckWebRunVisualProgStream = (packet: AckWebRunVisualProgStream_Packet) => { }
	AckWebStopVisualProg = (packet: AckWebStopVisualProg_Packet) => { }
	ReqWebConnectRIProxyServer = (packet: ReqWebConnectRIProxyServer_Packet) => { }
}


//------------------------------------------------------------------------
// visualprogram w2s Protocol Handler
//------------------------------------------------------------------------
export class w2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqWebLogin = (packet: ReqWebLogin_Packet) => { }
	ReqWebRunVisualProgStream = (packet: ReqWebRunVisualProgStream_Packet) => { }
	ReqWebStopVisualProg = (packet: ReqWebStopVisualProg_Packet) => { }
}


//------------------------------------------------------------------------
// visualprogram s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: Welcome_Packet) => { }
	AckLogin = (packet: AckLogin_Packet) => { }
	ReqRunVisualProg = (packet: ReqRunVisualProg_Packet) => { }
	ReqRunVisualProgStream = (packet: ReqRunVisualProgStream_Packet) => { }
	ReqRunVisualProgFiles = (packet: ReqRunVisualProgFiles_Packet) => { }
	ReqStopVisualProg = (packet: ReqStopVisualProg_Packet) => { }
	ReqConnectRIProxyServer = (packet: ReqConnectRIProxyServer_Packet) => { }
}


//------------------------------------------------------------------------
// visualprogram c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqLogin = (packet: ReqLogin_Packet) => { }
	AckRunVisualProg = (packet: AckRunVisualProg_Packet) => { }
	AckRunVisualProgStream = (packet: AckRunVisualProgStream_Packet) => { }
	AckRunVisualProgFiles = (packet: AckRunVisualProgFiles_Packet) => { }
	AckStopVisualProg = (packet: AckStopVisualProg_Packet) => { }
	AckConnectRIProxyServer = (packet: AckConnectRIProxyServer_Packet) => { }
}

}

