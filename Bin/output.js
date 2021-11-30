//------------------------------------------------------------------------
// protocol handler javascript code
// Author:  ProtocolGenerator (by jjuiddong)
//------------------------------------------------------------------------

const VT_EMPTY = 0;
const VT_I2 = 2;
const VT_I4 = 3;
const VT_R4 = 4;
const VT_R8 = 5;
const VT_BSTR = 8;
const VT_BOOL = 11;
const VT_I1 = 16;
const VT_UI1 = 17;
const VT_UI2 = 18;
const VT_UI4 = 19;
const VT_I8 = 20;
const VT_UI8 = 21;
const VT_INT = 22;
const VT_UINT = 23;
const VT_ARRAY = 0x2000;
const VT_BYREF = 0x4000; // for array type


class TypeVariant {
  constructor() {
    this.vt = VT_EMPTY; // value type
    this.value = 0; // any value
  }

  //------------------------------------------------------------------------------
  // parse variant binary data in dataview
  // return read size
  fromDataView(dataView, offset) {
    let vt = dataView.getUint16(offset, true);
    let length = 2; // value type 2 bytes
    offset += 2;

    // 4bytes alignments
    switch (vt) {
      case VT_I4:
      case VT_R4:
      case VT_R8:
      case VT_UI4:
      case VT_I8:
      case VT_UI8:
      case VT_INT:
      case VT_UINT:
        if (offset % 4 != 0) {
          const dummy = 4 - (offset % 4); // 4 bytes alignments
          offset += dummy;
          length += dummy;
        }
        break;
    }

    switch (vt) {
      case VT_EMPTY:
        break; // nothing
      case VT_I2:
        this.value = dataView.getInt16(offset, true);
        length += 2;
        break;
      case VT_I4:
        this.value = dataView.getInt32(offset, true);
        length += 4;
        break;
      case VT_R4:
        this.value = dataView.getFloat32(offset, true);
        length += 4;
        break;
      case VT_R8:
        this.value = dataView.getFloat64(offset, true);
        length += 8;
        break;
      case VT_BSTR:
        this.value = this.dv2str(dataView, offset);
        length += this.value.length + 1;
        break;
      case VT_BOOL:
        this.value = dataView.getUint8(offset, true) > 0 ? true : false;
        length += 1;
        break;
      case VT_I1:
        this.value = dataView.getInt8(offset, true);
        length += 1;
        break;
      case VT_UI1:
        this.value = dataView.getUint8(offset, true);
        length += 1;
        break;
      case VT_UI2:
        this.value = dataView.getUint16(offset, true);
        length += 2;
        break;
      case VT_UI4:
        this.value = dataView.getUint32(offset, true);
        length += 4;
        break;
      case VT_I8:
        this.value = dataView.getInt64(offset, true);
        length += 8;
        break;
      case VT_UI8:
        this.value = dataView.getUint64(offset, true);
        length += 8;
        break;
      case VT_INT:
        this.value = dataView.getInt32(offset, true);
        length += 4;
        break;
      case VT_UINT:
        this.value = dataView.getUint32(offset, true);
        length += 4;
        break;
      default:
        console.log(`error parse TypeVariant ${vt}`);
        break;
    }
    return length;
  }

  //--------------------------------------------------------------------------------
  // parse string in dataView
  // return string
  // dataView: DataView
  // offset: offset number
  dv2str(dataView, offset) {
    let str = [];
    let i = offset;
    let c = dataView.getUint8(i);
    while (c != 0) {
      str.push(c);
      c = dataView.getUint8(++i);
    }
    if (str.length == 0) return "";
    return new TextDecoder().decode(new Uint8Array(str));
  }
}

class Packet {
  // size: buffer size (byte unit)
  constructor(size = 0) {
    this.buff = size == 0 ? null : new ArrayBuffer(size);
    this.dv = this.buff ? new DataView(this.buff) : null;
    this.offset = 0; // read/write offset
    this.byteLength = size;
  }

