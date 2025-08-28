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
		packet.pushStr(data.scope)
		packet.pushStr(data.name)
		packet.pushTypeVariant(data.var)
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
		packet.pushUint32(data.idx)
		packet.pushBool(data.cmp)
		packet.pushTypeVariantVector(data.val)
	}
	function Make_sRegisterVector(packet: Network.Packet, data: sRegister[]) {
	}

	// r2h Protocol Packet Data
	export type UploadIntermediateCode_Packet = {
		itprId: number, 
		vmId: number, 
		code: string, 
	}
	export type ReqIntermediateCode_Packet = {
		itprId: number, 
		vmId: number, 
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
		vmId: number, 
	}
	export type ReqBreakPoint_Packet = {
		itprId: number, 
		vmId: number, 
		enable: boolean, 
		id: number, 
	}
	export type ReqStop_Packet = {
		itprId: number, 
	}
	export type ReqInput_Packet = {
		itprId: number, 
		vmId: number, 
		eventName: string, 
	}
	export type ReqEvent_Packet = {
		itprId: number, 
		vmId: number, 
		eventName: string, 
		values: Map<string,string[]>, 
	}
	export type ReqStepDebugType_Packet = {
		stepDbgType: number, 
	}
	export type ReqDebugInfo_Packet = {
		vmIds: Int32Array | null, 
	}
	export type ReqVariableInfo_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
	}
	export type ReqChangeVariable_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
		value: string, 
	}
	export type ReqHeartBeat_Packet = {
	}


	// h2r Protocol Packet Data
	export type Welcome_Packet = {
		msg: string, 
	}
	export type AckUploadIntermediateCode_Packet = {
		itprId: number, 
		vmId: number, 
		result: number, 
	}
	export type AckIntermediateCode_Packet = {
		itprId: number, 
		vmId: number, 
		result: number, 
		count: number, 
		index: number, 
		totalBufferSize: number, 
		data: Uint8Array | null, 
	}
	export type SpawnTotalInterpreterInfo_Packet = {
		totalCount: number, 
		index: number, 
		itprId: number, 
		vmIds: Int32Array | null, 
	}
	export type SpawnInterpreterInfo_Packet = {
		itprId: number, 
		parentVmId: number, 
		vmId: number, 
		nodeFileName: string, 
		nodeName: string, 
	}
	export type RemoveInterpreter_Packet = {
		vmId: number, 
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
		vmId: number, 
		result: number, 
	}
	export type AckBreakPoint_Packet = {
		itprId: number, 
		vmId: number, 
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
		vmId: number, 
		result: number, 
	}
	export type AckEvent_Packet = {
		itprId: number, 
		vmId: number, 
		eventName: string, 
		result: number, 
	}
	export type AckStepDebugType_Packet = {
		stepDbgType: number, 
		result: number, 
	}
	export type AckDebugInfo_Packet = {
		vmIds: Int32Array | null, 
		result: number, 
	}
	export type AckChangeVariable_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
		result: number, 
	}
	export type SyncVMInstruction_Packet = {
		itprId: number, 
		vmId: number, 
		indices: Uint16Array | null, 
	}
	export type SyncVMRegister_Packet = {
		itprId: number, 
		vmId: number, 
		infoType: number, 
		reg: sRegister, 
	}
	export type SyncVMSymbolTable_Packet = {
		itprId: number, 
		vmId: number, 
		start: number, 
		count: number, 
		symbol: sSyncSymbol[], 
	}
	export type SyncVMOutput_Packet = {
		itprId: number, 
		vmId: number, 
		output: string, 
	}
	export type SyncVMWidgets_Packet = {
		itprId: number, 
		vmId: number, 
		widgetName: string, 
	}
	export type SyncVMArray_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
		startIdx: number, 
		totalSize: number, 
		array: TypeVariant[], 
	}
	export type SyncVMArrayBool_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
		startIdx: number, 
		totalSize: number, 
		array: Uint8Array | null, 
	}
	export type SyncVMArrayNumber_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
		startIdx: number, 
		totalSize: number, 
		array: Float32Array | null, 
	}
	export type SyncVMArrayString_Packet = {
		itprId: number, 
		vmId: number, 
		varName: string, 
		startIdx: number, 
		totalSize: number, 
		array: string[], 
	}
	export type SyncVMTimer_Packet = {
		itprId: number, 
		vmId: number, 
		scopeName: string, 
		timerId: number, 
		time: number, 
		actionType: number, 
	}
	export type ExecuteCustomFunction_Packet = {
		fnName: string, 
		args: Map<string,string[]>, 
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
			case 201763535: // UploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const code = packet.getStr()
						const parsePacket: UploadIntermediateCode_Packet = {
							itprId,
							vmId,
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

			case 2564628098: // ReqIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const parsePacket: ReqIntermediateCode_Packet = {
							itprId,
							vmId,
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

			case 4041661876: // ReqRun
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

			case 250175983: // ReqOneStep
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

			case 433520154: // ReqResumeRun
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

			case 1318526848: // ReqBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const parsePacket: ReqBreak_Packet = {
							itprId,
							vmId,
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

			case 3344680000: // ReqBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const parsePacket: ReqBreakPoint_Packet = {
							itprId,
							vmId,
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

			case 4009815483: // ReqStop
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

			case 1145831178: // ReqInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const eventName = packet.getStr()
						const parsePacket: ReqInput_Packet = {
							itprId,
							vmId,
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

			case 1246662946: // ReqEvent
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const eventName = packet.getStr()
						const values = packet.getMapStrArray()
						const parsePacket: ReqEvent_Packet = {
							itprId,
							vmId,
							eventName,
							values,
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

			case 2764328627: // ReqStepDebugType
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

			case 207775889: // ReqDebugInfo
				{
					if (option == 1) { // binary?
						const vmIds = packet.getInt32Array()
						const parsePacket: ReqDebugInfo_Packet = {
							vmIds,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqDebugInfo(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqDebugInfo_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqDebugInfo(parsePacket)
						})
					}
				}
				break;

			case 1419406571: // ReqVariableInfo
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const parsePacket: ReqVariableInfo_Packet = {
							itprId,
							vmId,
							varName,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqVariableInfo(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqVariableInfo_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqVariableInfo(parsePacket)
						})
					}
				}
				break;

			case 1095285398: // ReqChangeVariable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const value = packet.getStr()
						const parsePacket: ReqChangeVariable_Packet = {
							itprId,
							vmId,
							varName,
							value,
						}
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqChangeVariable(parsePacket)
						})
					} else { // json?
						const parsePacket: ReqChangeVariable_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof r2h_ProtocolHandler)
								handler.ReqChangeVariable(parsePacket)
						})
					}
				}
				break;

			case 2149460281: // ReqHeartBeat
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
			case 3047429: // Welcome
				{
					if (option == 1) { // binary?
						const msg = packet.getStr()
						const parsePacket: Welcome_Packet = {
							msg,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					} else { // json?
						const parsePacket: Welcome_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.Welcome(parsePacket)
						})
					}
				}
				break;

			case 2462931757: // AckUploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckUploadIntermediateCode_Packet = {
							itprId,
							vmId,
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

			case 3529554811: // AckIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getUint8()
						const vmId = packet.getInt32()
						const result = packet.getUint8()
						const count = packet.getUint8()
						const index = packet.getUint8()
						const totalBufferSize = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket: AckIntermediateCode_Packet = {
							itprId,
							vmId,
							result,
							count,
							index,
							totalBufferSize,
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

			case 641506255: // SpawnTotalInterpreterInfo
				{
					if (option == 1) { // binary?
						const totalCount = packet.getInt32()
						const index = packet.getInt32()
						const itprId = packet.getInt32()
						const vmIds = packet.getInt32Array()
						const parsePacket: SpawnTotalInterpreterInfo_Packet = {
							totalCount,
							index,
							itprId,
							vmIds,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SpawnTotalInterpreterInfo(parsePacket)
						})
					} else { // json?
						const parsePacket: SpawnTotalInterpreterInfo_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SpawnTotalInterpreterInfo(parsePacket)
						})
					}
				}
				break;

			case 3321369215: // SpawnInterpreterInfo
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parentVmId = packet.getInt32()
						const vmId = packet.getInt32()
						const nodeFileName = packet.getStr()
						const nodeName = packet.getStr()
						const parsePacket: SpawnInterpreterInfo_Packet = {
							itprId,
							parentVmId,
							vmId,
							nodeFileName,
							nodeName,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SpawnInterpreterInfo(parsePacket)
						})
					} else { // json?
						const parsePacket: SpawnInterpreterInfo_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SpawnInterpreterInfo(parsePacket)
						})
					}
				}
				break;

			case 1233636689: // RemoveInterpreter
				{
					if (option == 1) { // binary?
						const vmId = packet.getInt32()
						const parsePacket: RemoveInterpreter_Packet = {
							vmId,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.RemoveInterpreter(parsePacket)
						})
					} else { // json?
						const parsePacket: RemoveInterpreter_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.RemoveInterpreter(parsePacket)
						})
					}
				}
				break;

			case 3610915871: // AckRun
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

			case 3508334431: // AckOneStep
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

			case 599066917: // AckResumeRun
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

			case 3655568643: // AckBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckBreak_Packet = {
							itprId,
							vmId,
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

			case 3327558841: // AckBreakPoint
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const enable = packet.getBool()
						const id = packet.getUint32()
						const result = packet.getInt32()
						const parsePacket: AckBreakPoint_Packet = {
							itprId,
							vmId,
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

			case 3614765391: // AckStop
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

			case 3422905177: // AckInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket: AckInput_Packet = {
							itprId,
							vmId,
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

			case 4273588753: // AckEvent
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const eventName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckEvent_Packet = {
							itprId,
							vmId,
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

			case 831321155: // AckStepDebugType
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

			case 1917526265: // AckDebugInfo
				{
					if (option == 1) { // binary?
						const vmIds = packet.getInt32Array()
						const result = packet.getInt32()
						const parsePacket: AckDebugInfo_Packet = {
							vmIds,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckDebugInfo(parsePacket)
						})
					} else { // json?
						const parsePacket: AckDebugInfo_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckDebugInfo(parsePacket)
						})
					}
				}
				break;

			case 910405673: // AckChangeVariable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket: AckChangeVariable_Packet = {
							itprId,
							vmId,
							varName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckChangeVariable(parsePacket)
						})
					} else { // json?
						const parsePacket: AckChangeVariable_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.AckChangeVariable(parsePacket)
						})
					}
				}
				break;

			case 601265629: // SyncVMInstruction
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const indices = packet.getUint16Array()
						const parsePacket: SyncVMInstruction_Packet = {
							itprId,
							vmId,
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

			case 398018375: // SyncVMRegister
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const infoType = packet.getInt32()
						const reg = Parse_sRegister(packet)
						const parsePacket: SyncVMRegister_Packet = {
							itprId,
							vmId,
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

			case 20168650: // SyncVMSymbolTable
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const start = packet.getUint32()
						const count = packet.getUint32()
						const symbol = Parse_sSyncSymbolVector(packet)
						const parsePacket: SyncVMSymbolTable_Packet = {
							itprId,
							vmId,
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

			case 2811960017: // SyncVMOutput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const output = packet.getStr()
						const parsePacket: SyncVMOutput_Packet = {
							itprId,
							vmId,
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

			case 2243507416: // SyncVMWidgets
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const widgetName = packet.getStr()
						const parsePacket: SyncVMWidgets_Packet = {
							itprId,
							vmId,
							widgetName,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMWidgets(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMWidgets_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMWidgets(parsePacket)
						})
					}
				}
				break;

			case 1959519461: // SyncVMArray
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const totalSize = packet.getUint32()
						const array = packet.getTypeVariantVector()
						const parsePacket: SyncVMArray_Packet = {
							itprId,
							vmId,
							varName,
							startIdx,
							totalSize,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArray(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMArray_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArray(parsePacket)
						})
					}
				}
				break;

			case 3675896516: // SyncVMArrayBool
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const totalSize = packet.getUint32()
						const array = packet.getUint8Array()
						const parsePacket: SyncVMArrayBool_Packet = {
							itprId,
							vmId,
							varName,
							startIdx,
							totalSize,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArrayBool(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMArrayBool_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArrayBool(parsePacket)
						})
					}
				}
				break;

			case 1234136920: // SyncVMArrayNumber
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const totalSize = packet.getUint32()
						const array = packet.getFloat32Array()
						const parsePacket: SyncVMArrayNumber_Packet = {
							itprId,
							vmId,
							varName,
							startIdx,
							totalSize,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArrayNumber(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMArrayNumber_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArrayNumber(parsePacket)
						})
					}
				}
				break;

			case 1603321848: // SyncVMArrayString
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const varName = packet.getStr()
						const startIdx = packet.getUint32()
						const totalSize = packet.getUint32()
						const array = packet.getStrArray()
						const parsePacket: SyncVMArrayString_Packet = {
							itprId,
							vmId,
							varName,
							startIdx,
							totalSize,
							array,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArrayString(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMArrayString_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMArrayString(parsePacket)
						})
					}
				}
				break;

			case 4149275049: // SyncVMTimer
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const vmId = packet.getInt32()
						const scopeName = packet.getStr()
						const timerId = packet.getInt32()
						const time = packet.getInt32()
						const actionType = packet.getInt32()
						const parsePacket: SyncVMTimer_Packet = {
							itprId,
							vmId,
							scopeName,
							timerId,
							time,
							actionType,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMTimer(parsePacket)
						})
					} else { // json?
						const parsePacket: SyncVMTimer_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.SyncVMTimer(parsePacket)
						})
					}
				}
				break;

			case 445946481: // ExecuteCustomFunction
				{
					if (option == 1) { // binary?
						const fnName = packet.getStr()
						const args = packet.getMapStrArray()
						const parsePacket: ExecuteCustomFunction_Packet = {
							fnName,
							args,
						}
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.ExecuteCustomFunction(parsePacket)
						})
					} else { // json?
						const parsePacket: ExecuteCustomFunction_Packet = 
							JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof h2r_ProtocolHandler)
								handler.ExecuteCustomFunction(parsePacket)
						})
					}
				}
				break;

			case 2075525090: // AckHeartBeat
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

	// Protocol: UploadIntermediateCode
	UploadIntermediateCode(isBinary: boolean, itprId: number, vmId: number, code: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(code)
			this.node?.sendPacketBinary(5301, 201763535, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				code,
			}
			this.node?.sendPacket(5301, 201763535, packet)
		}
	}
	
	// Protocol: ReqIntermediateCode
	ReqIntermediateCode(isBinary: boolean, itprId: number, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5301, 2564628098, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			this.node?.sendPacket(5301, 2564628098, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(isBinary: boolean, itprId: number, runType: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(runType)
			this.node?.sendPacketBinary(5301, 4041661876, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				runType,
			}
			this.node?.sendPacket(5301, 4041661876, packet)
		}
	}
	
	// Protocol: ReqOneStep
	ReqOneStep(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 250175983, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 250175983, packet)
		}
	}
	
	// Protocol: ReqResumeRun
	ReqResumeRun(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 433520154, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 433520154, packet)
		}
	}
	
	// Protocol: ReqBreak
	ReqBreak(isBinary: boolean, itprId: number, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5301, 1318526848, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			this.node?.sendPacket(5301, 1318526848, packet)
		}
	}
	
	// Protocol: ReqBreakPoint
	ReqBreakPoint(isBinary: boolean, itprId: number, vmId: number, enable: boolean, id: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			this.node?.sendPacketBinary(5301, 3344680000, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
			}
			this.node?.sendPacket(5301, 3344680000, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 4009815483, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 4009815483, packet)
		}
	}
	
	// Protocol: ReqInput
	ReqInput(isBinary: boolean, itprId: number, vmId: number, eventName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			this.node?.sendPacketBinary(5301, 1145831178, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
			}
			this.node?.sendPacket(5301, 1145831178, packet)
		}
	}
	
	// Protocol: ReqEvent
	ReqEvent(isBinary: boolean, itprId: number, vmId: number, eventName: string, values: Map<string,string[]>, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			packet.pushMapStrArray(values)
			this.node?.sendPacketBinary(5301, 1246662946, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				values,
			}
			this.node?.sendPacket(5301, 1246662946, packet)
		}
	}
	
	// Protocol: ReqStepDebugType
	ReqStepDebugType(isBinary: boolean, stepDbgType: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			this.node?.sendPacketBinary(5301, 2764328627, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
			}
			this.node?.sendPacket(5301, 2764328627, packet)
		}
	}
	
	// Protocol: ReqDebugInfo
	ReqDebugInfo(isBinary: boolean, vmIds: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32Array(vmIds)
			this.node?.sendPacketBinary(5301, 207775889, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
			}
			this.node?.sendPacket(5301, 207775889, packet)
		}
	}
	
	// Protocol: ReqVariableInfo
	ReqVariableInfo(isBinary: boolean, itprId: number, vmId: number, varName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			this.node?.sendPacketBinary(5301, 1419406571, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
			}
			this.node?.sendPacket(5301, 1419406571, packet)
		}
	}
	
	// Protocol: ReqChangeVariable
	ReqChangeVariable(isBinary: boolean, itprId: number, vmId: number, varName: string, value: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushStr(value)
			this.node?.sendPacketBinary(5301, 1095285398, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				value,
			}
			this.node?.sendPacket(5301, 1095285398, packet)
		}
	}
	
	// Protocol: ReqHeartBeat
	ReqHeartBeat(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5301, 2149460281, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5301, 2149460281, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol 
//------------------------------------------------------------------------
export class h2r_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			this.node?.sendPacketBinary(5300, 3047429, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(5300, 3047429, packet)
		}
	}
	
	// Protocol: AckUploadIntermediateCode
	AckUploadIntermediateCode(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 2462931757, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 2462931757, packet)
		}
	}
	
	// Protocol: AckIntermediateCode
	AckIntermediateCode(isBinary: boolean, itprId: number, vmId: number, result: number, count: number, index: number, totalBufferSize: number, data: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushUint8(itprId)
			packet.pushInt32(vmId)
			packet.pushUint8(result)
			packet.pushUint8(count)
			packet.pushUint8(index)
			packet.pushUint32(totalBufferSize)
			packet.pushUint8Array(data)
			this.node?.sendPacketBinary(5300, 3529554811, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
				count,
				index,
				totalBufferSize,
				data,
			}
			this.node?.sendPacket(5300, 3529554811, packet)
		}
	}
	
	// Protocol: SpawnTotalInterpreterInfo
	SpawnTotalInterpreterInfo(isBinary: boolean, totalCount: number, index: number, itprId: number, vmIds: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(totalCount)
			packet.pushInt32(index)
			packet.pushInt32(itprId)
			packet.pushInt32Array(vmIds)
			this.node?.sendPacketBinary(5300, 641506255, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				totalCount,
				index,
				itprId,
				vmIds,
			}
			this.node?.sendPacket(5300, 641506255, packet)
		}
	}
	
	// Protocol: SpawnInterpreterInfo
	SpawnInterpreterInfo(isBinary: boolean, itprId: number, parentVmId: number, vmId: number, nodeFileName: string, nodeName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(parentVmId)
			packet.pushInt32(vmId)
			packet.pushStr(nodeFileName)
			packet.pushStr(nodeName)
			this.node?.sendPacketBinary(5300, 3321369215, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				parentVmId,
				vmId,
				nodeFileName,
				nodeName,
			}
			this.node?.sendPacket(5300, 3321369215, packet)
		}
	}
	
	// Protocol: RemoveInterpreter
	RemoveInterpreter(isBinary: boolean, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5300, 1233636689, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmId,
			}
			this.node?.sendPacket(5300, 1233636689, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3610915871, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 3610915871, packet)
		}
	}
	
	// Protocol: AckOneStep
	AckOneStep(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3508334431, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 3508334431, packet)
		}
	}
	
	// Protocol: AckResumeRun
	AckResumeRun(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 599066917, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 599066917, packet)
		}
	}
	
	// Protocol: AckBreak
	AckBreak(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3655568643, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 3655568643, packet)
		}
	}
	
	// Protocol: AckBreakPoint
	AckBreakPoint(isBinary: boolean, itprId: number, vmId: number, enable: boolean, id: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushBool(enable)
			packet.pushUint32(id)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3327558841, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
				result,
			}
			this.node?.sendPacket(5300, 3327558841, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3614765391, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 3614765391, packet)
		}
	}
	
	// Protocol: AckInput
	AckInput(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3422905177, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 3422905177, packet)
		}
	}
	
	// Protocol: AckEvent
	AckEvent(isBinary: boolean, itprId: number, vmId: number, eventName: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 4273588753, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				result,
			}
			this.node?.sendPacket(5300, 4273588753, packet)
		}
	}
	
	// Protocol: AckStepDebugType
	AckStepDebugType(isBinary: boolean, stepDbgType: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 831321155, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
				result,
			}
			this.node?.sendPacket(5300, 831321155, packet)
		}
	}
	
	// Protocol: AckDebugInfo
	AckDebugInfo(isBinary: boolean, vmIds: number[], result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32Array(vmIds)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 1917526265, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
				result,
			}
			this.node?.sendPacket(5300, 1917526265, packet)
		}
	}
	
	// Protocol: AckChangeVariable
	AckChangeVariable(isBinary: boolean, itprId: number, vmId: number, varName: string, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 910405673, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				result,
			}
			this.node?.sendPacket(5300, 910405673, packet)
		}
	}
	
	// Protocol: SyncVMInstruction
	SyncVMInstruction(isBinary: boolean, itprId: number, vmId: number, indices: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushUint16Array(indices)
			this.node?.sendPacketBinary(5300, 601265629, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				indices,
			}
			this.node?.sendPacket(5300, 601265629, packet)
		}
	}
	
	// Protocol: SyncVMRegister
	SyncVMRegister(isBinary: boolean, itprId: number, vmId: number, infoType: number, reg: sRegister, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(infoType)
			Make_sRegister(packet,reg)
			this.node?.sendPacketBinary(5300, 398018375, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				infoType,
				reg,
			}
			this.node?.sendPacket(5300, 398018375, packet)
		}
	}
	
	// Protocol: SyncVMSymbolTable
	SyncVMSymbolTable(isBinary: boolean, itprId: number, vmId: number, start: number, count: number, symbol: sSyncSymbol[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushUint32(start)
			packet.pushUint32(count)
			Make_sSyncSymbolVector(packet,symbol)
			this.node?.sendPacketBinary(5300, 20168650, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				start,
				count,
				symbol,
			}
			this.node?.sendPacket(5300, 20168650, packet)
		}
	}
	
	// Protocol: SyncVMOutput
	SyncVMOutput(isBinary: boolean, itprId: number, vmId: number, output: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(output)
			this.node?.sendPacketBinary(5300, 2811960017, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				output,
			}
			this.node?.sendPacket(5300, 2811960017, packet)
		}
	}
	
	// Protocol: SyncVMWidgets
	SyncVMWidgets(isBinary: boolean, itprId: number, vmId: number, widgetName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(widgetName)
			this.node?.sendPacketBinary(5300, 2243507416, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				widgetName,
			}
			this.node?.sendPacket(5300, 2243507416, packet)
		}
	}
	
	// Protocol: SyncVMArray
	SyncVMArray(isBinary: boolean, itprId: number, vmId: number, varName: string, startIdx: number, totalSize: number, array: TypeVariant[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushUint32(totalSize)
			packet.pushTypeVariantVector(array)
			this.node?.sendPacketBinary(5300, 1959519461, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 1959519461, packet)
		}
	}
	
	// Protocol: SyncVMArrayBool
	SyncVMArrayBool(isBinary: boolean, itprId: number, vmId: number, varName: string, startIdx: number, totalSize: number, array: boolean[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushUint32(totalSize)
			packet.pushBoolArray(array)
			this.node?.sendPacketBinary(5300, 3675896516, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 3675896516, packet)
		}
	}
	
	// Protocol: SyncVMArrayNumber
	SyncVMArrayNumber(isBinary: boolean, itprId: number, vmId: number, varName: string, startIdx: number, totalSize: number, array: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushUint32(totalSize)
			packet.pushFloat32Array(array)
			this.node?.sendPacketBinary(5300, 1234136920, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 1234136920, packet)
		}
	}
	
	// Protocol: SyncVMArrayString
	SyncVMArrayString(isBinary: boolean, itprId: number, vmId: number, varName: string, startIdx: number, totalSize: number, array: string[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			packet.pushUint32(startIdx)
			packet.pushUint32(totalSize)
			packet.pushStrArray(array)
			this.node?.sendPacketBinary(5300, 1603321848, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 1603321848, packet)
		}
	}
	
	// Protocol: SyncVMTimer
	SyncVMTimer(isBinary: boolean, itprId: number, vmId: number, scopeName: string, timerId: number, time: number, actionType: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(scopeName)
			packet.pushInt32(timerId)
			packet.pushInt32(time)
			packet.pushInt32(actionType)
			this.node?.sendPacketBinary(5300, 4149275049, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				scopeName,
				timerId,
				time,
				actionType,
			}
			this.node?.sendPacket(5300, 4149275049, packet)
		}
	}
	
	// Protocol: ExecuteCustomFunction
	ExecuteCustomFunction(isBinary: boolean, fnName: string, args: Map<string,string[]>, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(fnName)
			packet.pushMapStrArray(args)
			this.node?.sendPacketBinary(5300, 445946481, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				fnName,
				args,
			}
			this.node?.sendPacket(5300, 445946481, packet)
		}
	}
	
	// Protocol: AckHeartBeat
	AckHeartBeat(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5300, 2075525090, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5300, 2075525090, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Handler
//------------------------------------------------------------------------
export class r2h_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

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
	ReqDebugInfo = (packet: remotedbg2.ReqDebugInfo_Packet) => { }
	ReqVariableInfo = (packet: remotedbg2.ReqVariableInfo_Packet) => { }
	ReqChangeVariable = (packet: remotedbg2.ReqChangeVariable_Packet) => { }
	ReqHeartBeat = (packet: remotedbg2.ReqHeartBeat_Packet) => { }
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol Handler
//------------------------------------------------------------------------
export class h2r_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: remotedbg2.Welcome_Packet) => { }
	AckUploadIntermediateCode = (packet: remotedbg2.AckUploadIntermediateCode_Packet) => { }
	AckIntermediateCode = (packet: remotedbg2.AckIntermediateCode_Packet) => { }
	SpawnTotalInterpreterInfo = (packet: remotedbg2.SpawnTotalInterpreterInfo_Packet) => { }
	SpawnInterpreterInfo = (packet: remotedbg2.SpawnInterpreterInfo_Packet) => { }
	RemoveInterpreter = (packet: remotedbg2.RemoveInterpreter_Packet) => { }
	AckRun = (packet: remotedbg2.AckRun_Packet) => { }
	AckOneStep = (packet: remotedbg2.AckOneStep_Packet) => { }
	AckResumeRun = (packet: remotedbg2.AckResumeRun_Packet) => { }
	AckBreak = (packet: remotedbg2.AckBreak_Packet) => { }
	AckBreakPoint = (packet: remotedbg2.AckBreakPoint_Packet) => { }
	AckStop = (packet: remotedbg2.AckStop_Packet) => { }
	AckInput = (packet: remotedbg2.AckInput_Packet) => { }
	AckEvent = (packet: remotedbg2.AckEvent_Packet) => { }
	AckStepDebugType = (packet: remotedbg2.AckStepDebugType_Packet) => { }
	AckDebugInfo = (packet: remotedbg2.AckDebugInfo_Packet) => { }
	AckChangeVariable = (packet: remotedbg2.AckChangeVariable_Packet) => { }
	SyncVMInstruction = (packet: remotedbg2.SyncVMInstruction_Packet) => { }
	SyncVMRegister = (packet: remotedbg2.SyncVMRegister_Packet) => { }
	SyncVMSymbolTable = (packet: remotedbg2.SyncVMSymbolTable_Packet) => { }
	SyncVMOutput = (packet: remotedbg2.SyncVMOutput_Packet) => { }
	SyncVMWidgets = (packet: remotedbg2.SyncVMWidgets_Packet) => { }
	SyncVMArray = (packet: remotedbg2.SyncVMArray_Packet) => { }
	SyncVMArrayBool = (packet: remotedbg2.SyncVMArrayBool_Packet) => { }
	SyncVMArrayNumber = (packet: remotedbg2.SyncVMArrayNumber_Packet) => { }
	SyncVMArrayString = (packet: remotedbg2.SyncVMArrayString_Packet) => { }
	SyncVMTimer = (packet: remotedbg2.SyncVMTimer_Packet) => { }
	ExecuteCustomFunction = (packet: remotedbg2.ExecuteCustomFunction_Packet) => { }
	AckHeartBeat = (packet: remotedbg2.AckHeartBeat_Packet) => { }
}

}

