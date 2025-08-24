//------------------------------------------------------------------------
// Name:    remotedbg2
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
import TypeVariant from "../common/variant";
import { Network } from "../network/network";

export namespace remotedbg2Async {
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
// remotedbg2Async r2h Protocol Dispatcher
//------------------------------------------------------------------------
export class r2h_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(5301)
	}
	dispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {
		switch (packetId) {
			case 457514035: // UploadIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.UploadIntermediateCode(parsePacket)
					})
				}
				break;

			case 795015035: // ReqIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqIntermediateCode(parsePacket)
					})
				}
				break;

			case 682133217: // ReqRun
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqRun(parsePacket)
					})
				}
				break;

			case 2870899790: // ReqOneStep
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqOneStep(parsePacket)
					})
				}
				break;

			case 3748318806: // ReqResumeRun
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqResumeRun(parsePacket)
					})
				}
				break;

			case 2831828176: // ReqBreak
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqBreak(parsePacket)
					})
				}
				break;

			case 1838451287: // ReqBreakPoint
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqBreakPoint(parsePacket)
					})
				}
				break;

			case 1371786614: // ReqStop
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqStop(parsePacket)
					})
				}
				break;

			case 3482448363: // ReqInput
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqInput(parsePacket)
					})
				}
				break;

			case 2822441921: // ReqEvent
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqEvent(parsePacket)
					})
				}
				break;

			case 2515517823: // ReqStepDebugType
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqStepDebugType(parsePacket)
					})
				}
				break;

			case 3176404268: // ReqDebugInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqDebugInfo(parsePacket)
					})
				}
				break;

			case 695140836: // ReqVariableInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqVariableInfo(parsePacket)
					})
				}
				break;

			case 2174514456: // ReqChangeVariable
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqChangeVariable(parsePacket)
					})
				}
				break;

			case 3763178599: // ReqHeartBeat
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqHeartBeat(parsePacket)
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
// remotedbg2Async h2r Protocol Dispatcher
//------------------------------------------------------------------------
export class h2r_Dispatcher extends Network.Dispatcher {
	 constructor() {
		super(5300)
	}
	dispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {
		switch (packetId) {
			case 3696227036: // Welcome
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.Welcome(parsePacket)
					})
				}
				break;

			case 275980554: // AckUploadIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckUploadIntermediateCode(parsePacket)
					})
				}
				break;

			case 994148536: // AckIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckIntermediateCode(parsePacket)
					})
				}
				break;

			case 3719566959: // SpawnTotalInterpreterInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SpawnTotalInterpreterInfo(parsePacket)
					})
				}
				break;

			case 24455197: // SpawnInterpreterInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SpawnInterpreterInfo(parsePacket)
					})
				}
				break;

			case 1638782716: // RemoveInterpreter
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.RemoveInterpreter(parsePacket)
					})
				}
				break;

			case 2902549966: // AckRun
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckRun(parsePacket)
					})
				}
				break;

			case 595084065: // AckOneStep
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckOneStep(parsePacket)
					})
				}
				break;

			case 601884097: // AckResumeRun
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckResumeRun(parsePacket)
					})
				}
				break;

			case 1020437751: // AckBreak
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckBreak(parsePacket)
					})
				}
				break;

			case 1057870316: // AckBreakPoint
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckBreakPoint(parsePacket)
					})
				}
				break;

			case 1755796261: // AckStop
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckStop(parsePacket)
					})
				}
				break;

			case 3028964340: // AckInput
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckInput(parsePacket)
					})
				}
				break;

			case 1257402006: // AckEvent
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckEvent(parsePacket)
					})
				}
				break;

			case 586554168: // AckStepDebugType
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckStepDebugType(parsePacket)
					})
				}
				break;

			case 735183747: // AckDebugInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckDebugInfo(parsePacket)
					})
				}
				break;

			case 2665558547: // AckChangeVariable
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckChangeVariable(parsePacket)
					})
				}
				break;

			case 2385736043: // SyncVMInstruction
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMInstruction(parsePacket)
					})
				}
				break;

			case 3080259290: // SyncVMRegister
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMRegister(parsePacket)
					})
				}
				break;

			case 4266750021: // SyncVMSymbolTable
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMSymbolTable(parsePacket)
					})
				}
				break;

			case 2045997333: // SyncVMOutput
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMOutput(parsePacket)
					})
				}
				break;

			case 2686259685: // SyncVMWidgets
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMWidgets(parsePacket)
					})
				}
				break;

			case 3305233207: // SyncVMArray
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArray(parsePacket)
					})
				}
				break;

			case 2817606738: // SyncVMArrayBool
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayBool(parsePacket)
					})
				}
				break;

			case 7391856: // SyncVMArrayNumber
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayNumber(parsePacket)
					})
				}
				break;

			case 3300699295: // SyncVMArrayString
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayString(parsePacket)
					})
				}
				break;

			case 909166531: // SyncVMTimer
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMTimer(parsePacket)
					})
				}
				break;

			case 3996115506: // ExecuteCustomFunction
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.ExecuteCustomFunction(parsePacket)
					})
				}
				break;

			case 1130150688: // AckHeartBeat
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckHeartBeat(parsePacket)
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
// remotedbg2Async r2h Protocol 
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
			this.node?.sendPacketBinary(5301, 457514035, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				code,
			}
			this.node?.sendPacket(5301, 457514035, packet)
		}
	}
	
	// Protocol: ReqIntermediateCode
	ReqIntermediateCode(isBinary: boolean, itprId: number, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5301, 795015035, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			this.node?.sendPacket(5301, 795015035, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(isBinary: boolean, itprId: number, runType: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(runType)
			this.node?.sendPacketBinary(5301, 682133217, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				runType,
			}
			this.node?.sendPacket(5301, 682133217, packet)
		}
	}
	
	// Protocol: ReqOneStep
	ReqOneStep(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 2870899790, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 2870899790, packet)
		}
	}
	
	// Protocol: ReqResumeRun
	ReqResumeRun(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 3748318806, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 3748318806, packet)
		}
	}
	
	// Protocol: ReqBreak
	ReqBreak(isBinary: boolean, itprId: number, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5301, 2831828176, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			this.node?.sendPacket(5301, 2831828176, packet)
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
			this.node?.sendPacketBinary(5301, 1838451287, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
			}
			this.node?.sendPacket(5301, 1838451287, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 1371786614, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 1371786614, packet)
		}
	}
	
	// Protocol: ReqInput
	ReqInput(isBinary: boolean, itprId: number, vmId: number, eventName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			this.node?.sendPacketBinary(5301, 3482448363, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
			}
			this.node?.sendPacket(5301, 3482448363, packet)
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
			this.node?.sendPacketBinary(5301, 2822441921, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				values,
			}
			this.node?.sendPacket(5301, 2822441921, packet)
		}
	}
	
	// Protocol: ReqStepDebugType
	ReqStepDebugType(isBinary: boolean, stepDbgType: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			this.node?.sendPacketBinary(5301, 2515517823, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
			}
			this.node?.sendPacket(5301, 2515517823, packet)
		}
	}
	
	// Protocol: ReqDebugInfo
	ReqDebugInfo(isBinary: boolean, vmIds: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32Array(vmIds)
			this.node?.sendPacketBinary(5301, 3176404268, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
			}
			this.node?.sendPacket(5301, 3176404268, packet)
		}
	}
	
	// Protocol: ReqVariableInfo
	ReqVariableInfo(isBinary: boolean, itprId: number, vmId: number, varName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			this.node?.sendPacketBinary(5301, 695140836, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
			}
			this.node?.sendPacket(5301, 695140836, packet)
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
			this.node?.sendPacketBinary(5301, 2174514456, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				value,
			}
			this.node?.sendPacket(5301, 2174514456, packet)
		}
	}
	
	// Protocol: ReqHeartBeat
	ReqHeartBeat(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5301, 3763178599, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5301, 3763178599, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2Async h2r Protocol 
//------------------------------------------------------------------------
export class h2r_Protocol extends Network.Protocol {
	 constructor() { super() }

	// Protocol: Welcome
	Welcome(isBinary: boolean, msg: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(msg)
			this.node?.sendPacketBinary(5300, 3696227036, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(5300, 3696227036, packet)
		}
	}
	
	// Protocol: AckUploadIntermediateCode
	AckUploadIntermediateCode(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 275980554, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 275980554, packet)
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
			this.node?.sendPacketBinary(5300, 994148536, packet.buff, packet.offset)
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
			this.node?.sendPacket(5300, 994148536, packet)
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
			this.node?.sendPacketBinary(5300, 3719566959, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				totalCount,
				index,
				itprId,
				vmIds,
			}
			this.node?.sendPacket(5300, 3719566959, packet)
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
			this.node?.sendPacketBinary(5300, 24455197, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				parentVmId,
				vmId,
				nodeFileName,
				nodeName,
			}
			this.node?.sendPacket(5300, 24455197, packet)
		}
	}
	
	// Protocol: RemoveInterpreter
	RemoveInterpreter(isBinary: boolean, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5300, 1638782716, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmId,
			}
			this.node?.sendPacket(5300, 1638782716, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 2902549966, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 2902549966, packet)
		}
	}
	
	// Protocol: AckOneStep
	AckOneStep(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 595084065, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 595084065, packet)
		}
	}
	
	// Protocol: AckResumeRun
	AckResumeRun(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 601884097, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 601884097, packet)
		}
	}
	
	// Protocol: AckBreak
	AckBreak(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 1020437751, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 1020437751, packet)
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
			this.node?.sendPacketBinary(5300, 1057870316, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
				result,
			}
			this.node?.sendPacket(5300, 1057870316, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 1755796261, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 1755796261, packet)
		}
	}
	
	// Protocol: AckInput
	AckInput(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3028964340, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 3028964340, packet)
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
			this.node?.sendPacketBinary(5300, 1257402006, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				result,
			}
			this.node?.sendPacket(5300, 1257402006, packet)
		}
	}
	
	// Protocol: AckStepDebugType
	AckStepDebugType(isBinary: boolean, stepDbgType: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 586554168, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
				result,
			}
			this.node?.sendPacket(5300, 586554168, packet)
		}
	}
	
	// Protocol: AckDebugInfo
	AckDebugInfo(isBinary: boolean, vmIds: number[], result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32Array(vmIds)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 735183747, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
				result,
			}
			this.node?.sendPacket(5300, 735183747, packet)
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
			this.node?.sendPacketBinary(5300, 2665558547, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				result,
			}
			this.node?.sendPacket(5300, 2665558547, packet)
		}
	}
	
	// Protocol: SyncVMInstruction
	SyncVMInstruction(isBinary: boolean, itprId: number, vmId: number, indices: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushUint16Array(indices)
			this.node?.sendPacketBinary(5300, 2385736043, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				indices,
			}
			this.node?.sendPacket(5300, 2385736043, packet)
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
			this.node?.sendPacketBinary(5300, 3080259290, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				infoType,
				reg,
			}
			this.node?.sendPacket(5300, 3080259290, packet)
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
			this.node?.sendPacketBinary(5300, 4266750021, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				start,
				count,
				symbol,
			}
			this.node?.sendPacket(5300, 4266750021, packet)
		}
	}
	
	// Protocol: SyncVMOutput
	SyncVMOutput(isBinary: boolean, itprId: number, vmId: number, output: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(output)
			this.node?.sendPacketBinary(5300, 2045997333, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				output,
			}
			this.node?.sendPacket(5300, 2045997333, packet)
		}
	}
	
	// Protocol: SyncVMWidgets
	SyncVMWidgets(isBinary: boolean, itprId: number, vmId: number, widgetName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(widgetName)
			this.node?.sendPacketBinary(5300, 2686259685, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				widgetName,
			}
			this.node?.sendPacket(5300, 2686259685, packet)
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
			this.node?.sendPacketBinary(5300, 3305233207, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 3305233207, packet)
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
			this.node?.sendPacketBinary(5300, 2817606738, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 2817606738, packet)
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
			this.node?.sendPacketBinary(5300, 7391856, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 7391856, packet)
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
			this.node?.sendPacketBinary(5300, 3300699295, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 3300699295, packet)
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
			this.node?.sendPacketBinary(5300, 909166531, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				scopeName,
				timerId,
				time,
				actionType,
			}
			this.node?.sendPacket(5300, 909166531, packet)
		}
	}
	
	// Protocol: ExecuteCustomFunction
	ExecuteCustomFunction(isBinary: boolean, fnName: string, args: Map<string,string[]>, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(fnName)
			packet.pushMapStrArray(args)
			this.node?.sendPacketBinary(5300, 3996115506, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				fnName,
				args,
			}
			this.node?.sendPacket(5300, 3996115506, packet)
		}
	}
	
	// Protocol: AckHeartBeat
	AckHeartBeat(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5300, 1130150688, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5300, 1130150688, packet)
		}
	}
	
}


