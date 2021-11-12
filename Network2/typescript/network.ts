//
// 2021-01-01, jjuiddong
// network definition
//  - Packet
//  - Protocol Dispatcher
//  - Protocol Handler
//  - Protocol
//

import TypeVariant from "../common/variant"


export namespace Network {
    // variant type (c++)
    const VT_EMPTY: number = 0;
    const VT_I2: number = 2;
    const VT_I4: number = 3;
    const VT_R4: number = 4;
    const VT_R8: number = 5;
    const VT_BSTR: number = 8;
    const VT_BOOL: number = 11;
    const VT_I1: number = 16;
    const VT_UI1: number = 17;
    const VT_UI2: number = 18;
    const VT_UI4: number = 19;
    const VT_I8: number = 20;
    const VT_UI8: number = 21;
    const VT_INT: number = 22;
    const VT_UINT: number = 23;
    const VT_ARRAY: number = 0x2000;
    const VT_BYREF: number = 0x4000; // for array type


    //------------------------------------------------------------------------------
    // packet
    //------------------------------------------------------------------------------
    export class Packet {
        buff: ArrayBuffer | null = null
        dv: DataView | null = null
        offset: number = 0
        byteLength: number = 0 // buffer byte length

        // size: buffer size (byte unit)
        constructor(size: number = 0) {
            this.buff = size == 0 ? null : new ArrayBuffer(size)
            if (this.buff)
                this.dv = new DataView(this.buff)
            this.offset = 0 // read/write offset
            this.byteLength = size;
        }
        //--------------------------------------------------------------------------
        // initialize read/write
        init() {
            this.offset = 0
        }
        //--------------------------------------------------------------------------
        // initialize with ArrayBuffer
        // message: websocket receive message buffer
        // offset: buffer offset (packet header size)
        initWithArrayBuffer(message: ArrayBuffer, offset: number = 0) {
            this.dv = new DataView(message)
            this.buff = message
            this.offset = offset
            this.byteLength = message.byteLength
        }
        //--------------------------------------------------------------------------
        // 4byte alignment
        calc4ByteAlignment() {
            if (this.offset % 4 != 0)
                this.offset = this.offset + (4 - (this.offset % 4))
        }
        //--------------------------------------------------------------------------
        // boolean to arraybuffer
        pushBool(val: boolean) {
            if (this.offset + 1 > this.byteLength)
                return

            // if (!this.buff)
            //     return
            // let view = new Uint8Array(this.buff, this.offset)
            // view[0] = val ? 0 : 1
            this.dv?.setUint8(this.offset, val ? 1 : 0)
            this.offset += 1
        }
        //--------------------------------------------------------------------------
        // string to arraybuffer
        pushStr(str: string) {
            // let view = new Uint8Array(this.buff, this.offset)
            // let ec = new TextEncoder().encode(str)
            // for (let i = 0; i < ec.length; ++i) {
            //     view[i] = ec[i]
            // }
            // view[ec.length] = 0
            // this.offset += ec.length + 1 // add null data

            let ec = new TextEncoder().encode(str)
            for (let i = 0; (i < ec.length) && (this.offset < this.byteLength); ++i) {
                this.dv?.setUint8(this.offset, ec[i])
                this.offset++
            }
            if (this.offset < this.byteLength) {
                this.dv?.setUint8(this.offset, 0)
                this.offset++
            }
        }
        //--------------------------------------------------------------------------
        // string array to arraybuffer
        pushStrArray(strArray: string[]) {
            this.pushInt(strArray.length) // array size
            // let dst = 0
            // let view = new Uint8Array(this.buff, this.offset)
            // for (let i = 0; i < strArray.length; ++i) {
            //     let str = strArray[i]
            //     let ec = new TextEncoder().encode(str)
            //     for (let i = 0; i < ec.length; ++i) {
            //         view[dst++] = ec[i]
            //     }
            //     view[dst++] = 0
            // }
            // this.offset += dst

            for (let i = 0; i < strArray.length; ++i) {
                const str = strArray[i]
                let ec = new TextEncoder().encode(str)
                for (let k = 0; (k < ec.length) && (this.offset < this.byteLength); ++k) {
                    this.dv?.setUint8(this.offset++, ec[k])
                }
                if (this.offset < this.byteLength)
                    this.dv?.setUint8(this.offset++, 0)
            }
        }
        //--------------------------------------------------------------------------
        // number to arraybuffer
        pushByte(num: number) {
            if (this.offset >= this.byteLength)
                return
            // let view = new Uint8Array(this.buff, this.offset)
            // view[0] = num
            // this.offset += 1
            this.dv?.setUint8(this.offset, num)
            this.offset += 1
        }
        //--------------------------------------------------------------------------
        // number to arraybuffer
        pushInt(num: number) {
            this.calc4ByteAlignment()
            if (this.offset + 4 > this.byteLength)
                return
            // let view = new Int32Array(this.buff, this.offset)
            // view[0] = num
            // this.offset += 4
            this.dv?.setInt32(this.offset, num, true)
            this.offset += 4
        }
        //--------------------------------------------------------------------------
        // number to arraybuffer
        pushFloat(num: number) {
            this.calc4ByteAlignment()
            if (this.offset + 4 > this.byteLength)
                return
            // let view = new Float32Array(this.buff, this.offset)
            // view[0] = num
            // this.offset += 4
            this.dv?.setFloat32(this.offset, num, true)
            this.offset += 4
        }
        pushUint8(num: number) {
            if (this.offset + 1 > this.byteLength)
                return

            // let view = new Uint8Array(this.buff, this.offset)
            // view[0] = (num == 0) ? 0 : 1
            // this.offset += 1
            this.dv?.setUint8(this.offset, num)
            this.offset += 1
        }
        pushInt8(num: number) {
            if (this.offset + 1 > this.byteLength)
                return
            this.dv?.setInt8(this.offset, num)
            this.offset += 1
        }
        pushUint16(num: number) {
            if (this.offset + 2 > this.byteLength)
                return
            this.dv?.setUint16(this.offset, num, true)
            this.offset += 2
        }
        pushInt16(num: number) {
            if (this.offset + 2 > this.byteLength)
                return
            this.dv?.setInt16(this.offset, num, true)
            this.offset += 2
        }
        pushUint32(num: number) {
            this.calc4ByteAlignment()
            if (this.offset + 4 > this.byteLength)
                return
            this.dv?.setUint32(this.offset, num, true)
            this.offset += 4
        }
        pushInt32(num: number) {
            this.calc4ByteAlignment()
            if (this.offset + 4 > this.byteLength)
                return
            this.dv?.setInt32(this.offset, num, true)
            this.offset += 4
        }
        pushUint64(num: bigint) {
            this.calc4ByteAlignment()
            if (this.offset + 8 > this.byteLength)
                return
            this.dv?.setBigUint64(this.offset, num, true)
            this.offset += 8
        }
        pushInt64(num: bigint) {
            this.calc4ByteAlignment()
            if (this.offset + 8 > this.byteLength)
                return
            this.dv?.setBigInt64(this.offset, num, true)
            this.offset += 8
        }
        pushFloat32(num: number) {
            this.calc4ByteAlignment()
            if (this.offset + 4 > this.byteLength)
                return
            this.dv?.setFloat32(this.offset, num, true)
            this.offset += 4
        }
        pushFloat64(num: number) {
            this.calc4ByteAlignment()
            if (this.offset + 8 > this.byteLength)
                return
            this.dv?.setFloat64(this.offset, num, true)
            this.offset += 8
        }
        pushBoolArray(ar: boolean[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 1) < this.byteLength); ++i) {
                this.dv?.setUint8(this.offset, ar[i] ? 1 : 0)
                this.offset += 1
            }
        }
        pushUint8Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 1) < this.byteLength); ++i) {
                this.dv?.setUint8(this.offset, ar[i])
                this.offset += 1
            }
        }
        pushInt8Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 1) < this.byteLength); ++i) {
                this.dv?.setInt8(this.offset, ar[i])
                this.offset += 1
            }
        }
        pushUint16Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 2) < this.byteLength); ++i) {
                this.dv?.setUint16(this.offset, ar[i], true)
                this.offset += 2
            }
        }
        pushInt16Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 2) < this.byteLength); ++i) {
                this.dv?.setInt16(this.offset, ar[i], true)
                this.offset += 2
            }
        }
        pushUint32Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 4) < this.byteLength); ++i) {
                this.dv?.setUint32(this.offset, ar[i], true)
                this.offset += 4
            }
        }
        pushInt32Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 4) < this.byteLength); ++i) {
                this.dv?.setInt32(this.offset, ar[i], true)
                this.offset += 4
            }
        }
        pushFloat32Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 4) < this.byteLength); ++i) {
                this.dv?.setFloat32(this.offset, ar[i], true)
                this.offset += 4
            }
        }
        pushFloat64Array(ar: number[]) {
            this.pushUint32(ar.length)
            for (let i = 0; (i < ar.length) && ((this.offset + 8) < this.byteLength); ++i) {
                this.dv?.setFloat64(this.offset, ar[i], true)
                this.offset += 8
            }
        }

        //--------------------------------------------------------------------------
        // getBoolean
        getBool(): boolean {
            if (!this.dv)
                return false
            const v = this.dv.getUint8(this.offset)
            this.offset += 1
            return v != 0
        }
        //--------------------------------------------------------------------------
        // getInt8
        getInt8(): number {
            if (!this.dv)
                return 0
            const v = this.dv.getInt8(this.offset)
            this.offset += 1
            return v
        }
        //--------------------------------------------------------------------------
        // getUint8
        getUint8(): number {
            if (!this.dv)
                return 0
            const v = this.dv.getUint8(this.offset)
            this.offset += 1
            return v
        }
        //--------------------------------------------------------------------------
        // getInt16
        getInt16(): number {
            if (!this.dv)
                return 0
            const v = this.dv.getInt16(this.offset, true)
            this.offset += 2
            return v
        }
        //--------------------------------------------------------------------------
        // getUint16
        getUint16(): number {
            if (!this.dv)
                return 0
            const v = this.dv.getUint16(this.offset, true)
            this.offset += 2
            return v
        }
        //--------------------------------------------------------------------------
        // getInt32
        getInt32(): number {
            if (!this.dv)
                return 0
            this.calc4ByteAlignment()
            const v = this.dv.getInt32(this.offset, true)
            this.offset += 4
            return v
        }
        //--------------------------------------------------------------------------
        // getUint32
        getUint32(): number {
            if (!this.dv)
                return 0
            this.calc4ByteAlignment()
            const v = this.dv.getUint32(this.offset, true)
            this.offset += 4
            return v
        }
        //--------------------------------------------------------------------------
        // getInt64
        getInt64(): bigint {
            if (!this.dv)
                return BigInt(0)
            this.calc4ByteAlignment()
            const v = this.dv.getBigInt64(this.offset, true)
            this.offset += 8
            return v
        }
        //--------------------------------------------------------------------------
        // getUint64
        getUint64(): bigint {
            if (!this.dv)
                return BigInt(0)
            this.calc4ByteAlignment()
            const v = this.dv.getBigUint64(this.offset, true)
            this.offset += 8
            return v
        }        
        //--------------------------------------------------------------------------
        // getFloat32
        getFloat32(): number {
            if (!this.dv)
                return 0
            this.calc4ByteAlignment()
            const v = this.dv.getFloat32(this.offset, true)
            this.offset += 4
            return v
        }
        //--------------------------------------------------------------------------
        // getFloat64
        getFloat64(): number {
            if (!this.dv)
                return 0
            this.calc4ByteAlignment()
            const v = this.dv.getFloat64(this.offset, true)
            this.offset += 8
            return v
        }
        //--------------------------------------------------------------------------
        // get array buffer
        getArray<T>(
            Alloc: { (buff: ArrayBuffer, offset: number, length: number): T }
        ): T | null {
            if (!this.buff)
                return null
            const length = this.getUint32()
            if (length == 0)
                return null
            let out = Alloc(this.buff, this.offset, length)
            return out
        }
        getInt8Array(): Int8Array | null {
            return this.getArray<Int8Array>((buff, offset, length) => {
                let ar = new Int8Array(buff, offset, length)
                this.offset += length
                return ar
            })
        }
        getUint8Array(): Uint8Array | null {
            return this.getArray<Uint8Array>((buff, offset, length) => {
                let ar = new Uint8Array(buff, offset, length)
                this.offset += length
                return ar;
            })
        }
        getInt16Array(): Int16Array | null {
            return this.getArray<Int16Array>((buff, offset, length) => {
                let ar = new Int16Array(buff, offset, length)
                this.offset += length * 2
                return ar
            })
        }
        getUint16Array(): Uint16Array | null {
            return this.getArray<Uint16Array>((buff, offset, length) => {
                let ar = new Uint16Array(buff, offset, length)
                this.offset += length * 2
                return ar
            })
        }
        getInt32Array(): Int32Array | null {
            return this.getArray<Int32Array>((buff, offset, length) => {
                let ar = new Int32Array(buff, offset, length)
                this.offset += length * 4
                return ar
            })
        }
        getUint32Array(): Uint32Array | null {
            return this.getArray<Uint32Array>((buff, offset, length) => {
                let ar = new Uint32Array(buff, offset, length)
                this.offset += length * 4
                return ar
            })
        }
        getFloat32Array(): Float32Array | null {
            return this.getArray<Float32Array>((buff, offset, length) => {
                let ar = new Float32Array(buff, offset, length)
                this.offset += length * 4
                return ar
            })
        }
        getFloat64Array(): Float64Array | null {
            return this.getArray<Float64Array>((buff, offset, length) => {
                let ar = new Float64Array(buff, offset, length)
                this.offset += length * 8
                return ar
            })
        }
        //--------------------------------------------------------------------------
        // getStr
        getStr(): string {
            if (!this.dv)
                return ''
            let str = []
            // let c = this.dv.getUint8(this.offset++)
            // str.push(c)
            // while ((c != 0) && (this.offset < this.byteLength)) {
            //     c = this.dv.getUint8(this.offset++)
            //     str.push(c)
            // }
            while (this.offset < this.byteLength) {
                const c = this.dv.getUint8(this.offset++)
                if (c == 0)
                    break
                str.push(c)
            }
            if (str.length == 0) return ''
            return new TextDecoder().decode(new Uint8Array(str))
        }
        //--------------------------------------------------------------------------
        // get String array
        getStrArray(): string[] {
            if (!this.dv || !this.buff)
                return []
            const size = this.getUint32()
            let ar: string[] = []
            for (let i = 0; i < size; ++i)
                ar.push(this.getStr())
            return ar
        }
        //--------------------------------------------------------------------------
        // get TypeVariant
        getTypeVariant(): TypeVariant {
            let out = new TypeVariant()
            out.vt = this.getUint16();
            switch (out.vt) {
                case VT_EMPTY:
                    break; // nothing
                case VT_I2:
                    out.value = this.getInt16();
                    break;
                case VT_I4:
                    out.value = this.getInt32();
                    break;
                case VT_R4:
                    out.value = this.getFloat32();
                    break;
                case VT_R8:
                    out.value = this.getFloat64();
                    break;
                case VT_BSTR:
                    out.value = this.getStr();
                    break;
                case VT_BOOL:
                    out.value = this.getUint8() > 0 ? true : false;
                    break;
                case VT_I1:
                    out.value = this.getInt8();
                    break;
                case VT_UI1:
                    out.value = this.getUint8();
                    break;
                case VT_UI2:
                    out.value = this.getUint16();
                    break;
                case VT_UI4:
                    out.value = this.getUint32();
                    break;
                case VT_I8:
                    out.value = this.getInt32();
                    break;
                case VT_UI8:
                    out.value = this.getUint32();
                    break;
                case VT_INT:
                    out.value = this.getInt32();
                    break;
                case VT_UINT:
                    out.value = this.getUint32();
                    break;
                default:
                    if (out.vt & VT_BYREF) {
                        out.value = this.getUint32();
                    } else {
                        console.log(`error parse Network.Packet-TypeVariant ${out.vt}`);
                    }
                    break;
            }
            return out
        }
        //--------------------------------------------------------------------------
        // get TypeVariant Array
        getTypeVariantArray(size: number): TypeVariant[] {
            let ar: TypeVariant[] = []
            for (let i = 0; i < size; ++i)
                ar.push(this.getTypeVariant())
            return ar
        }
        //--------------------------------------------------------------------------
        // get TypeVariant Vector
        getTypeVariantVector(): TypeVariant[] {
            const size = this.getUint32()
            return this.getTypeVariantArray(size)
        }
    }

    //------------------------------------------------------------------------------
    // Protocol Dispatcher
    //------------------------------------------------------------------------------
    export class Dispatcher {
        id: number // protocol id
        constructor(id: number) {
            this.id = id
        }
        // dispatch packet
        dispatch(ws: WebSocket, packet: Packet, handlers: Handler[]) { }
    }

    //------------------------------------------------------------------------------
    // Protocol Handler
    //------------------------------------------------------------------------------
    export class Handler {
        constructor() {
        }
    }

    //------------------------------------------------------------------------------
    // Protocol Maker & Sender
    //------------------------------------------------------------------------------
    export class Protocol {
        ws: WebSocket | null = null
        constructor() {
        }
    }

    //------------------------------------------------------------------------------
    // websocket client
    //------------------------------------------------------------------------------
    export class WebSockClient {
        ws: WebSocket
        dispatchers: Dispatcher[] = []
        handlers: Handler[] = []
        isConnect: boolean = false
        isShowError: boolean = true

        constructor(url: string) {
            let self = this
            this.ws = new WebSocket(url)
            this.ws.onopen = (evt) => {
                self.isConnect = true
                self.onopen()
            }

            this.ws.onmessage = (evt) => {
                let fileReader = new FileReader()
                fileReader.onload = (event) => {
                    if (!(event.target?.result instanceof ArrayBuffer))
                        return
                    let packet = new Packet()
                    packet.initWithArrayBuffer(event.target.result)
                    const protocolId = packet.getUint32();
                    const packetId = packet.getUint32();
                    let dispatcher: Dispatcher | undefined =
                        self.dispatchers.find(p => p.id == protocolId)
                    if (!dispatcher) {
                        if (protocolId == 1000 && packetId == 1281093745)
                            return // ignore welcome packet
                        if (this.isShowError)
                            console.log(`error, not found dispatcher ${protocolId}, ${packetId}`)
                        return
                    }
                    dispatcher.dispatch(self.ws, packet, self.handlers)
                }
                fileReader.readAsArrayBuffer(evt.data)
            }

            this.ws.onclose = (evt) => {
                self.isConnect = false
                self.onclose()
            }

            this.ws.onerror = (evt) => {
                self.isConnect = false
                self.onerror()
            }
        }
        //--------------------------------------------------------------------------
        // on open, close
        onopen = () => { }
        onclose = () => { }
        onerror = () => { }
        //--------------------------------------------------------------------------
        // add dispatcher
        addDispatcher(dispatcher: Dispatcher) {
            if (this.dispatchers.find(d => d.id == dispatcher.id)) return // already exist
            this.dispatchers.push(dispatcher)
        }
        //--------------------------------------------------------------------------
        // remove dispatcher
        // id: dispatcher id
        removeDispatcher(id: number) {
            this.dispatchers = this.dispatchers.filter(d => d.id != id)
        }
        //--------------------------------------------------------------------------
        // add handler
        addHandler(handler: Handler) {
            if (this.handlers.find(h => h == handler)) return // already exist
            this.handlers.push(handler)
        }
        //--------------------------------------------------------------------------
        // remove handler
        removeHandler(handler: Handler) {
            this.handlers = this.handlers.filter(p => p != handler)
        }
        //--------------------------------------------------------------------------
        // register protocol
        registerProtocol(protocol: Protocol) {
            protocol.ws = this.ws
        }
        //--------------------------------------------------------------------------
        // clear
        clear() {
            this.ws.close();
            this.dispatchers = []
            this.handlers = []
        }
    }

    //------------------------------------------------------------------------------
    // send packet (send json string)
    // packet format = | packet header (16 byte) | packet data |
    // packet length = header size (16 byte) + data size (? byte)
    // packet header = | protocol id (4) | packet id(4) | packet length (4) | option (4) |
    // packet: json object
    export function sendPacket(ws: WebSocket,
        protocolId: number, packetId: number, packet: any
    ) {
        const headerSize: number = 16
        let packetData: string = JSON.stringify(packet);
        let length: number = headerSize + packetData.length;

        // packet frame buffer
        let buff = new ArrayBuffer(length);

        // 16 bytes header buffer setting
        let header = new Uint32Array(buff, 0, 4);
        header[0] = protocolId; // protocol id
        header[1] = packetId; // packet id
        header[2] = length; // packet length
        header[3] = 0; // json string flag

        // json string to arraybuffer
        let view = new Uint8Array(buff, headerSize);
        for (let i = 0, strLen = packetData.length; i < strLen; ++i) {
            view[i] = packetData.charCodeAt(i);
        }

        ws.send(buff);
    }

    //--------------------------------------------------------------------------------
    // send binary packet
    // packet: array buffer
    // packetId: number
    // packet format = | packet header (16 byte) | packet data |
    // packet length = header size (16 byte) + data size (? byte)
    // packet header = | protocol id (4) | packet id(4) | packet length (4) | option (4) |
    export function sendPacketBinary(ws: WebSocket,
        protocolId: number, packetId: number, packet: ArrayBuffer | null, packetLen: number
    ) {
        if (!packet) return
        let length = 16 + packetLen;
        length += length % 4 == 0 ? 0 : 4 - (length % 4); // 4byte alignment

        let buff = new ArrayBuffer(length);
        let header = new Uint32Array(buff);
        header[0] = protocolId; // protocol id (tricky code, mimic webserver)
        header[1] = packetId; // packet id
        header[2] = buff.byteLength; // packet length
        header[3] = 0x01; // binary flag on

        let src = new Uint8Array(packet);
        let dst = new Uint8Array(buff, 16);
        for (let i = 0; i < src.length; ++i) {
            dst[i] = src[i]
        }
        ws.send(buff);
    }

    //--------------------------------------------------------------------------------
    // send packet with json object
    export function sendPacket2(ws: WebSocket, packet: Packet, jsonObj: any) {
        if (packet.buff)
            ws.send(packet.buff);
    }

}

