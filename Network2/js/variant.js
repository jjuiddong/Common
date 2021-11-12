//
// 2020-11-18, jjuiddong
// variant type from C++
//

// variant type (c++)
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

export default class TypeVariant {
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
        if ((offset % 4) != 0) {
          const dummy = 4 - (offset % 4) // 4 bytes alignments
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