//------------------------------------------------------------------------
// remotedbg2Async r2h Protocol Handler
//------------------------------------------------------------------------
export class r2h_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	UploadIntermediateCode = (packet: remotedbg2Async.UploadIntermediateCode_Packet) => { }
	ReqIntermediateCode = (packet: remotedbg2Async.ReqIntermediateCode_Packet) => { }
	ReqRun = (packet: remotedbg2Async.ReqRun_Packet) => { }
	ReqOneStep = (packet: remotedbg2Async.ReqOneStep_Packet) => { }
	ReqResumeRun = (packet: remotedbg2Async.ReqResumeRun_Packet) => { }
	ReqBreak = (packet: remotedbg2Async.ReqBreak_Packet) => { }
	ReqBreakPoint = (packet: remotedbg2Async.ReqBreakPoint_Packet) => { }
	ReqStop = (packet: remotedbg2Async.ReqStop_Packet) => { }
	ReqInput = (packet: remotedbg2Async.ReqInput_Packet) => { }
	ReqEvent = (packet: remotedbg2Async.ReqEvent_Packet) => { }
	ReqStepDebugType = (packet: remotedbg2Async.ReqStepDebugType_Packet) => { }
	ReqDebugInfo = (packet: remotedbg2Async.ReqDebugInfo_Packet) => { }
	ReqVariableInfo = (packet: remotedbg2Async.ReqVariableInfo_Packet) => { }
	ReqChangeVariable = (packet: remotedbg2Async.ReqChangeVariable_Packet) => { }
	ReqHeartBeat = (packet: remotedbg2Async.ReqHeartBeat_Packet) => { }
}


