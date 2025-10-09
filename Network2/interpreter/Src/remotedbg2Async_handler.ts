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
	export type ReqVMTree_Packet = {
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
	export type AckVMTree_Packet = {
		id: number, 
		result: number, 
	}
	export type AckVMTreeStream_Packet = {
		id: number, 
		count: number, 
		index: number, 
		totalBufferSize: number, 
		data: Uint8Array | null, 
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
			case 201763535: // UploadIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.UploadIntermediateCode(parsePacket)
					})
				}
				break;

			case 2564628098: // ReqIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqIntermediateCode(parsePacket)
					})
				}
				break;

			case 4041661876: // ReqRun
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqRun(parsePacket)
					})
				}
				break;

			case 250175983: // ReqOneStep
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqOneStep(parsePacket)
					})
				}
				break;

			case 433520154: // ReqResumeRun
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqResumeRun(parsePacket)
					})
				}
				break;

			case 1318526848: // ReqBreak
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqBreak(parsePacket)
					})
				}
				break;

			case 3344680000: // ReqBreakPoint
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqBreakPoint(parsePacket)
					})
				}
				break;

			case 4009815483: // ReqStop
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqStop(parsePacket)
					})
				}
				break;

			case 1145831178: // ReqInput
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqInput(parsePacket)
					})
				}
				break;

			case 1246662946: // ReqEvent
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqEvent(parsePacket)
					})
				}
				break;

			case 2764328627: // ReqStepDebugType
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqStepDebugType(parsePacket)
					})
				}
				break;

			case 207775889: // ReqDebugInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqDebugInfo(parsePacket)
					})
				}
				break;

			case 1419406571: // ReqVariableInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqVariableInfo(parsePacket)
					})
				}
				break;

			case 1095285398: // ReqChangeVariable
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqChangeVariable(parsePacket)
					})
				}
				break;

			case 1696701698: // ReqVMTree
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqVMTree(parsePacket)
					})
				}
				break;

			case 2149460281: // ReqHeartBeat
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
			case 3047429: // Welcome
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.Welcome(parsePacket)
					})
				}
				break;

			case 2462931757: // AckUploadIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckUploadIntermediateCode(parsePacket)
					})
				}
				break;

			case 3529554811: // AckIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckIntermediateCode(parsePacket)
					})
				}
				break;

			case 641506255: // SpawnTotalInterpreterInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SpawnTotalInterpreterInfo(parsePacket)
					})
				}
				break;

			case 3321369215: // SpawnInterpreterInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SpawnInterpreterInfo(parsePacket)
					})
				}
				break;

			case 1233636689: // RemoveInterpreter
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.RemoveInterpreter(parsePacket)
					})
				}
				break;

			case 3610915871: // AckRun
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckRun(parsePacket)
					})
				}
				break;

			case 3508334431: // AckOneStep
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckOneStep(parsePacket)
					})
				}
				break;

			case 599066917: // AckResumeRun
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckResumeRun(parsePacket)
					})
				}
				break;

			case 3655568643: // AckBreak
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckBreak(parsePacket)
					})
				}
				break;

			case 3327558841: // AckBreakPoint
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckBreakPoint(parsePacket)
					})
				}
				break;

			case 3614765391: // AckStop
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckStop(parsePacket)
					})
				}
				break;

			case 3422905177: // AckInput
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckInput(parsePacket)
					})
				}
				break;

			case 4273588753: // AckEvent
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckEvent(parsePacket)
					})
				}
				break;

			case 831321155: // AckStepDebugType
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckStepDebugType(parsePacket)
					})
				}
				break;

			case 1917526265: // AckDebugInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckDebugInfo(parsePacket)
					})
				}
				break;

			case 910405673: // AckChangeVariable
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckChangeVariable(parsePacket)
					})
				}
				break;

			case 3389117266: // AckVMTree
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckVMTree(parsePacket)
					})
				}
				break;

			case 3768290937: // AckVMTreeStream
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckVMTreeStream(parsePacket)
					})
				}
				break;

			case 601265629: // SyncVMInstruction
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMInstruction(parsePacket)
					})
				}
				break;

			case 398018375: // SyncVMRegister
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMRegister(parsePacket)
					})
				}
				break;

			case 20168650: // SyncVMSymbolTable
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMSymbolTable(parsePacket)
					})
				}
				break;

			case 2811960017: // SyncVMOutput
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMOutput(parsePacket)
					})
				}
				break;

			case 2243507416: // SyncVMWidgets
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMWidgets(parsePacket)
					})
				}
				break;

			case 1959519461: // SyncVMArray
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArray(parsePacket)
					})
				}
				break;

			case 3675896516: // SyncVMArrayBool
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayBool(parsePacket)
					})
				}
				break;

			case 1234136920: // SyncVMArrayNumber
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayNumber(parsePacket)
					})
				}
				break;

			case 1603321848: // SyncVMArrayString
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayString(parsePacket)
					})
				}
				break;

			case 4149275049: // SyncVMTimer
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMTimer(parsePacket)
					})
				}
				break;

			case 445946481: // ExecuteCustomFunction
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.ExecuteCustomFunction(parsePacket)
					})
				}
				break;

			case 2075525090: // AckHeartBeat
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
	
	// Protocol: ReqVMTree
	ReqVMTree(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5301, 1696701698, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5301, 1696701698, packet)
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
// remotedbg2Async h2r Protocol 
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
	
	// Protocol: AckVMTree
	AckVMTree(isBinary: boolean, id: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(id)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3389117266, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				result,
			}
			this.node?.sendPacket(5300, 3389117266, packet)
		}
	}
	
	// Protocol: AckVMTreeStream
	AckVMTreeStream(isBinary: boolean, id: number, count: number, index: number, totalBufferSize: number, data: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(id)
			packet.pushUint16(count)
			packet.pushUint16(index)
			packet.pushUint32(totalBufferSize)
			packet.pushUint8Array(data)
			this.node?.sendPacketBinary(5300, 3768290937, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				id,
				count,
				index,
				totalBufferSize,
				data,
			}
			this.node?.sendPacket(5300, 3768290937, packet)
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
	ReqVMTree = (packet: remotedbg2Async.ReqVMTree_Packet) => { }
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
	AckVMTree = (packet: remotedbg2Async.AckVMTree_Packet) => { }
	AckVMTreeStream = (packet: remotedbg2Async.AckVMTreeStream_Packet) => { }
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

