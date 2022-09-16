//------------------------------------------------------------------------
// Name:    test
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace test {

	// s2c Protocol Packet Data
	export type AckResult_Packet = {
		packetName: string, 
		result: number, 
		vars: Map<string,string[]>, 
	}


	// c2s Protocol Packet Data
	export type PacketName1_Packet = {
		id: string, 
		data: string, 
		num: number, 
	}


//------------------------------------------------------------------------
// test s2c Protocol Dispatcher
//------------------------------------------------------------------------
export class s2c_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(100000)
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
			case 3846277350: // AckResult
				{
					if (option == 1) { // binary?
						const packetName = packet.getStr()
						const result = packet.getFloat32()
						const vars = packet.getMapStrArray()
						const parsePacket: AckResult_Packet = {
							packetName,
							result,
							vars,
						}
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckResult(parsePacket)
						})
					} else { // json?
						const parsePacket: AckResult_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof s2c_ProtocolHandler)
								handler.AckResult(parsePacket)
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
// test c2s Protocol Dispatcher
//------------------------------------------------------------------------
export class c2s_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(100001)
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
			case 2900479863: // PacketName1
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const data = packet.getStr()
						const num = packet.getFloat32()
						const parsePacket: PacketName1_Packet = {
							id,
							data,
							num,
						}
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.PacketName1(parsePacket)
						})
					} else { // json?
						const parsePacket: PacketName1_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof c2s_ProtocolHandler)
								handler.PacketName1(parsePacket)
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
// test s2c Protocol 
//------------------------------------------------------------------------
export class s2c_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: AckResult
	AckResult(isBinary: boolean, packetName: string, result: number, vars: Map<string,string[]>, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(packetName)
			packet.pushFloat32(result)
			packet.pushMapStrArray(vars)
			this.node?.sendPacketBinary(100000, 3846277350, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				packetName,
				result,
				vars,
			}
			this.node?.sendPacket(100000, 3846277350, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// test c2s Protocol 
//------------------------------------------------------------------------
export class c2s_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: PacketName1
	PacketName1(isBinary: boolean, id: string, data: string, num: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(id)
			packet.pushStr(data)
			packet.pushFloat32(num)
			this.node?.sendPacketBinary(100001, 2900479863, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				data,
				num,
			}
			this.node?.sendPacket(100001, 2900479863, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// test s2c Protocol Handler
//------------------------------------------------------------------------
export class s2c_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	AckResult = (packet: test.AckResult_Packet) => { }
}


//------------------------------------------------------------------------
// test c2s Protocol Handler
//------------------------------------------------------------------------
export class c2s_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	PacketName1 = (packet: test.PacketName1_Packet) => { }
}

}