//------------------------------------------------------------------------
// remotedbg2Async h2r Protocol Handler
//------------------------------------------------------------------------
export class h2r_ProtocolHandler extends Network.Handler {
	 constructor() { super() } 

	Welcome = (packet: remotedbg2Async.Welcome_Packet) => { }
	AckUploadIntermediateCode = (packet: remotedbg2Async.AckUploadIntermediateCode_Packet) => { }
	AckIntermediateCode = (packet: remotedbg2Async.AckIntermediateCode_Packet) => { }
	SpawnTotalInterpreterInfo = (packet: remotedbg2Async.SpawnTotalInterpreterInfo_Packet) => { }
	SpawnInterpreterInfo = (packet: remotedbg2Async.SpawnInterpreterInfo_Packet) => { }
	RemoveInterpreter = (packet: remotedbg2Async.RemoveInterpreter_Packet) => { }
	AckRun = (packet: remotedbg2Async.AckRun_Packet) => { }
	AckOneStep = (packet: remotedbg2Async.AckOneStep_Packet) => { }
	AckResumeRun = (packet: remotedbg2Async.AckResumeRun_Packet) => { }
	AckBreak = (packet: remotedbg2Async.AckBreak_Packet) => { }
	AckBreakPoint = (packet: remotedbg2Async.AckBreakPoint_Packet) => { }
	AckStop = (packet: remotedbg2Async.AckStop_Packet) => { }
	AckInput = (packet: remotedbg2Async.AckInput_Packet) => { }
	AckEvent = (packet: remotedbg2Async.AckEvent_Packet) => { }
	AckStepDebugType = (packet: remotedbg2Async.AckStepDebugType_Packet) => { }
	AckDebugInfo = (packet: remotedbg2Async.AckDebugInfo_Packet) => { }
	AckChangeVariable = (packet: remotedbg2Async.AckChangeVariable_Packet) => { }
	SyncVMInstruction = (packet: remotedbg2Async.SyncVMInstruction_Packet) => { }
	SyncVMRegister = (packet: remotedbg2Async.SyncVMRegister_Packet) => { }
	SyncVMSymbolTable = (packet: remotedbg2Async.SyncVMSymbolTable_Packet) => { }
	SyncVMOutput = (packet: remotedbg2Async.SyncVMOutput_Packet) => { }
	SyncVMWidgets = (packet: remotedbg2Async.SyncVMWidgets_Packet) => { }
	SyncVMArray = (packet: remotedbg2Async.SyncVMArray_Packet) => { }
	SyncVMArrayBool = (packet: remotedbg2Async.SyncVMArrayBool_Packet) => { }
	SyncVMArrayNumber = (packet: remotedbg2Async.SyncVMArrayNumber_Packet) => { }
	SyncVMArrayString = (packet: remotedbg2Async.SyncVMArrayString_Packet) => { }
	SyncVMTimer = (packet: remotedbg2Async.SyncVMTimer_Packet) => { }
	ExecuteCustomFunction = (packet: remotedbg2Async.ExecuteCustomFunction_Packet) => { }
	AckHeartBeat = (packet: remotedbg2Async.AckHeartBeat_Packet) => { }
}

}

