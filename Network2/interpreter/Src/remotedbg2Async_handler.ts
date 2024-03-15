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
			case 1418562193: // UploadIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.UploadIntermediateCode(parsePacket)
					})
				}
				break;

			case 1644585100: // ReqIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqIntermediateCode(parsePacket)
					})
				}
				break;

			case 923151823: // ReqRun
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqRun(parsePacket)
					})
				}
				break;

			case 2884814738: // ReqOneStep
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqOneStep(parsePacket)
					})
				}
				break;

			case 742173167: // ReqResumeRun
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqResumeRun(parsePacket)
					})
				}
				break;

			case 784411795: // ReqBreak
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqBreak(parsePacket)
					})
				}
				break;

			case 2487089996: // ReqBreakPoint
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqBreakPoint(parsePacket)
					})
				}
				break;

			case 1453251868: // ReqStop
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqStop(parsePacket)
					})
				}
				break;

			case 3140751413: // ReqInput
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqInput(parsePacket)
					})
				}
				break;

			case 186222094: // ReqEvent
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqEvent(parsePacket)
					})
				}
				break;

			case 3084593987: // ReqStepDebugType
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqStepDebugType(parsePacket)
					})
				}
				break;

			case 2166551586: // ReqDebugInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqDebugInfo(parsePacket)
					})
				}
				break;

			case 4127584998: // ReqVariableInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqVariableInfo(parsePacket)
					})
				}
				break;

			case 1626832220: // ReqChangeVariable
				{
					handlers.forEach(handler => {
						if (handler instanceof r2h_ProtocolHandler)
							handler.ReqChangeVariable(parsePacket)
					})
				}
				break;

			case 2532286881: // ReqHeartBeat
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
			case 1281093745: // Welcome
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.Welcome(parsePacket)
					})
				}
				break;

			case 4005257575: // AckUploadIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckUploadIntermediateCode(parsePacket)
					})
				}
				break;

			case 1397310616: // AckIntermediateCode
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckIntermediateCode(parsePacket)
					})
				}
				break;

			case 977092268: // SpawnTotalInterpreterInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SpawnTotalInterpreterInfo(parsePacket)
					})
				}
				break;

			case 762776747: // SpawnInterpreterInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SpawnInterpreterInfo(parsePacket)
					})
				}
				break;

			case 940777945: // RemoveInterpreter
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.RemoveInterpreter(parsePacket)
					})
				}
				break;

			case 4148808214: // AckRun
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckRun(parsePacket)
					})
				}
				break;

			case 3643391279: // AckOneStep
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckOneStep(parsePacket)
					})
				}
				break;

			case 1012496086: // AckResumeRun
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckResumeRun(parsePacket)
					})
				}
				break;

			case 2129545277: // AckBreak
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckBreak(parsePacket)
					})
				}
				break;

			case 2045074648: // AckBreakPoint
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckBreakPoint(parsePacket)
					})
				}
				break;

			case 114435221: // AckStop
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckStop(parsePacket)
					})
				}
				break;

			case 1658444570: // AckInput
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckInput(parsePacket)
					})
				}
				break;

			case 1906481345: // AckEvent
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckEvent(parsePacket)
					})
				}
				break;

			case 4225702489: // AckStepDebugType
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckStepDebugType(parsePacket)
					})
				}
				break;

			case 4276104084: // AckDebugInfo
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckDebugInfo(parsePacket)
					})
				}
				break;

			case 3660358812: // AckChangeVariable
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.AckChangeVariable(parsePacket)
					})
				}
				break;

			case 4206107288: // SyncVMInstruction
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMInstruction(parsePacket)
					})
				}
				break;

			case 3001685594: // SyncVMRegister
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMRegister(parsePacket)
					})
				}
				break;

			case 3045798844: // SyncVMSymbolTable
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMSymbolTable(parsePacket)
					})
				}
				break;

			case 1348120458: // SyncVMOutput
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMOutput(parsePacket)
					})
				}
				break;

			case 323195839: // SyncVMWidgets
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMWidgets(parsePacket)
					})
				}
				break;

			case 1209241191: // SyncVMArray
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArray(parsePacket)
					})
				}
				break;

			case 3278867969: // SyncVMArrayBool
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayBool(parsePacket)
					})
				}
				break;

			case 3822230413: // SyncVMArrayNumber
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayNumber(parsePacket)
					})
				}
				break;

			case 2291689449: // SyncVMArrayString
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMArrayString(parsePacket)
					})
				}
				break;

			case 3712761243: // SyncVMTimer
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.SyncVMTimer(parsePacket)
					})
				}
				break;

			case 638159302: // ExecuteCustomFunction
				{
					handlers.forEach(handler => {
						if (handler instanceof h2r_ProtocolHandler)
							handler.ExecuteCustomFunction(parsePacket)
					})
				}
				break;

			case 1133387750: // AckHeartBeat
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
			this.node?.sendPacketBinary(5301, 1418562193, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				code,
			}
			this.node?.sendPacket(5301, 1418562193, packet)
		}
	}
	
	// Protocol: ReqIntermediateCode
	ReqIntermediateCode(isBinary: boolean, itprId: number, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5301, 1644585100, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			this.node?.sendPacket(5301, 1644585100, packet)
		}
	}
	
	// Protocol: ReqRun
	ReqRun(isBinary: boolean, itprId: number, runType: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushStr(runType)
			this.node?.sendPacketBinary(5301, 923151823, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				runType,
			}
			this.node?.sendPacket(5301, 923151823, packet)
		}
	}
	
	// Protocol: ReqOneStep
	ReqOneStep(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 2884814738, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 2884814738, packet)
		}
	}
	
	// Protocol: ReqResumeRun
	ReqResumeRun(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 742173167, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 742173167, packet)
		}
	}
	
	// Protocol: ReqBreak
	ReqBreak(isBinary: boolean, itprId: number, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5301, 784411795, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
			}
			this.node?.sendPacket(5301, 784411795, packet)
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
			this.node?.sendPacketBinary(5301, 2487089996, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
			}
			this.node?.sendPacket(5301, 2487089996, packet)
		}
	}
	
	// Protocol: ReqStop
	ReqStop(isBinary: boolean, itprId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			this.node?.sendPacketBinary(5301, 1453251868, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
			}
			this.node?.sendPacket(5301, 1453251868, packet)
		}
	}
	
	// Protocol: ReqInput
	ReqInput(isBinary: boolean, itprId: number, vmId: number, eventName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(eventName)
			this.node?.sendPacketBinary(5301, 3140751413, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
			}
			this.node?.sendPacket(5301, 3140751413, packet)
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
			this.node?.sendPacketBinary(5301, 186222094, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				values,
			}
			this.node?.sendPacket(5301, 186222094, packet)
		}
	}
	
	// Protocol: ReqStepDebugType
	ReqStepDebugType(isBinary: boolean, stepDbgType: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			this.node?.sendPacketBinary(5301, 3084593987, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
			}
			this.node?.sendPacket(5301, 3084593987, packet)
		}
	}
	
	// Protocol: ReqDebugInfo
	ReqDebugInfo(isBinary: boolean, vmIds: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32Array(vmIds)
			this.node?.sendPacketBinary(5301, 2166551586, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
			}
			this.node?.sendPacket(5301, 2166551586, packet)
		}
	}
	
	// Protocol: ReqVariableInfo
	ReqVariableInfo(isBinary: boolean, itprId: number, vmId: number, varName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(varName)
			this.node?.sendPacketBinary(5301, 4127584998, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
			}
			this.node?.sendPacket(5301, 4127584998, packet)
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
			this.node?.sendPacketBinary(5301, 1626832220, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				value,
			}
			this.node?.sendPacket(5301, 1626832220, packet)
		}
	}
	
	// Protocol: ReqHeartBeat
	ReqHeartBeat(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5301, 2532286881, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5301, 2532286881, packet)
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
			this.node?.sendPacketBinary(5300, 1281093745, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				msg,
			}
			this.node?.sendPacket(5300, 1281093745, packet)
		}
	}
	
	// Protocol: AckUploadIntermediateCode
	AckUploadIntermediateCode(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 4005257575, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 4005257575, packet)
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
			this.node?.sendPacketBinary(5300, 1397310616, packet.buff, packet.offset)
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
			this.node?.sendPacket(5300, 1397310616, packet)
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
			this.node?.sendPacketBinary(5300, 977092268, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				totalCount,
				index,
				itprId,
				vmIds,
			}
			this.node?.sendPacket(5300, 977092268, packet)
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
			this.node?.sendPacketBinary(5300, 762776747, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				parentVmId,
				vmId,
				nodeFileName,
				nodeName,
			}
			this.node?.sendPacket(5300, 762776747, packet)
		}
	}
	
	// Protocol: RemoveInterpreter
	RemoveInterpreter(isBinary: boolean, vmId: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(vmId)
			this.node?.sendPacketBinary(5300, 940777945, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmId,
			}
			this.node?.sendPacket(5300, 940777945, packet)
		}
	}
	
	// Protocol: AckRun
	AckRun(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 4148808214, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 4148808214, packet)
		}
	}
	
	// Protocol: AckOneStep
	AckOneStep(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 3643391279, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 3643391279, packet)
		}
	}
	
	// Protocol: AckResumeRun
	AckResumeRun(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 1012496086, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 1012496086, packet)
		}
	}
	
	// Protocol: AckBreak
	AckBreak(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 2129545277, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 2129545277, packet)
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
			this.node?.sendPacketBinary(5300, 2045074648, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				enable,
				id,
				result,
			}
			this.node?.sendPacket(5300, 2045074648, packet)
		}
	}
	
	// Protocol: AckStop
	AckStop(isBinary: boolean, itprId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 114435221, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				result,
			}
			this.node?.sendPacket(5300, 114435221, packet)
		}
	}
	
	// Protocol: AckInput
	AckInput(isBinary: boolean, itprId: number, vmId: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 1658444570, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				result,
			}
			this.node?.sendPacket(5300, 1658444570, packet)
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
			this.node?.sendPacketBinary(5300, 1906481345, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				eventName,
				result,
			}
			this.node?.sendPacket(5300, 1906481345, packet)
		}
	}
	
	// Protocol: AckStepDebugType
	AckStepDebugType(isBinary: boolean, stepDbgType: number, result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(stepDbgType)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 4225702489, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				stepDbgType,
				result,
			}
			this.node?.sendPacket(5300, 4225702489, packet)
		}
	}
	
	// Protocol: AckDebugInfo
	AckDebugInfo(isBinary: boolean, vmIds: number[], result: number, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32Array(vmIds)
			packet.pushInt32(result)
			this.node?.sendPacketBinary(5300, 4276104084, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				vmIds,
				result,
			}
			this.node?.sendPacket(5300, 4276104084, packet)
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
			this.node?.sendPacketBinary(5300, 3660358812, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				result,
			}
			this.node?.sendPacket(5300, 3660358812, packet)
		}
	}
	
	// Protocol: SyncVMInstruction
	SyncVMInstruction(isBinary: boolean, itprId: number, vmId: number, indices: number[], ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushUint16Array(indices)
			this.node?.sendPacketBinary(5300, 4206107288, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				indices,
			}
			this.node?.sendPacket(5300, 4206107288, packet)
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
			this.node?.sendPacketBinary(5300, 3001685594, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				infoType,
				reg,
			}
			this.node?.sendPacket(5300, 3001685594, packet)
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
			this.node?.sendPacketBinary(5300, 3045798844, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				start,
				count,
				symbol,
			}
			this.node?.sendPacket(5300, 3045798844, packet)
		}
	}
	
	// Protocol: SyncVMOutput
	SyncVMOutput(isBinary: boolean, itprId: number, vmId: number, output: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(output)
			this.node?.sendPacketBinary(5300, 1348120458, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				output,
			}
			this.node?.sendPacket(5300, 1348120458, packet)
		}
	}
	
	// Protocol: SyncVMWidgets
	SyncVMWidgets(isBinary: boolean, itprId: number, vmId: number, widgetName: string, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushInt32(itprId)
			packet.pushInt32(vmId)
			packet.pushStr(widgetName)
			this.node?.sendPacketBinary(5300, 323195839, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				widgetName,
			}
			this.node?.sendPacket(5300, 323195839, packet)
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
			this.node?.sendPacketBinary(5300, 1209241191, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 1209241191, packet)
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
			this.node?.sendPacketBinary(5300, 3278867969, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 3278867969, packet)
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
			this.node?.sendPacketBinary(5300, 3822230413, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 3822230413, packet)
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
			this.node?.sendPacketBinary(5300, 2291689449, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				varName,
				startIdx,
				totalSize,
				array,
			}
			this.node?.sendPacket(5300, 2291689449, packet)
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
			this.node?.sendPacketBinary(5300, 3712761243, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				itprId,
				vmId,
				scopeName,
				timerId,
				time,
				actionType,
			}
			this.node?.sendPacket(5300, 3712761243, packet)
		}
	}
	
	// Protocol: ExecuteCustomFunction
	ExecuteCustomFunction(isBinary: boolean, fnName: string, args: Map<string,string[]>, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			packet.pushStr(fnName)
			packet.pushMapStrArray(args)
			this.node?.sendPacketBinary(5300, 638159302, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
				fnName,
				args,
			}
			this.node?.sendPacket(5300, 638159302, packet)
		}
	}
	
	// Protocol: AckHeartBeat
	AckHeartBeat(isBinary: boolean, ) {
		if (isBinary) { // binary send?
			let packet = new Network.Packet(512)
			this.node?.sendPacketBinary(5300, 1133387750, packet.buff, packet.offset)
		} else { // json string send?
			const packet = {
			}
			this.node?.sendPacket(5300, 1133387750, packet)
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