  //--------------------------------------------------------------------------------
  // initialize read/write
  init() {
    this.offset = 0;
  }

  //--------------------------------------------------------------------------------
  // initialize with ArrayBuffer
  // message: websocket receive message buffer
  // offset: buffer offset (packet header size)
  initWithArrayBuffer(message, offset = 0) {
    this.dv = new DataView(message);
    this.buff = message;
    this.offset = offset;
    this.byteLength = message.byteLength;
  }

  //--------------------------------------------------------------------------------
  // 4byte alignment
  calc4ByteAlignment() {
    if (this.offset % 4 != 0)
      this.offset = this.offset + (4 - (this.offset % 4));
  }

  //--------------------------------------------------------------------------------
  // boolean to arraybuffer
  pushBool(val) {
    if (!this.dv) return;
    if (this.offset + 1 > this.byteLength) return;
    this.dv.setUint8(this.offset, val ? 1 : 0);
    this.offset += 1;
  }

  //--------------------------------------------------------------------------------
  // string to arraybuffer
  pushStr(str) {
    if (!this.dv) return;
    let ec = new TextEncoder().encode(str);
    for (let i = 0; i < ec.length && this.offset < this.byteLength; ++i) {
      this.dv.setUint8(this.offset, ec[i]);
      this.offset++;
    }
    if (this.offset < this.byteLength) {
      this.dv.setUint8(this.offset, 0);
      this.offset++;
    }
  }

  //--------------------------------------------------------------------------------
  // string array to arraybuffer
  pushStrArray(strArray) {
    if (!this.dv) return;
    this.pushInt(strArray.length); // array size
    for (let i = 0; i < strArray.length; ++i) {
      const str = strArray[i];
      let ec = new TextEncoder().encode(str);
      for (let k = 0; k < ec.length && this.offset < this.byteLength; ++k) {
        this.dv.setUint8(this.offset++, ec[k]);
      }
      if (this.offset < this.byteLength) this.dv.setUint8(this.offset++, 0);
    }
  }

  //--------------------------------------------------------------------------------
  // number to arraybuffer
  pushByte(num) {
    if (!this.dv) return;
    if (this.offset >= this.byteLength) return;
    this.dv.setUint8(this.offset, num);
    this.offset += 1;
  }

  //--------------------------------------------------------------------------------
  // number to arraybuffer
  pushInt(num) {
    if (!this.dv) return;
    if (this.offset + 4 > this.byteLength) return;
    this.dv.setInt32(this.offset, num, true);
    this.offset += 4;
  }

