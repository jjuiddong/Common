//
// 2020-11-18, jjuiddong
// variant type from C++
//

import Symbol, { SymbolType } from "../nodeeditor/symbol";
import Util from "./util";

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


export default class TypeVariant {
  vt: number = VT_EMPTY; // value type
  value: any = 0; // value

  constructor() {
  }

  //------------------------------------------------------------------------------
  // set value
  set(typeVar : TypeVariant) {
    this.vt = typeVar.vt
    this.value = Util.deepCopy(typeVar.value)
  }

  //------------------------------------------------------------------------------
  // parse variant binary data in dataview
  // return read size
  fromDataView(dataView: DataView, offset: number) {
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
          const dummy = 4 - (offset % 4) // 4 bytes alignments
          offset += dummy;
          length += dummy;
        }
        break;
      default:
        if (vt & VT_BYREF) {
          if (offset % 4 != 0) {
            const dummy = 4 - (offset % 4) // 4 bytes alignments
            offset += dummy;
            length += dummy;
          }
        }
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
        this.value = dataView.getUint8(offset) > 0 ? true : false;
        length += 1;
        break;
      case VT_I1:
        this.value = dataView.getInt8(offset);
        length += 1;
        break;
      case VT_UI1:
        this.value = dataView.getUint8(offset);
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
        this.value = dataView.getInt32(offset, true);
        length += 8;
        break;
      case VT_UI8:
        this.value = dataView.getUint32(offset, true);
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
        if (vt & VT_BYREF) {
          this.value = dataView.getInt32(offset, true);
          length += 4;
        } else {
          console.log(`error parse TypeVariant ${vt}`);
        }
        break;
    }
    return length;
  }

  //------------------------------------------------------------------------------
  // parse variant binary data in dataview
  // return read size
  toString(isStringDoubleQuoto: boolean = false): string {
    if (this.vt == VT_BSTR) {
      if (isStringDoubleQuoto) {
        return `\"${this.value}\"`
      }
      else {
        return this.value
      }
    }
    else
      return this.value.toString()
  }

  //--------------------------------------------------------------------------------
  // convert to symbol class
  // name : symbol name
  // return symbol
  toSymbol(name: string): Symbol | null {
    let symbol : Symbol | null = new Symbol()
    symbol.name = name
    symbol.value = this.value
    switch (this.vt) {
      case VT_EMPTY:
        symbol.type = SymbolType.None;
        break; // nothing
      case VT_R4:
      case VT_R8:
        symbol.type = SymbolType.Float;
        break;
      case VT_BSTR:
        symbol.type = SymbolType.String;
        break;
      case VT_BOOL:
        symbol.type = SymbolType.Bool;
        break;
      case VT_I2:
      case VT_I4:
      case VT_I1:
      case VT_UI1:
      case VT_UI2:
      case VT_UI4:
      case VT_I8:
      case VT_UI8:
      case VT_INT:
      case VT_UINT:
        symbol.type = SymbolType.Int;
        break;
      default:
        if (this.vt & VT_BYREF) {
          symbol.type = SymbolType.Array;
        } else {
          symbol = null
          console.log(`error toSymbol TypeVariant ${this.vt}`);
        }
        break;
    }
    return symbol
  }

  //--------------------------------------------------------------------------------
  // parse string in dataView
  // return string
  // dataView: DataView
  // offset: offset number
  dv2str(dataView: DataView, offset: number) {
    let str: number[] = [];
    let i: number = offset;
    let c: number = dataView.getUint8(i);
    while (c != 0) {
      str.push(c);
      c = dataView.getUint8(++i);
    }
    if (str.length == 0)
      return "";
    return new TextDecoder().decode(new Uint8Array(str));
  }

  //------------------------------------------------------------------------------
  // clone
  clone() : TypeVariant {
    let ret = new TypeVariant()
    ret.vt = this.vt
    ret.value = Util.deepCopy(this.value)  
    return ret
  }

}
