
export default class output {}

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
// output s2c Protocol Dispatcher
//------------------------------------------------------------------------
output.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 6000
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
				if (handler instanceof output.s2c_ProtocolHandler)
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
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.Welcome(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.Welcome(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 851424104: // AckLogin
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.AckLogin(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.AckLogin(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2822050476: // AckLogout
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.AckLogout(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.AckLogout(wss, ws, parsePacket)
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
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const parsePacket = {
							name,
							url,
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.ReqStartCluster(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.ReqStartCluster(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 2908314736: // ReqCloseCluster
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const parsePacket = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.ReqCloseCluster(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.s2c_ProtocolHandler)
								handler.ReqCloseCluster(wss, ws, parsePacket)
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
// output c2s Protocol Dispatcher
//------------------------------------------------------------------------
output.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 6100
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
				if (handler instanceof output.c2s_ProtocolHandler)
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
						const name = packet.getStr()
						const parsePacket = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.ReqLogin(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.ReqLogin(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 1095604361: // ReqLogout
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const parsePacket = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.ReqLogout(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.ReqLogout(wss, ws, parsePacket)
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
						const parsePacket = {
							name,
							url,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.AckStartCluster(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.AckStartCluster(wss, ws, parsePacket)
						})
					}
				}
				break;

			case 3666782746: // AckCloseCluster
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.AckCloseCluster(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.AckCloseCluster(wss, ws, parsePacket)
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
						const parsePacket = {
							robotName,
							state1,
							state2,
							state3,
							state4,
							pos,
						}
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
								handler.RealtimeRobotState(wss, ws, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof output.c2s_ProtocolHandler)
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
// output s2c Protocol 
//------------------------------------------------------------------------
output.s2c_Protocol = class {
	constructor() { }

	// Protocol: Welcome
	Welcome(ws, isBinary, msg, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(msg)
			WsSockServer.sendPacketBinary(ws, 6000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			WsSockServer.sendPacket(ws, 6000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(ws, isBinary, name, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 6000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			WsSockServer.sendPacket(ws, 6000, 851424104, packet)
		}
	}
	
	// Protocol: AckLogout
	AckLogout(ws, isBinary, name, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 6000, 2822050476, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			WsSockServer.sendPacket(ws, 6000, 2822050476, packet)
		}
	}
	
	// Protocol: ReqStartCluster
	ReqStartCluster(ws, isBinary, name, url, projectName, mapFileName, pathFileName, nodeFileNames, taskName, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushStr(projectName)
			packet.pushStr(mapFileName)
			packet.pushStr(pathFileName)
			packet.pushStrArray(nodeFileNames)
			packet.pushStr(taskName)
			WsSockServer.sendPacketBinary(ws, 6000, 841234034, packet.buff, packet.offset)
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
			WsSockServer.sendPacket(ws, 6000, 841234034, packet)
		}
	}
	
	// Protocol: ReqCloseCluster
	ReqCloseCluster(ws, isBinary, name, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			WsSockServer.sendPacketBinary(ws, 6000, 2908314736, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			WsSockServer.sendPacket(ws, 6000, 2908314736, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// output c2s Protocol 
//------------------------------------------------------------------------
output.c2s_Protocol = class {
	constructor() { }

	// Protocol: ReqLogin
	ReqLogin(ws, isBinary, name, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			WsSockServer.sendPacketBinary(ws, 6100, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			WsSockServer.sendPacket(ws, 6100, 1956887904, packet)
		}
	}
	
	// Protocol: ReqLogout
	ReqLogout(ws, isBinary, name, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			WsSockServer.sendPacketBinary(ws, 6100, 1095604361, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			WsSockServer.sendPacket(ws, 6100, 1095604361, packet)
		}
	}
	
	// Protocol: AckStartCluster
	AckStartCluster(ws, isBinary, name, url, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 6100, 1748961021, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
				result,
			}
			WsSockServer.sendPacket(ws, 6100, 1748961021, packet)
		}
	}
	
	// Protocol: AckCloseCluster
	AckCloseCluster(ws, isBinary, name, result, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			WsSockServer.sendPacketBinary(ws, 6100, 3666782746, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			WsSockServer.sendPacket(ws, 6100, 3666782746, packet)
		}
	}
	
	// Protocol: RealtimeRobotState
	RealtimeRobotState(ws, isBinary, robotName, state1, state2, state3, state4, pos, ) {
		if (isBinary) { // binary send?
			let packet = new Packet(512)
			packet.pushStr(robotName)
			packet.pushUint8(state1)
			packet.pushUint8(state2)
			packet.pushUint8(state3)
			packet.pushUint8(state4)
			Make_Vector3(packet,pos)
			WsSockServer.sendPacketBinary(ws, 6100, 1079519326, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				robotName,
				state1,
				state2,
				state3,
				state4,
				pos,
			}
			WsSockServer.sendPacket(ws, 6100, 1079519326, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// output s2c Protocol Handler
//------------------------------------------------------------------------
output.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Welcome(wss, ws, packet) {}
	AckLogin(wss, ws, packet) {}
	AckLogout(wss, ws, packet) {}
	ReqStartCluster(wss, ws, packet) {}
	ReqCloseCluster(wss, ws, packet) {}
}


//------------------------------------------------------------------------
// output c2s Protocol Handler
//------------------------------------------------------------------------
output.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	ReqLogin(wss, ws, packet) {}
	ReqLogout(wss, ws, packet) {}
	AckStartCluster(wss, ws, packet) {}
	AckCloseCluster(wss, ws, packet) {}
	RealtimeRobotState(wss, ws, packet) {}
}