  //--------------------------------------------------------------------------------
  // number to arraybuffer
  pushFloat(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 4 > this.byteLength) return;
    this.dv.setFloat32(this.offset, num, true);
    this.offset += 4;
  }
  pushUint8(num) {
    if (!this.dv) return;
    if (this.offset + 1 > this.byteLength) return;
    this.dv.setUint8(this.offset, num);
    this.offset += 1;
  }
  pushInt8(num) {
    if (!this.dv) return;
    if (this.offset + 1 > this.byteLength) return;
    this.dv.setInt8(this.offset, num);
    this.offset += 1;
  }
  pushUint16(num) {
    if (!this.dv) return;
    if (this.offset + 2 > this.byteLength) return;
    this.dv.setUint16(this.offset, num, true);
    this.offset += 2;
  }
  pushInt16(num) {
    if (!this.dv) return;
    if (this.offset + 2 > this.byteLength) return;
    this.dv.setInt16(this.offset, num, true);
    this.offset += 2;
  }
  pushUint32(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 4 > this.byteLength) return;
    this.dv.setUint32(this.offset, num, true);
    this.offset += 4;
  }
  pushInt32(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 4 > this.byteLength) return;
    this.dv.setInt32(this.offset, num, true);
    this.offset += 4;
  }
  pushUint64(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 8 > this.byteLength) return;
    this.dv.setBigUint64(this.offset, num, true);
    this.offset += 8;
  }
  pushInt64(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 8 > this.byteLength) return;
    this.dv.setBigInt64(this.offset, num, true);
    this.offset += 8;
  }
  pushFloat32(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 4 > this.byteLength) return;
    this.dv.setFloat32(this.offset, num, true);
    this.offset += 4;
  }
  pushFloat64(num) {
    if (!this.dv) return;
    this.calc4ByteAlignment();
    if (this.offset + 8 > this.byteLength) return;
    this.dv.setFloat64(this.offset, num, true);
    this.offset += 8;
  }
  pushBoolArray(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 1 < this.byteLength; ++i) {
      this.dv.setUint8(this.offset, ar[i] ? 1 : 0);
      this.offset += 1;
    }
  }
  pushUint8Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 1 < this.byteLength; ++i) {
      this.dv.setUint8(this.offset, ar[i]);
      this.offset += 1;
    }
  }
  pushInt8Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 1 < this.byteLength; ++i) {
      this.dv.setInt8(this.offset, ar[i]);
      this.offset += 1;
    }
  }
  pushUint16Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 2 < this.byteLength; ++i) {
      this.dv.setUint16(this.offset, ar[i], true);
      this.offset += 2;
    }
  }
  pushInt16Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 2 < this.byteLength; ++i) {
      this.dv.setInt16(this.offset, ar[i], true);
      this.offset += 2;
    }
  }
  pushUint32Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 4 < this.byteLength; ++i) {
      this.dv.setUint32(this.offset, ar[i], true);
      this.offset += 4;
    }
  }
  pushInt32Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 4 < this.byteLength; ++i) {
      this.dv.setInt32(this.offset, ar[i], true);
      this.offset += 4;
    }
  }
  pushFloat32Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 4 < this.byteLength; ++i) {
      this.dv.setFloat32(this.offset, ar[i], true);
      this.offset += 4;
    }
  }
  pushFloat64Array(ar) {
    if (!this.dv) return;
    this.pushUint32(ar.length);
    for (let i = 0; i < ar.length && this.offset + 8 < this.byteLength; ++i) {
      this.dv.setFloat64(this.offset, ar[i], true);
      this.offset += 8;
    }
  }

  //--------------------------------------------------------------------------------
  // parse string in dataView
  // return string
  // dataView: DataView
  // offset: offset number
  dv2str(dataView) {
    let str = [];
    let i = this.offset;
    let c = dataView.getUint8(i);
    while (c != 0) {
      str.push(c);
      c = dataView.getUint8(++i);
    }
    if (str.length == 0) return "";
    return new TextDecoder().decode(new Uint8Array(str));
  }

  //--------------------------------------------------------------------------
  // getBoolean
  getBool() {
    if (!this.dv) return false;
    const v = this.dv.getUint8(this.offset);
    this.offset += 1;
    return v != 0;
  }
  //--------------------------------------------------------------------------------
  // getInt8
  getInt8() {
    if (this.dv) {
      const v = this.dv.getInt8(this.offset);
      this.offset += 1;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------------
  // getUint8
  getUint8() {
    if (this.dv) {
      const v = this.dv.getUint8(this.offset);
      this.offset += 1;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------
  // getInt16
  getInt16() {
    if (!this.dv) return 0;
    const v = this.dv.getInt16(this.offset, true);
    this.offset += 2;
    return v;
  }
  //--------------------------------------------------------------------------
  // getUint16
  getUint16() {
    if (!this.dv) return 0;
    const v = this.dv.getUint16(this.offset, true);
    this.offset += 2;
    return v;
  }
  //--------------------------------------------------------------------------------
  // getInt32
  getInt32() {
    if (this.dv) {
      this.calc4ByteAlignment();
      const v = this.dv.getInt32(this.offset, true);
      this.offset += 4;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------------
  // getUint32
  getUint32() {
    if (this.dv) {
      this.calc4ByteAlignment();
      const v = this.dv.getUint32(this.offset, true);
      this.offset += 4;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------------
  // getInt64
  getInt64() {
    if (this.dv) {
      this.calc4ByteAlignment();
      const v = this.dv.getBigInt64(this.offset, true);
      this.offset += 8;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------------
  // getUint64
  getUint64() {
    if (this.dv) {
      this.calc4ByteAlignment();
      const v = this.dv.getBigUint64(this.offset, true);
      this.offset += 8;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------------
  // getFloat32
  getFloat32() {
    if (this.dv) {
      this.calc4ByteAlignment();
      const v = this.dv.getFloat32(this.offset, true);
      this.offset += 4;
      return v;
    }
    return 0;
  }

  //--------------------------------------------------------------------------------
  // getFloat64
  getFloat64() {
    if (this.dv) {
      this.calc4ByteAlignment();
      const v = this.dv.getFloat64(this.offset, true);
      this.offset += 8;
      return v;
    }
    return 0;
  }
  //--------------------------------------------------------------------------
  // get array buffer
  getArray(Alloc) {
    if (!this.buff) return null;
    const length = this.getUint32();
    if (length == 0) return null;
    let out = Alloc(this.buff, this.offset, length);
    return out;
  }
  getInt8Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Int8Array(buff, offset, length);
      this.offset += length;
      return ar;
    });
  }
  getUint8Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Uint8Array(buff, offset, length);
      this.offset += length;
      return ar;
    });
  }
  getInt16Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Int16Array(buff, offset, length);
      this.offset += length * 2;
      return ar;
    });
  }
  getUint16Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Uint16Array(buff, offset, length);
      this.offset += length * 2;
      return ar;
    });
  }
  getInt32Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Int32Array(buff, offset, length);
      this.offset += length * 4;
      return ar;
    });
  }
  getUint32Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Uint32Array(buff, offset, length);
      this.offset += length * 4;
      return ar;
    });
  }
  // getInt64Array() {
  //     return this.getArray((buff, offset, length) => {
  //         let ar = new BigInt64Array(buff, offset, length)
  //         this.offset += length * 8
  //         return ar
  //     })
  // }
  // getUint64Array() {
  //     return this.getArray((buff, offset, length) => {
  //         let ar = new BigUint64Array(buff, offset, length)
  //         this.offset += length * 8
  //         return ar
  //     })
  // }
  getFloat32Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Float32Array(buff, offset, length);
      this.offset += length * 4;
      return ar;
    });
  }
  getFloat64Array() {
    return this.getArray((buff, offset, length) => {
      let ar = new Float64Array(buff, offset, length);
      this.offset += length * 8;
      return ar;
    });
  }
  //--------------------------------------------------------------------------------
  // getStr
  getStr() {
    if (this.dv && this.buff) {
      let str = [];
      while (this.offset < this.byteLength) {
        const c = this.dv.getUint8(this.offset++);
        if (c == 0) break;
        str.push(c);
      }
      if (str.length == 0) return "";
      return new TextDecoder().decode(new Uint8Array(str));
    }
    return "";
  }
  //--------------------------------------------------------------------------
  // get string array
  getStrArray() {
    if (!this.dv || !this.buff) return [];
    const size = this.getUint32();
    let ar = [];
    for (let i = 0; i < size; ++i) ar.push(this.getStr());
    return ar;
  }
  //--------------------------------------------------------------------------
  // get TypeVariant
  getTypeVariant() {
    let out = new TypeVariant();
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
    return out;
  }
  //--------------------------------------------------------------------------
  // get TypeVariant Array
  getTypeVariantArray(size) {
    let ar = [];
    for (let i = 0; i < size; ++i) ar.push(this.getTypeVariant());
    return ar;
  }
  //--------------------------------------------------------------------------
  // get TypeVariant Vector
  getTypeVariantVector() {
    const size = this.getUint32();
    return this.getTypeVariantArray(size);
  }
}

