//------------------------------------------------------------------------
// Name:    webrelay
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace webrelayAsync {

	// s2c Protocol Packet Data
	export type Welcome_Packet = {
		msg: string, 
	}
	export type AckLogin_Packet = {
		name: string, 
		type: number, 
		result: number, 
	}
	export type AckLogout_Packet = {
		name: string, 
		result: number, 
	}
	export type ReqStartInstance_Packet = {
		name: string, 
		url: string, 
	}
	export type ReqCloseInstance_Packet = {
		name: string, 
	}
	export type ReqConnectProxyServer_Packet = {
		url: string, 
		name: string, 
	}
	export type ReqPing_Packet = {
		id: number, 
	}


	// c2s Protocol Packet Data
	export type ReqLogin_Packet = {
		name: string, 
		type: number, 
	}
	export type ReqLogout_Packet = {
		name: string, 
	}
	export type AckStartInstance_Packet = {
		name: string, 
		url: string, 
		result: number, 
	}
	export type AckCloseInstance_Packet = {
		name: string, 
		result: number, 
	}
	export type AckConnectProxyServer_Packet = {
		url: string, 
		name: string, 
		result: number, 
	}
	export type AckPing_Packet = {
		id: number, 
	}


//------------------------------------------------------------------------
// webrelayAsync s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(12000)
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

			case 4021090742: // ReqStartInstance
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.ReqStartInstance(parsePacket)
					})
				}
				break;

			case 2576937513: // ReqCloseInstance
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.ReqCloseInstance(parsePacket)
					})
				}
				break;

			case 2863529255: // ReqConnectProxyServer
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.ReqConnectProxyServer(parsePacket)
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
// webrelayAsync c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(12100)
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

			case 619773631: // AckStartInstance
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.AckStartInstance(parsePacket)
					})
				}
				break;

			case 673086330: // AckCloseInstance
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.AckCloseInstance(parsePacket)
					})
				}
				break;

			case 1736849374: // AckConnectProxyServer
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.AckConnectProxyServer(parsePacket)
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
// webrelayAsync s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			this.node?.sendPacketBinary(12000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(12000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(isBinary: boolean, name: string, type: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(type)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(12000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				type,
				result,
			}
			this.node?.sendPacket(12000, 851424104, packet)
		}
	}
	
	// Protocol: AckLogout
	AckLogout(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(12000, 2822050476, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(12000, 2822050476, packet)
		}
	}
	
	// Protocol: ReqStartInstance
	ReqStartInstance(isBinary: boolean, name: string, url: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			this.node?.sendPacketBinary(12000, 4021090742, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
			}
			this.node?.sendPacket(12000, 4021090742, packet)
		}
	}
	
	// Protocol: ReqCloseInstance
	ReqCloseInstance(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(12000, 2576937513, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(12000, 2576937513, packet)
		}
	}
	
	// Protocol: ReqConnectProxyServer
	ReqConnectProxyServer(isBinary: boolean, url: string, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			this.node?.sendPacketBinary(12000, 2863529255, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
			}
			this.node?.sendPacket(12000, 2863529255, packet)
		}
	}
	
	// Protocol: ReqPing
	ReqPing(isBinary: boolean, id: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(id)
			this.node?.sendPacketBinary(12000, 357726072, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			this.node?.sendPacket(12000, 357726072, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// webrelayAsync c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqLogin
	ReqLogin(isBinary: boolean, name: string, type: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(type)
			this.node?.sendPacketBinary(12100, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				type,
			}
			this.node?.sendPacket(12100, 1956887904, packet)
		}
	}
	
	// Protocol: ReqLogout
	ReqLogout(isBinary: boolean, name: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			this.node?.sendPacketBinary(12100, 1095604361, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
			}
			this.node?.sendPacket(12100, 1095604361, packet)
		}
	}
	
	// Protocol: AckStartInstance
	AckStartInstance(isBinary: boolean, name: string, url: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushStr(url)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(12100, 619773631, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				url,
				result,
			}
			this.node?.sendPacket(12100, 619773631, packet)
		}
	}
	
	// Protocol: AckCloseInstance
	AckCloseInstance(isBinary: boolean, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(12100, 673086330, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				name,
				result,
			}
			this.node?.sendPacket(12100, 673086330, packet)
		}
	}
	
	// Protocol: AckConnectProxyServer
	AckConnectProxyServer(isBinary: boolean, url: string, name: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(url)
			packet.pushStr(name)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(12100, 1736849374, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				url,
				name,
				result,
			}
			this.node?.sendPacket(12100, 1736849374, packet)
		}
	}
	
	// Protocol: AckPing
	AckPing(isBinary: boolean, id: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(id)
			this.node?.sendPacketBinary(12100, 3275360863, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			this.node?.sendPacket(12100, 3275360863, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// webrelayAsync s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: webrelayAsync.Welcome_Packet) => { }
	AckLogin = (packet: webrelayAsync.AckLogin_Packet) => { }
	AckLogout = (packet: webrelayAsync.AckLogout_Packet) => { }
	ReqStartInstance = (packet: webrelayAsync.ReqStartInstance_Packet) => { }
	ReqCloseInstance = (packet: webrelayAsync.ReqCloseInstance_Packet) => { }
	ReqConnectProxyServer = (packet: webrelayAsync.ReqConnectProxyServer_Packet) => { }
	ReqPing = (packet: webrelayAsync.ReqPing_Packet) => { }
}


//------------------------------------------------------------------------
// webrelayAsync c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqLogin = (packet: webrelayAsync.ReqLogin_Packet) => { }
	ReqLogout = (packet: webrelayAsync.ReqLogout_Packet) => { }
	AckStartInstance = (packet: webrelayAsync.AckStartInstance_Packet) => { }
	AckCloseInstance = (packet: webrelayAsync.AckCloseInstance_Packet) => { }
	AckConnectProxyServer = (packet: webrelayAsync.AckConnectProxyServer_Packet) => { }
	AckPing = (packet: webrelayAsync.AckPing_Packet) => { }
}

}

