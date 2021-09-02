//------------------------------------------------------------------------
// Name:    remotedbg2
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace remotedbg2 {
	export type sSyncSymbol = {
		scope: string, 
		name: string, 
		var: TypeVariant, 
	}
	function Parse_sSyncSymbol(packet: Network.Packet) : sSyncSymbol {
		return {
			scope: packet.getStr(),
			name: packet.getStr(),
			var: packet.getTypeVariant(),
		}
	}
	function Parse_sSyncSymbolVector(packet: Network.Packet) : sSyncSymbol[] {
		const size = packet.getUint32()
		if (size == 0) return []
		let ar: sSyncSymbol[] = []
		for(let i=0; i < size; ++i)
			ar.push(Parse_sSyncSymbol(packet))
		return ar
	}
	function Make_sSyncSymbol(packet: Network.Packet, data: sSyncSymbol) {
	}
	function Make_sSyncSymbolVector(packet: Network.Packet, data: sSyncSymbol[]) {
	}
	export type sRegister = {
		idx: number, 
		cmp: boolean, 
		val: TypeVariant[], 
	}
	function Parse_sRegister(packet: Network.Packet) : sRegister {
		return {
			idx: packet.getUint32(),
			cmp: packet.getBool(),
			val: packet.getTypeVariantVector(),
		}
	}
	function Parse_sRegisterVector(packet: Network.Packet) : sRegister[] {
		const size = packet.getUint32()
		if (size == 0) return []
		let ar: sRegister[] = []
		for(let i=0; i < size; ++i)
			ar.push(Parse_sRegister(packet))
		return ar
	}
	function Make_sRegister(packet: Network.Packet, data: sRegister) {
	}
	function Make_sRegisterVector(packet: Network.Packet, data: sRegister[]) {
	}

	// r2h Protocol Packet Data
	export type Welcome_Packet = {
		msg: string, 
	}
	export type UploadIntermediateCode_Packet = {
		itprId: number, 
		code: string, 
	}
	export type ReqIntermediateCode_Packet = {
		itprId: number, 
	}
	export type ReqRun_Packet = {
		itprId: number, 
		runType: string, 
	}
	export type ReqOneStep_Packet = {
		itprId: number, 
	}
	export type ReqResumeRun_Packet = {
		itprId: number, 
	}
	export type ReqBreak_Packet = {
		itprId: number, 
	}
	export type ReqBreakPoint_Packet = {
		itprId: number, 
		enable: boolean, 
		id: number, 
	}
	export type ReqStop_Packet = {
		itprId: number, 
	}
	export type ReqInput_Packet = {
		itprId: number, 
		vmIdx: number, 
		eventName: string, 
	}
	export type ReqEvent_Packet = {
		itprId: number, 
		vmIdx: number, 
		eventName: string, 
	}
	export type ReqStepDebugType_Packet = {
		stepDbgType: number, 
	}
	export type ReqHeartBeat_Packet = {
	}


	// h2r Protocol Packet Data
	export type AckUploadIntermediateCode_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckIntermediateCode_Packet = {
		itprId: number, 
		result: number, 
		count: number, 
		index: number, 
		data: Uint8Array | null, 
	}
	export type AckRun_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckOneStep_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckResumeRun_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckBreak_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckBreakPoint_Packet = {
		itprId: number, 
		enable: boolean, 
		id: number, 
		result: number, 
	}
	export type AckStop_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckInput_Packet = {
		itprId: number, 
		result: number, 
	}
	export type AckEvent_Packet = {
		itprId: number, 
		vmIdx: number, 
		eventName: string, 
		result: number, 
	}
	export type AckStepDebugType_Packet = {
		stepDbgType: number, 
		result: number, 
	}
	export type SyncVMInstruction_Packet = {
		itprId: number, 
		vmIdx: number, 
		indices: Uint16Array | null, 
	}
	export type SyncVMRegister_Packet = {
		itprId: number, 
		vmIdx: number, 
		infoType: number, 
		reg: sRegister, 
	}
	export type SyncVMSymbolTable_Packet = {
		itprId: number, 
		vmIdx: number, 
		start: number, 
		count: number, 
		symbol: sSyncSymbol[], 
	}
	export type SyncVMOutput_Packet = {
		itprId: number, 
		vmIdx: number, 
		output: string, 
	}
	export type AckHeartBeat_Packet = {
	}


//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Dispatcher
//------------------------------------------------------------------------
export class r2h_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(5301)
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
							if (handler instanceof r2h_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					} else { // json?
						const parsePacket: Welcome_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					}
				}
				break;

			case 1418562193: // UploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const code = packet.getStr()
						const parsePacket: UploadIntermediateCode_Packet = {
							itprId,
							code,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.UploadIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket: UploadIntermediateCode_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.UploadIntermediateCode(parsePacket)
						})
					}
				}
				break;

			case 1644585100: // ReqIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket: ReqIntermediateCode_Packet = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqIntermediateCode_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqIntermediateCode(parsePacket)
						})
					}
				}
				break;

			case 923151823: // ReqRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const runType = packet.getStr()
						const parsePacket: ReqRun_Packet = {
							itprId,
							runType,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqRun(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqRun_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqRun(parsePacket)
						})
					}
				}
				break;

			case 2884814738: // ReqOneStep
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket: ReqOneStep_Packet = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqOneStep(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqOneStep_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqOneStep(parsePacket)
						})
					}
				}
				break;

			case 742173167: // ReqResumeRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket: ReqResumeRun_Packet = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqResumeRun(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqResumeRun_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqResumeRun(parsePacket)
						})
					}
				}
				break;

			case 784411795: // ReqBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket: ReqBreak_Packet = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqBreak(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqBreak_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqBreak(parsePacket)
						})
					}
				}
				break;

			case 2487089996: // ReqBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const parsePacket: ReqBreakPoint_Packet = {
							itprId,
							enable,
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqBreakPoint(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqBreakPoint_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqBreakPoint(parsePacket)
						})
					}
				}
				break;

			case 1453251868: // ReqStop
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket: ReqStop_Packet = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqStop(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqStop_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqStop(parsePacket)
						})
					}
				}
				break;

			case 3140751413: // ReqInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const eventName = packet.getStr()
						const parsePacket: ReqInput_Packet = {
							itprId,
							vmIdx,
							eventName,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqInput(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqInput_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqInput(parsePacket)
						})
					}
				}
				break;

			case 186222094: // ReqEvent
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const eventName = packet.getStr()
						const parsePacket: ReqEvent_Packet = {
							itprId,
							vmIdx,
							eventName,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqEvent(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqEvent_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqEvent(parsePacket)
						})
					}
				}
				break;

			case 3084593987: // ReqStepDebugType
				{
					if (option == 1) { // binary?
						const stepDbgType = packet.getInt32()
						const parsePacket: ReqStepDebugType_Packet = {
							stepDbgType,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqStepDebugType(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqStepDebugType_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqStepDebugType(parsePacket)
						})
					}
				}
				break;

			case 2532286881: // ReqHeartBeat
				{
					if (option == 1) { // binary?
						const parsePacket: ReqHeartBeat_Packet = {
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqHeartBeat(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqHeartBeat_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqHeartBeat(parsePacket)
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
// remotedbg2 h2r Protocol Dispatcher
//------------------------------------------------------------------------
export class h2r_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(5300)
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
			case 4005257575: // AckUploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckUploadIntermediateCode_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckUploadIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket: AckUploadIntermediateCode_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckUploadIntermediateCode(parsePacket)
						})
					}
				}
				break;

			case 1397310616: // AckIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const count = packet.getUint32()
						const index = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket: AckIntermediateCode_Packet = {
							itprId,
							result,
							count,
							index,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckIntermediateCode(parsePacket)
						})
					} else { // json?
						const parsePacket: AckIntermediateCode_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckIntermediateCode(parsePacket)
						})
					}
				}
				break;

			case 4148808214: // AckRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckRun_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckRun(parsePacket)
						})
					} else { // json?
						const parsePacket: AckRun_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckRun(parsePacket)
						})
					}
				}
				break;

			case 3643391279: // AckOneStep
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckOneStep_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckOneStep(parsePacket)
						})
					} else { // json?
						const parsePacket: AckOneStep_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckOneStep(parsePacket)
						})
					}
				}
				break;

			case 1012496086: // AckResumeRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckResumeRun_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckResumeRun(parsePacket)
						})
					} else { // json?
						const parsePacket: AckResumeRun_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckResumeRun(parsePacket)
						})
					}
				}
				break;

			case 2129545277: // AckBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckBreak_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckBreak(parsePacket)
						})
					} else { // json?
						const parsePacket: AckBreak_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckBreak(parsePacket)
						})
					}
				}
				break;

			case 2045074648: // AckBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const result = packet.getInt32()
						const parsePacket: AckBreakPoint_Packet = {
							itprId,
							enable,
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckBreakPoint(parsePacket)
						})
					} else { // json?
						const parsePacket: AckBreakPoint_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckBreakPoint(parsePacket)
						})
					}
				}
				break;

			case 114435221: // AckStop
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckStop_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckStop(parsePacket)
						})
					} else { // json?
						const parsePacket: AckStop_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckStop(parsePacket)
						})
					}
				}
				break;

			case 1658444570: // AckInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckInput_Packet = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckInput(parsePacket)
						})
					} else { // json?
						const parsePacket: AckInput_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckInput(parsePacket)
						})
					}
				}
				break;

			case 1906481345: // AckEvent
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const eventName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckEvent_Packet = {
							itprId,
							vmIdx,
							eventName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckEvent(parsePacket)
						})
					} else { // json?
						const parsePacket: AckEvent_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckEvent(parsePacket)
						})
					}
				}
				break;

			case 4225702489: // AckStepDebugType
				{
					if (option == 1) { // binary?
						const stepDbgType = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckStepDebugType_Packet = {
							stepDbgType,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckStepDebugType(parsePacket)
						})
					} else { // json?
						const parsePacket: AckStepDebugType_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckStepDebugType(parsePacket)
						})
					}
				}
				break;

			case 4206107288: // SyncVMInstruction
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const indices = packet.getUint16Array()
						const parsePacket: SyncVMInstruction_Packet = {
							itprId,
							vmIdx,
							indices,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMInstruction(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMInstruction_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMInstruction(parsePacket)
						})
					}
				}
				break;

			case 3001685594: // SyncVMRegister
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const infoType = packet.getInt32()
						const reg = Parse_sRegister(packet)
						const parsePacket: SyncVMRegister_Packet = {
							itprId,
							vmIdx,
							infoType,
							reg,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMRegister(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMRegister_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMRegister(parsePacket)
						})
					}
				}
				break;

			case 3045798844: // SyncVMSymbolTable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const start = packet.getUint32()
						const count = packet.getUint32()
						const symbol = Parse_sSyncSymbolVector(packet)
						const parsePacket: SyncVMSymbolTable_Packet = {
							itprId,
							vmIdx,
							start,
							count,
							symbol,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMSymbolTable(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMSymbolTable_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMSymbolTable(parsePacket)
						})
					}
				}
				break;

			case 1348120458: // SyncVMOutput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmIdx = packet.getInt32()
						const output = packet.getStr()
						const parsePacket: SyncVMOutput_Packet = {
							itprId,
							vmIdx,
							output,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMOutput(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMOutput_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMOutput(parsePacket)
						})
					}
				}
				break;

			case 1133387750: // AckHeartBeat
				{
					if (option == 1) { // binary?
						const parsePacket: AckHeartBeat_Packet = {
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckHeartBeat(parsePacket)
						})
					} else { // json?
						const parsePacket: AckHeartBeat_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckHeartBeat(parsePacket)
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
// remotedbg2 r2h Protocol 
//------------------------------------------------------------------------
export class r2h_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			Network.sendPacketBinary(this.ws, 5301, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			Network.sendPacket(this.ws, 5301, 1281093745, packet)
		}
	}
	
	// Protocol: UploadIntermediateCode
	UploadIntermediateCode(isBinary: boolean, itprId: number, code: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(code)
			Network.sendPacketBinary(this.ws, 5301, 1418562193, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				code,
			}
			Network.sendPacket(this.ws, 5301, 1418562193, packet)
		}
	}
	
	// Protocol: ReqIntermediateCode
	ReqIntermediateCode(isBinary: boolean, itprId: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			Network.sendPacketBinary(this.ws, 5301, 1644585100, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			Network.sendPacket(this.ws, 5301, 1644585100, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(isBinary: boolean, itprId: number, runType: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(runType)
			Network.sendPacketBinary(this.ws, 5301, 923151823, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				runType,
			}
			Network.sendPacket(this.ws, 5301, 923151823, packet)
		}
	}
	
	// Protocol: ReqOneStep
	ReqOneStep(isBinary: boolean, itprId: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			Network.sendPacketBinary(this.ws, 5301, 2884814738, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			Network.sendPacket(this.ws, 5301, 2884814738, packet)
		}
	}
	
	// Protocol: ReqResumeRun
	ReqResumeRun(isBinary: boolean, itprId: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			Network.sendPacketBinary(this.ws, 5301, 742173167, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			Network.sendPacket(this.ws, 5301, 742173167, packet)
		}
	}
	
	// Protocol: ReqBreak
	ReqBreak(isBinary: boolean, itprId: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			Network.sendPacketBinary(this.ws, 5301, 784411795, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			Network.sendPacket(this.ws, 5301, 784411795, packet)
		}
	}
	
	// Protocol: ReqBreakPoint
	ReqBreakPoint(isBinary: boolean, itprId: number, enable: boolean, id: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			Network.sendPacketBinary(this.ws, 5301, 2487089996, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				enable,
				id,
			}
			Network.sendPacket(this.ws, 5301, 2487089996, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(isBinary: boolean, itprId: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			Network.sendPacketBinary(this.ws, 5301, 1453251868, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			Network.sendPacket(this.ws, 5301, 1453251868, packet)
		}
	}
	
	// Protocol: ReqInput
	ReqInput(isBinary: boolean, itprId: number, vmIdx: number, eventName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushStr(eventName)
			Network.sendPacketBinary(this.ws, 5301, 3140751413, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				eventName,
			}
			Network.sendPacket(this.ws, 5301, 3140751413, packet)
		}
	}
	
	// Protocol: ReqEvent
	ReqEvent(isBinary: boolean, itprId: number, vmIdx: number, eventName: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushStr(eventName)
			Network.sendPacketBinary(this.ws, 5301, 186222094, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				eventName,
			}
			Network.sendPacket(this.ws, 5301, 186222094, packet)
		}
	}
	
	// Protocol: ReqStepDebugType
	ReqStepDebugType(isBinary: boolean, stepDbgType: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			Network.sendPacketBinary(this.ws, 5301, 3084593987, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
			}
			Network.sendPacket(this.ws, 5301, 3084593987, packet)
		}
	}
	
	// Protocol: ReqHeartBeat
	ReqHeartBeat(isBinary: boolean, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Network.sendPacketBinary(this.ws, 5301, 2532286881, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			Network.sendPacket(this.ws, 5301, 2532286881, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol 
//------------------------------------------------------------------------
export class h2r_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: AckUploadIntermediateCode
	AckUploadIntermediateCode(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 4005257575, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 4005257575, packet)
		}
	}
	
	// Protocol: AckIntermediateCode
	AckIntermediateCode(isBinary: boolean, itprId: number, result: number, count: number, index: number, data: number[], ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			packet.pushUint32(count)
			packet.pushUint32(index)
			packet.pushUint8Array(data)
			Network.sendPacketBinary(this.ws, 5300, 1397310616, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
				count,
				index,
				data,
			}
			Network.sendPacket(this.ws, 5300, 1397310616, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 4148808214, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 4148808214, packet)
		}
	}
	
	// Protocol: AckOneStep
	AckOneStep(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 3643391279, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 3643391279, packet)
		}
	}
	
	// Protocol: AckResumeRun
	AckResumeRun(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 1012496086, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 1012496086, packet)
		}
	}
	
	// Protocol: AckBreak
	AckBreak(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 2129545277, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 2129545277, packet)
		}
	}
	
	// Protocol: AckBreakPoint
	AckBreakPoint(isBinary: boolean, itprId: number, enable: boolean, id: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 2045074648, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				enable,
				id,
				result,
			}
			Network.sendPacket(this.ws, 5300, 2045074648, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 114435221, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 114435221, packet)
		}
	}
	
	// Protocol: AckInput
	AckInput(isBinary: boolean, itprId: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 1658444570, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			Network.sendPacket(this.ws, 5300, 1658444570, packet)
		}
	}
	
	// Protocol: AckEvent
	AckEvent(isBinary: boolean, itprId: number, vmIdx: number, eventName: string, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushStr(eventName)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 1906481345, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				eventName,
				result,
			}
			Network.sendPacket(this.ws, 5300, 1906481345, packet)
		}
	}
	
	// Protocol: AckStepDebugType
	AckStepDebugType(isBinary: boolean, stepDbgType: number, result: number, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			packet.pushInt32(result)
			Network.sendPacketBinary(this.ws, 5300, 4225702489, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
				result,
			}
			Network.sendPacket(this.ws, 5300, 4225702489, packet)
		}
	}
	
	// Protocol: SyncVMInstruction
	SyncVMInstruction(isBinary: boolean, itprId: number, vmIdx: number, indices: number[], ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushUint16Array(indices)
			Network.sendPacketBinary(this.ws, 5300, 4206107288, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				indices,
			}
			Network.sendPacket(this.ws, 5300, 4206107288, packet)
		}
	}
	
	// Protocol: SyncVMRegister
	SyncVMRegister(isBinary: boolean, itprId: number, vmIdx: number, infoType: number, reg: sRegister, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushInt32(infoType)
			Make_sRegister(packet,reg)
			Network.sendPacketBinary(this.ws, 5300, 3001685594, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				infoType,
				reg,
			}
			Network.sendPacket(this.ws, 5300, 3001685594, packet)
		}
	}
	
	// Protocol: SyncVMSymbolTable
	SyncVMSymbolTable(isBinary: boolean, itprId: number, vmIdx: number, start: number, count: number, symbol: sSyncSymbol[], ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushUint32(start)
			packet.pushUint32(count)
			Make_sSyncSymbolVector(packet,symbol)
			Network.sendPacketBinary(this.ws, 5300, 3045798844, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				start,
				count,
				symbol,
			}
			Network.sendPacket(this.ws, 5300, 3045798844, packet)
		}
	}
	
	// Protocol: SyncVMOutput
	SyncVMOutput(isBinary: boolean, itprId: number, vmIdx: number, output: string, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmIdx)
			packet.pushStr(output)
			Network.sendPacketBinary(this.ws, 5300, 1348120458, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmIdx,
				output,
			}
			Network.sendPacket(this.ws, 5300, 1348120458, packet)
		}
	}
	
	// Protocol: AckHeartBeat
	AckHeartBeat(isBinary: boolean, ) {
		if (!this.ws)
			return
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			Network.sendPacketBinary(this.ws, 5300, 1133387750, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			Network.sendPacket(this.ws, 5300, 1133387750, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Handler
//------------------------------------------------------------------------
export class r2h_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: remotedbg2.Welcome_Packet) => { }
	UploadIntermediateCode = (packet: remotedbg2.UploadIntermediateCode_Packet) => { }
	ReqIntermediateCode = (packet: remotedbg2.ReqIntermediateCode_Packet) => { }
	ReqRun = (packet: remotedbg2.ReqRun_Packet) => { }
	ReqOneStep = (packet: remotedbg2.ReqOneStep_Packet) => { }
	ReqResumeRun = (packet: remotedbg2.ReqResumeRun_Packet) => { }
	ReqBreak = (packet: remotedbg2.ReqBreak_Packet) => { }
	ReqBreakPoint = (packet: remotedbg2.ReqBreakPoint_Packet) => { }
	ReqStop = (packet: remotedbg2.ReqStop_Packet) => { }
	ReqInput = (packet: remotedbg2.ReqInput_Packet) => { }
	ReqEvent = (packet: remotedbg2.ReqEvent_Packet) => { }
	ReqStepDebugType = (packet: remotedbg2.ReqStepDebugType_Packet) => { }
	ReqHeartBeat = (packet: remotedbg2.ReqHeartBeat_Packet) => { }
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol Handler
//------------------------------------------------------------------------
export class h2r_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	AckUploadIntermediateCode = (packet: remotedbg2.AckUploadIntermediateCode_Packet) => { }
	AckIntermediateCode = (packet: remotedbg2.AckIntermediateCode_Packet) => { }
	AckRun = (packet: remotedbg2.AckRun_Packet) => { }
	AckOneStep = (packet: remotedbg2.AckOneStep_Packet) => { }
	AckResumeRun = (packet: remotedbg2.AckResumeRun_Packet) => { }
	AckBreak = (packet: remotedbg2.AckBreak_Packet) => { }
	AckBreakPoint = (packet: remotedbg2.AckBreakPoint_Packet) => { }
	AckStop = (packet: remotedbg2.AckStop_Packet) => { }
	AckInput = (packet: remotedbg2.AckInput_Packet) => { }
	AckEvent = (packet: remotedbg2.AckEvent_Packet) => { }
	AckStepDebugType = (packet: remotedbg2.AckStepDebugType_Packet) => { }
	SyncVMInstruction = (packet: remotedbg2.SyncVMInstruction_Packet) => { }
	SyncVMRegister = (packet: remotedbg2.SyncVMRegister_Packet) => { }
	SyncVMSymbolTable = (packet: remotedbg2.SyncVMSymbolTable_Packet) => { }
	SyncVMOutput = (packet: remotedbg2.SyncVMOutput_Packet) => { }
	AckHeartBeat = (packet: remotedbg2.AckHeartBeat_Packet) => { }
}

}