//------------------------------------------------------------------------------
// common receive packet handler
function onReceive(wss, ws, protocolId, packetId, packet) {
  postMessage({
    type: "packet",
    protocolId,
    packetId,
    packet,
  });
}

//------------------------------------------------------------------------------
// send packet (send json string)
// ws: websocket
// packet: json object
// protocolId: number
// packetId: number
// packet format = | packet header (16 byte) | packet data |
// packet length = header size (16 byte) + data size (? byte)
// packet header = |protocol id (4)|packet id(4)|packet length (4)|option (4)|
function sendPacket(ws, protocolId, packetId, packet) {
    const headerSize = 16
    let packetData = JSON.stringify(packet)
    let length = headerSize + packetData.length

    // packet frame buffer
    let buff = new ArrayBuffer(length)

    // 16 bytes header buffer setting
    let header = new Uint32Array(buff, 0, 4)
    header[0] = protocolId // protocol id
    header[1] = packetId // packet id
    header[2] = length // packet length
    header[3] = 0 // json string

    // json string to arraybuffer
    let view = new Uint8Array(buff, headerSize)
    for (let i = 0, strLen = packetData.length; i < strLen; ++i) {
        view[i] = packetData.charCodeAt(i)
    }
    ws.send(buff)
}

//------------------------------------------------------------------------------
// send packet (binary format)
// ws: websocket
// packet: ArrayBuffer
// protocolId: number
// packetId: number
// packet format = | packet header (16 byte) | packet data |
// packet length = header size (16 byte) + data size (? byte)
// packet header = |protocol id (4)|packet id(4)|packet length (4)|option (4)|
function sendPacketBinary(ws, protocolId, packetId, packet, packetLen) {
    let length = 16 + packetLen
    length += length % 4 == 0 ? 0 : 4 - (length % 4) // 4byte alignment

    let buff = new ArrayBuffer(length)
    let header = new Uint32Array(buff)
    header[0] = protocolId // protocol id
    header[1] = packetId // packet id
    header[2] = buff.byteLength // packet length
    header[3] = 0x01 // binary option

    let src = new Uint8Array(packet)
    let dst = new Uint8Array(buff, 16)
    for (let i = 0; i < src.length; ++i) dst[i] = src[i]

    ws.send(buff)
}


