//------------------------------------------------------------------------
// Name:    vplp
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace vplpAsync {
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

	// s2c Protocol Packet Data
	export type Welcome_Packet = {
		msg: string, 
	}
	export type AckLogin_Packet = {
		id: string, 
		result: number, 
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


	// c2s Protocol Packet Data
	export type ReqLogin_Packet = {
		id: string, 
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


//------------------------------------------------------------------------
// vplpAsync s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(1000)
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

			case 3863877132: // AckRunVisualProg
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckRunVisualProg(parsePacket)
					})
				}
				break;

			case 3454830338: // AckRunVisualProgStream
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckRunVisualProgStream(parsePacket)
					})
				}
				break;

			case 2713087572: // AckRunVisualProgFiles
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckRunVisualProgFiles(parsePacket)
					})
				}
				break;

			case 1895439953: // AckStopVisualProg
				{
					handlers.forEach(handler => {
						if (handler instanceof s2c_ProtocolHandler)
							handler.AckStopVisualProg(parsePacket)
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
// vplpAsync c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(2000)
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

			case 2250021743: // ReqRunVisualProg
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqRunVisualProg(parsePacket)
					})
				}
				break;

			case 3686541167: // ReqRunVisualProgStream
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqRunVisualProgStream(parsePacket)
					})
				}
				break;

			case 3721131371: // ReqRunVisualProgFiles
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqRunVisualProgFiles(parsePacket)
					})
				}
				break;

			case 4258374867: // ReqStopVisualProg
				{
					handlers.forEach(handler => {
						if (handler instanceof c2s_ProtocolHandler)
							handler.ReqStopVisualProg(parsePacket)
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
// vplpAsync s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			this.node?.sendPacketBinary(1000, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(1000, 1281093745, packet)
		}
	}
	
	// Protocol: AckLogin
	AckLogin(isBinary: boolean, id: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(1000, 851424104, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			this.node?.sendPacket(1000, 851424104, packet)
		}
	}
	
	// Protocol: AckRunVisualProg
	AckRunVisualProg(isBinary: boolean, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(1000, 3863877132, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			this.node?.sendPacket(1000, 3863877132, packet)
		}
	}
	
	// Protocol: AckRunVisualProgStream
	AckRunVisualProgStream(isBinary: boolean, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(1000, 3454830338, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			this.node?.sendPacket(1000, 3454830338, packet)
		}
	}
	
	// Protocol: AckRunVisualProgFiles
	AckRunVisualProgFiles(isBinary: boolean, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(1000, 2713087572, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			this.node?.sendPacket(1000, 2713087572, packet)
		}
	}
	
	// Protocol: AckStopVisualProg
	AckStopVisualProg(isBinary: boolean, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(1000, 1895439953, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				result,
			}
			this.node?.sendPacket(1000, 1895439953, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// vplpAsync c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: ReqLogin
	ReqLogin(isBinary: boolean, id: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			this.node?.sendPacketBinary(2000, 1956887904, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
			}
			this.node?.sendPacket(2000, 1956887904, packet)
		}
	}
	
	// Protocol: ReqRunVisualProg
	ReqRunVisualProg(isBinary: boolean, nodeFile: sNodeFile, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Make_sNodeFile(packet,nodeFile)
			this.node?.sendPacketBinary(2000, 2250021743, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFile,
			}
			this.node?.sendPacket(2000, 2250021743, packet)
		}
	}
	
	// Protocol: ReqRunVisualProgStream
	ReqRunVisualProgStream(isBinary: boolean, count: number, index: number, data: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushUint32(count)
			packet.pushUint32(index)
			packet.pushUint8Array(data)
			this.node?.sendPacketBinary(2000, 3686541167, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				count,
				index,
				data,
			}
			this.node?.sendPacket(2000, 3686541167, packet)
		}
	}
	
	// Protocol: ReqRunVisualProgFiles
	ReqRunVisualProgFiles(isBinary: boolean, nodeFileNames: string[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStrArray(nodeFileNames)
			this.node?.sendPacketBinary(2000, 3721131371, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				nodeFileNames,
			}
			this.node?.sendPacket(2000, 3721131371, packet)
		}
	}
	
	// Protocol: ReqStopVisualProg
	ReqStopVisualProg(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(2000, 4258374867, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(2000, 4258374867, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// vplpAsync s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: vplpAsync.Welcome_Packet) => { }
	AckLogin = (packet: vplpAsync.AckLogin_Packet) => { }
	AckRunVisualProg = (packet: vplpAsync.AckRunVisualProg_Packet) => { }
	AckRunVisualProgStream = (packet: vplpAsync.AckRunVisualProgStream_Packet) => { }
	AckRunVisualProgFiles = (packet: vplpAsync.AckRunVisualProgFiles_Packet) => { }
	AckStopVisualProg = (packet: vplpAsync.AckStopVisualProg_Packet) => { }
}


//------------------------------------------------------------------------
// vplpAsync c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	ReqLogin = (packet: vplpAsync.ReqLogin_Packet) => { }
	ReqRunVisualProg = (packet: vplpAsync.ReqRunVisualProg_Packet) => { }
	ReqRunVisualProgStream = (packet: vplpAsync.ReqRunVisualProgStream_Packet) => { }
	ReqRunVisualProgFiles = (packet: vplpAsync.ReqRunVisualProgFiles_Packet) => { }
	ReqStopVisualProg = (packet: vplpAsync.ReqStopVisualProg_Packet) => { }
}

}

