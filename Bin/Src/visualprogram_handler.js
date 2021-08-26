//------------------------------------------------------------------------
// Name:    visualprogram
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import Dbg from "../../dbg/dbg";
import WsSockServer from "../wsserver";
import Packet from "../packet"

export default class visualprogram {}

function Parse_sNodeFile(packet) {
	return {
			name: packet.getStr(),
	}
}
function Parse_sNodeFileVector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_sNodeFile(packet))
	return ar
}
function Make_sNodeFile(packet, data) {
}
function Make_sNodeFileVector(packet, data) {
}

//------------------------------------------------------------------------
// visualprogram s2w Protocol Dispatcher
//------------------------------------------------------------------------
visualprogram.s2w_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 2100
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
				if (handler instanceof visualprogram.s2w_ProtocolHandler)
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
			case 1933022727: // AckWebLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.AckWebLogin(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.AckWebLogin(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4236112087: // AckWebRunVisualProgStream
				{
					if (option == 1) { // binary?
						const nodeFileName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							nodeFileName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.AckWebRunVisualProgStream(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.AckWebRunVisualProgStream(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4248820795: // AckWebStopVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.AckWebStopVisualProg(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.AckWebStopVisualProg(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2874116538: // ReqWebConnectRIProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const parsePacket = {
							url,
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.ReqWebConnectRIProxyServer(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2w_ProtocolHandler)
								handler.ReqWebConnectRIProxyServer(wss, ws, parsePacket)
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
// visualprogram w2s Protocol Dispatcher
//------------------------------------------------------------------------
visualprogram.w2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 2101
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
				if (handler instanceof visualprogram.w2s_ProtocolHandler)
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
			case 4200687688: // ReqWebLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const parsePacket = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.w2s_ProtocolHandler)
								handler.ReqWebLogin(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.w2s_ProtocolHandler)
								handler.ReqWebLogin(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 994683893: // ReqWebRunVisualProgStream
				{
					if (option == 1) { // binary?
						const nodeFileName = packet.getStr()
						const parsePacket = {
							nodeFileName,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.w2s_ProtocolHandler)
								handler.ReqWebRunVisualProgStream(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.w2s_ProtocolHandler)
								handler.ReqWebRunVisualProgStream(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1431744851: // ReqWebStopVisualProg
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.w2s_ProtocolHandler)
								handler.ReqWebStopVisualProg(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.w2s_ProtocolHandler)
								handler.ReqWebStopVisualProg(wss, ws, parsePacket)
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
// visualprogram s2c Protocol Dispatcher
//------------------------------------------------------------------------
visualprogram.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 1000
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
				if (handler instanceof visualprogram.s2c_ProtocolHandler)
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
			case 1281093745: // Welcome
				{
					if (option == 1) { // binary?
						const msg = packet.getStr()
						const parsePacket = {
							msg,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.Welcome(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.Welcome(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 851424104: // AckLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.AckLogin(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.AckLogin(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2250021743: // ReqRunVisualProg
				{
					if (option == 1) { // binary?
						const nodeFile = Parse_sNodeFile(packet)
						const parsePacket = {
							nodeFile,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqRunVisualProg(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqRunVisualProg(wss, ws, parsePacket)
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
						const parsePacket = {
							count,
							index,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqRunVisualProgStream(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqRunVisualProgStream(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3721131371: // ReqRunVisualProgFiles
				{
					if (option == 1) { // binary?
						const nodeFileNames = packet.getStrArray()
						const parsePacket = {
							nodeFileNames,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqRunVisualProgFiles(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqRunVisualProgFiles(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 4258374867: // ReqStopVisualProg
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqStopVisualProg(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqStopVisualProg(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3492118321: // ReqConnectRIProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const parsePacket = {
							url,
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqConnectRIProxyServer(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.s2c_ProtocolHandler)
								handler.ReqConnectRIProxyServer(wss, ws, parsePacket)
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
// visualprogram c2s Protocol Dispatcher
//------------------------------------------------------------------------
visualprogram.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 2000
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
				if (handler instanceof visualprogram.c2s_ProtocolHandler)
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
			case 1956887904: // ReqLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const parsePacket = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.ReqLogin(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.ReqLogin(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3863877132: // AckRunVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckRunVisualProg(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckRunVisualProg(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3454830338: // AckRunVisualProgStream
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckRunVisualProgStream(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckRunVisualProgStream(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2713087572: // AckRunVisualProgFiles
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckRunVisualProgFiles(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckRunVisualProgFiles(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1895439953: // AckStopVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckStopVisualProg(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckStopVisualProg(wss, ws, parsePacket)
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
						const parsePacket = {
							url,
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckConnectRIProxyServer(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof visualprogram.c2s_ProtocolHandler)
								handler.AckConnectRIProxyServer(wss, ws, parsePacket)
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
// visualprogram s2w Protocol 
//------------------------------------------------------------------------
visualprogram.s2w_Protocol = class {
	constructor() { }

	// Protocol: AckWebLogin
	AckWebLogin(ws, isBinary, id, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(id)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2100, 1933022727, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			WsSockServer.sendPacket(ws, 2100, 1933022727, packet)
		}
	}
	
	// Protocol: AckWebRunVisualProgStream
	AckWebRunVisualProgStream(ws, isBinary, nodeFileName, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(nodeFileName)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2100, 4236112087, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileName,
				result,
			}
			WsSockServer.sendPacket(ws, 2100, 4236112087, packet)
		}
	}
	
	// Protocol: AckWebStopVisualProg
	AckWebStopVisualProg(ws, isBinary, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2100, 4248820795, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			WsSockServer.sendPacket(ws, 2100, 4248820795, packet)
		}
	}
	
	// Protocol: ReqWebConnectRIProxyServer
	ReqWebConnectRIProxyServer(ws, isBinary, url, name, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			WsSockServer.sendPacketBinary(ws, 2100, 2874116538, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
			}
			WsSockServer.sendPacket(ws, 2100, 2874116538, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram w2s Protocol 
//------------------------------------------------------------------------
visualprogram.w2s_Protocol = class {
	constructor() { }

	// Protocol: ReqWebLogin
	ReqWebLogin(ws, isBinary, id, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(id)
			WsSockServer.sendPacketBinary(ws, 2101, 4200687688, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			WsSockServer.sendPacket(ws, 2101, 4200687688, packet)
		}
	}
	
	// Protocol: ReqWebRunVisualProgStream
	ReqWebRunVisualProgStream(ws, isBinary, nodeFileName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(nodeFileName)
			WsSockServer.sendPacketBinary(ws, 2101, 994683893, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileName,
			}
			WsSockServer.sendPacket(ws, 2101, 994683893, packet)
		}
	}
	
	// Protocol: ReqWebStopVisualProg
	ReqWebStopVisualProg(ws, isBinary, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			WsSockServer.sendPacketBinary(ws, 2101, 1431744851, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			WsSockServer.sendPacket(ws, 2101, 1431744851, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram s2c Protocol 
//------------------------------------------------------------------------
visualprogram.s2c_Protocol = class {
	constructor() { }

	// Protocol: Welcome
	Welcome(ws, isBinary, msg, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(msg)
			WsSockServer.sendPacketBinary(ws, 1000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			WsSockServer.sendPacket(ws, 1000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(ws, isBinary, id, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(id)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 1000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			WsSockServer.sendPacket(ws, 1000, 851424104, packet)
		}
	}
	
	// Protocol: ReqRunVisualProg
	ReqRunVisualProg(ws, isBinary, nodeFile, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			Make_sNodeFile(packet,nodeFile)
			WsSockServer.sendPacketBinary(ws, 1000, 2250021743, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFile,
			}
			WsSockServer.sendPacket(ws, 1000, 2250021743, packet)
		}
	}
	
	// Protocol: ReqRunVisualProgStream
	ReqRunVisualProgStream(ws, isBinary, count, index, data, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushUint32(count)
			packet.pushUint32(index)
			packet.pushUint8Array(data)
			WsSockServer.sendPacketBinary(ws, 1000, 3686541167, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				count,
				index,
				data,
			}
			WsSockServer.sendPacket(ws, 1000, 3686541167, packet)
		}
	}
	
	// Protocol: ReqRunVisualProgFiles
	ReqRunVisualProgFiles(ws, isBinary, nodeFileNames, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStrArray(nodeFileNames)
			WsSockServer.sendPacketBinary(ws, 1000, 3721131371, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileNames,
			}
			WsSockServer.sendPacket(ws, 1000, 3721131371, packet)
		}
	}
	
	// Protocol: ReqStopVisualProg
	ReqStopVisualProg(ws, isBinary, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			WsSockServer.sendPacketBinary(ws, 1000, 4258374867, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			WsSockServer.sendPacket(ws, 1000, 4258374867, packet)
		}
	}
	
	// Protocol: ReqConnectRIProxyServer
	ReqConnectRIProxyServer(ws, isBinary, url, name, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			WsSockServer.sendPacketBinary(ws, 1000, 3492118321, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
			}
			WsSockServer.sendPacket(ws, 1000, 3492118321, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram c2s Protocol 
//------------------------------------------------------------------------
visualprogram.c2s_Protocol = class {
	constructor() { }

	// Protocol: ReqLogin
	ReqLogin(ws, isBinary, id, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(id)
			WsSockServer.sendPacketBinary(ws, 2000, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			WsSockServer.sendPacket(ws, 2000, 1956887904, packet)
		}
	}
	
	// Protocol: AckRunVisualProg
	AckRunVisualProg(ws, isBinary, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2000, 3863877132, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			WsSockServer.sendPacket(ws, 2000, 3863877132, packet)
		}
	}
	
	// Protocol: AckRunVisualProgStream
	AckRunVisualProgStream(ws, isBinary, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2000, 3454830338, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			WsSockServer.sendPacket(ws, 2000, 3454830338, packet)
		}
	}
	
	// Protocol: AckRunVisualProgFiles
	AckRunVisualProgFiles(ws, isBinary, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2000, 2713087572, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			WsSockServer.sendPacket(ws, 2000, 2713087572, packet)
		}
	}
	
	// Protocol: AckStopVisualProg
	AckStopVisualProg(ws, isBinary, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2000, 1895439953, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			WsSockServer.sendPacket(ws, 2000, 1895439953, packet)
		}
	}
	
	// Protocol: AckConnectRIProxyServer
	AckConnectRIProxyServer(ws, isBinary, url, name, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 2000, 3286171390, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
				result,
			}
			WsSockServer.sendPacket(ws, 2000, 3286171390, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// visualprogram s2w Protocol Handler
//------------------------------------------------------------------------
visualprogram.s2w_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	AckWebLogin(wss, ws, packet) {}
	AckWebRunVisualProgStream(wss, ws, packet) {}
	AckWebStopVisualProg(wss, ws, packet) {}
	ReqWebConnectRIProxyServer(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// visualprogram w2s Protocol Handler
//------------------------------------------------------------------------
visualprogram.w2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	ReqWebLogin(wss, ws, packet) {}
	ReqWebRunVisualProgStream(wss, ws, packet) {}
	ReqWebStopVisualProg(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// visualprogram s2c Protocol Handler
//------------------------------------------------------------------------
visualprogram.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Welcome(wss, ws, packet) {}
	AckLogin(wss, ws, packet) {}
	ReqRunVisualProg(wss, ws, packet) {}
	ReqRunVisualProgStream(wss, ws, packet) {}
	ReqRunVisualProgFiles(wss, ws, packet) {}
	ReqStopVisualProg(wss, ws, packet) {}
	ReqConnectRIProxyServer(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// visualprogram c2s Protocol Handler
//------------------------------------------------------------------------
visualprogram.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	ReqLogin(wss, ws, packet) {}
	AckRunVisualProg(wss, ws, packet) {}
	AckRunVisualProgStream(wss, ws, packet) {}
	AckRunVisualProgFiles(wss, ws, packet) {}
	AckStopVisualProg(wss, ws, packet) {}
	AckConnectRIProxyServer(wss, ws, packet) {}
}