class webrelay {}


//------------------------------------------------------------------------
// webrelay s2c Protocol Dispatcher
//------------------------------------------------------------------------
webrelay.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 12000
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
				if (handler instanceof webrelay.s2c_ProtocolHandler)
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
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 851424104: // AckLogin
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const type = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							name,
							type,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4021090742: // ReqStartInstance
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const url = packet.getStr()
						const parsePacket = {
							name,
							url,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2576937513: // ReqCloseInstance
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const parsePacket = {
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2863529255: // ReqConnectProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const parsePacket = {
							url,
							name,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 357726072: // ReqPing
				{
					if (option == 1) { // binary?
						const id = packet.getInt32()
						const parsePacket = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// webrelay c2s Protocol Dispatcher
//------------------------------------------------------------------------
webrelay.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 12100
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
				if (handler instanceof webrelay.c2s_ProtocolHandler)
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
						const type = packet.getInt32()
						const parsePacket = {
							name,
							type,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 619773631: // AckStartInstance
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
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 673086330: // AckCloseInstance
				{
					if (option == 1) { // binary?
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1736849374: // AckConnectProxyServer
				{
					if (option == 1) { // binary?
						const url = packet.getStr()
						const name = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							url,
							name,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3275360863: // AckPing
				{
					if (option == 1) { // binary?
						const id = packet.getInt32()
						const parsePacket = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof webrelay.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// webrelay s2c Protocol Handler
//------------------------------------------------------------------------
webrelay.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


//------------------------------------------------------------------------
// webrelay c2s Protocol Handler
//------------------------------------------------------------------------
webrelay.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


class cluster {}

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
// cluster s2c Protocol Dispatcher
//------------------------------------------------------------------------
cluster.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 8200
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
				if (handler instanceof cluster.s2c_ProtocolHandler)
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
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4148808214: // AckRun
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 114435221: // AckStop
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2663605951: // SpawnRobot
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const modelName = packet.getStr()
						const parsePacket = {
							robotName,
							modelName,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 27410734: // RemoveRobot
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const parsePacket = {
							robotName,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2061430798: // RobotWorkFinish
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const taskId = packet.getUint32()
						const totalWorkCount = packet.getUint32()
						const finishWorkCount = packet.getUint32()
						const parsePacket = {
							robotName,
							taskId,
							totalWorkCount,
							finishWorkCount,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const dir = Parse_Vector3(packet)
						const parsePacket = {
							robotName,
							state1,
							state2,
							state3,
							state4,
							pos,
							dir,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 896947492: // RobotPath
				{
					if (option == 1) { // binary?
						const robotName = packet.getStr()
						const path = packet.getUint16Array()
						const wtimes = packet.getUint16Array()
						const parsePacket = {
							robotName,
							path,
							wtimes,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4096031841: // AckLogFileName
				{
					if (option == 1) { // binary?
						const logFileName = packet.getStr()
						const parsePacket = {
							logFileName,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 357726072: // ReqPing
				{
					if (option == 1) { // binary?
						const id = packet.getInt32()
						const parsePacket = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// cluster c2s Protocol Dispatcher
//------------------------------------------------------------------------
cluster.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 8300
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
				if (handler instanceof cluster.c2s_ProtocolHandler)
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
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 923151823: // ReqRun
				{
					if (option == 1) { // binary?
						const projectName = packet.getStr()
						const mapFileName = packet.getStr()
						const pathFileName = packet.getStr()
						const nodeFileNames = packet.getStrArray()
						const taskName = packet.getStr()
						const parsePacket = {
							projectName,
							mapFileName,
							pathFileName,
							nodeFileNames,
							taskName,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1453251868: // ReqStop
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3127197809: // ReqLogFileName
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3275360863: // AckPing
				{
					if (option == 1) { // binary?
						const id = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof cluster.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// cluster s2c Protocol Handler
//------------------------------------------------------------------------
cluster.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


//------------------------------------------------------------------------
// cluster c2s Protocol Handler
//------------------------------------------------------------------------
cluster.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


class remotedbg2 {}

function Parse_sSyncSymbol(packet) {
	return {
			scope: packet.getStr(),
			name: packet.getStr(),
			var: packet.getTypeVariant(),
	}
}
function Parse_sSyncSymbolVector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_sSyncSymbol(packet))
	return ar
}
function Make_sSyncSymbol(packet, data) {
}
function Make_sSyncSymbolVector(packet, data) {
}
function Parse_sRegister(packet) {
	return {
			idx: packet.getUint32(),
			cmp: packet.getBool(),
			val: packet.getTypeVariantVector(),
	}
}
function Parse_sRegisterVector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_sRegister(packet))
	return ar
}
function Make_sRegister(packet, data) {
}
function Make_sRegisterVector(packet, data) {
}

//------------------------------------------------------------------------
// remotedbg2 r2h Protocol Dispatcher
//------------------------------------------------------------------------
remotedbg2.r2h_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 5301
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
				if (handler instanceof remotedbg2.r2h_ProtocolHandler)
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
			case 1418562193: // UploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const code = packet.getStr()
						const parsePacket = {
							itprId,
							code,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1644585100: // ReqIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 923151823: // ReqRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const runType = packet.getStr()
						const parsePacket = {
							itprId,
							runType,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2884814738: // ReqOneStep
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 742173167: // ReqResumeRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 784411795: // ReqBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							enable,
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1453251868: // ReqStop
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const parsePacket = {
							itprId,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							eventName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							eventName,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3084593987: // ReqStepDebugType
				{
					if (option == 1) { // binary?
						const stepDbgType = packet.getInt32()
						const parsePacket = {
							stepDbgType,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2166551586: // ReqDebugInfo
				{
					if (option == 1) { // binary?
						const itprIds = packet.getInt32Array()
						const parsePacket = {
							itprIds,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2532286881: // ReqHeartBeat
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.r2h_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// remotedbg2 h2r Protocol Dispatcher
//------------------------------------------------------------------------
remotedbg2.h2r_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 5300
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
				if (handler instanceof remotedbg2.h2r_ProtocolHandler)
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
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4005257575: // AckUploadIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1397310616: // AckIntermediateCode
				{
					if (option == 1) { // binary?
						const itprId = packet.getUint8()
						const result = packet.getUint8()
						const count = packet.getUint8()
						const index = packet.getUint8()
						const totalBufferSize = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket = {
							itprId,
							result,
							count,
							index,
							totalBufferSize,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4148808214: // AckRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3643391279: // AckOneStep
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1012496086: // AckResumeRun
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2129545277: // AckBreak
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							enable,
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 114435221: // AckStop
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1658444570: // AckInput
				{
					if (option == 1) { // binary?
						const itprId = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							itprId,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							eventName,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4225702489: // AckStepDebugType
				{
					if (option == 1) { // binary?
						const stepDbgType = packet.getInt32()
						const result = packet.getInt32()
						const parsePacket = {
							stepDbgType,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4276104084: // AckDebugInfo
				{
					if (option == 1) { // binary?
						const itprIds = packet.getInt32Array()
						const result = packet.getInt32()
						const parsePacket = {
							itprIds,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							indices,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							infoType,
							reg,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							start,
							count,
							symbol,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
						const parsePacket = {
							itprId,
							vmIdx,
							output,
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1133387750: // AckHeartBeat
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof remotedbg2.h2r_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// remotedbg2 r2h Protocol Handler
//------------------------------------------------------------------------
remotedbg2.r2h_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


//------------------------------------------------------------------------
// remotedbg2 h2r Protocol Handler
//------------------------------------------------------------------------
remotedbg2.h2r_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


class vplp {}

function Parse_sNodeFile(packet) {
	return {
			name: packet.getStr(),
	}
}
function Parse_sNodeFileVector(packet) {
	const size = packet.getUint32()
	if (size == 0) return []
	let ar = []
	for(let i=0; i < size; ++i)
		ar.push(Parse_sNodeFile(packet))
	return ar
}
function Make_sNodeFile(packet, data) {
}
function Make_sNodeFileVector(packet, data) {
}

//------------------------------------------------------------------------
// vplp s2c Protocol Dispatcher
//------------------------------------------------------------------------
vplp.s2c_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 1000
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
				if (handler instanceof vplp.s2c_ProtocolHandler)
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
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 851424104: // AckLogin
				{
					if (option == 1) { // binary?
						const id = packet.getStr()
						const result = packet.getInt32()
						const parsePacket = {
							id,
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3863877132: // AckRunVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3454830338: // AckRunVisualProgStream
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2713087572: // AckRunVisualProgFiles
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 1895439953: // AckStopVisualProg
				{
					if (option == 1) { // binary?
						const result = packet.getInt32()
						const parsePacket = {
							result,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.s2c_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// vplp c2s Protocol Dispatcher
//------------------------------------------------------------------------
vplp.c2s_Dispatcher = class {
	constructor(isRelay = false) {
		this.protocolId = 2000
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
				if (handler instanceof vplp.c2s_ProtocolHandler)
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
						const id = packet.getStr()
						const parsePacket = {
							id,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 2250021743: // ReqRunVisualProg
				{
					if (option == 1) { // binary?
						const nodeFile = Parse_sNodeFile(packet)
						const parsePacket = {
							nodeFile,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3686541167: // ReqRunVisualProgStream
				{
					if (option == 1) { // binary?
						const count = packet.getUint32()
						const index = packet.getUint32()
						const data = packet.getUint8Array()
						const parsePacket = {
							count,
							index,
							data,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 3721131371: // ReqRunVisualProgFiles
				{
					if (option == 1) { // binary?
						const nodeFileNames = packet.getStrArray()
						const parsePacket = {
							nodeFileNames,
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					}
				}
				break;

			case 4258374867: // ReqStopVisualProg
				{
					if (option == 1) { // binary?
						const parsePacket = {
						}
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
						})
					} else { // json?
						const parsePacket = JSON.parse(packet.getStr())
						handlers.forEach(handler => {
							if (handler instanceof vplp.c2s_ProtocolHandler)
								handler.Receive(wss, ws, protocolId, packetId, parsePacket)
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
// vplp s2c Protocol Handler
//------------------------------------------------------------------------
vplp.s2c_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


//------------------------------------------------------------------------
// vplp c2s Protocol Handler
//------------------------------------------------------------------------
vplp.c2s_ProtocolHandler = class {
	constructor() {} 
	Relay(wss, ws, packet) {} 
	Receive(wss, ws, protocolId, packetId, packet) {} 
}


var g_ws = null;
var g_isConnect = false;
var g_dispatchers = [];
var g_handlers = [];

onmessage = (evt) => {
  switch (evt.data.type) {
    case "connect":
      {
		g_dispatchers = [];
		g_handlers = [];


				g_dispatchers.push(new webrelay.s2c_Dispatcher());
				g_dispatchers.push(new remotedbg2.h2r_Dispatcher());
				g_dispatchers.push(new vplp.s2c_Dispatcher());
				g_dispatchers.push(new cluster.c2s_Dispatcher());
				g_dispatchers.push(new cluster.s2c_Dispatcher());

				var handler0 = new webrelay.s2c_ProtocolHandler();
				handler0.Receive = onReceive;
				g_handlers.push(handler0);

				var handler1 = new remotedbg2.h2r_ProtocolHandler();
				handler1.Receive = onReceive;
				g_handlers.push(handler1);

				var handler2 = new vplp.s2c_ProtocolHandler();
				handler2.Receive = onReceive;
				g_handlers.push(handler2);

				var handler3 = new cluster.s2c_ProtocolHandler();
				handler3.Receive = onReceive;
				g_handlers.push(handler3);


        g_ws = new WebSocket(evt.data.url);
        g_ws.onopen = (event) => {
          g_isConnect = true;
          postMessage({ type: "open" });
        };

        g_ws.onmessage = (event) => {
          let fileReader = new FileReader();
          fileReader.onload = (e) => {
            if (!(e.target?.result instanceof ArrayBuffer)) return;
            let packet = new Packet();
            packet.initWithArrayBuffer(e.target.result);
            const protocolId = packet.getUint32();
            const packetId = packet.getUint32();
            let dispatcher = g_dispatchers.find(
              (p) => p.protocolId == protocolId
            );
            if (!dispatcher) {
              return;
            }
            dispatcher.dispatch(null, g_ws, packet, g_handlers);
          };
          fileReader.readAsArrayBuffer(event.data);
        };

        g_ws.onerror = (event) => {
          postMessage({ type: "error" });
        };

        g_ws.onclose = (event) => {
          g_isConnect = false;
          postMessage({ type: "close" });
        };
      }
      break;

    case "close":
      if (g_ws) g_ws.close();
      g_handlers = [];
      g_dispatchers = [];
      break;

    case "send":
      if (!g_isConnect) return;
      sendPacket(g_ws, evt.data.protocolId, evt.data.packetId, evt.data.packet);
      break;

    case "sendBinary":
      if (!g_isConnect) return;
      sendPacketBinary(
        g_ws,
        evt.data.protocolId,
        evt.data.packetId,
        evt.data.packet,
        evt.data.packetLen
      );
      break;
  }
};

