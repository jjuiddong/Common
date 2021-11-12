//
// 2020-12-24, jjuiddong
// websocket server
//  - json/binary protocol parsing
//  - packet send with json/binary format
//
import http from 'http'
import WebSocket from 'ws'
import Packet from './packet'
import Util from './utility'

export default class WsSockServer {
    // expressSvr: express server
    constructor(name, expressSvr) {
        this.name = Util.copyString(name)
        this.httpSvr = http.createServer(expressSvr)
        this.clients = []
        this.dispatchers = []
        this.handlers = []

        this.wss = new WebSocket.Server({ server: this.httpSvr })
        this.wss.on('connection', (ws) => {
            ws.on('message', (message) => this.receivePacket(ws, message))
            ws.on('close', (code) => this.closeClient(ws, code))
            ws.on('error', (error) => console.log(`error ws socket ${error}`))            
            this.onAddClient(ws)
        })
        this.wss.on('error', (error) => console.log(`error ${error}`))        
    }

    //------------------------------------------------------------------------------
    // start server
    listen(port, listeningListener) {
        this.httpSvr.listen(port, listeningListener)
    }

    //------------------------------------------------------------------------------
    // find client correspond websocket
    findClient(ws) {
        for (let i = 0; i < this.clients.length; ++i) {
            if (this.clients[i].ws == ws) return this.clients[i]
        }
        return null
    }

    //------------------------------------------------------------------------------
    // find client by name
    findClientByName(name) {
        for (let i = 0; i < this.clients.length; ++i) {
            if (this.clients[i].name == name) return this.clients[i]
        }
        return null
    }

    //------------------------------------------------------------------------------
    // add hanlder
    addHandler(handler) {
        this.handlers.push(handler)
    }

    //------------------------------------------------------------------------------
    // remove hanlder
    removeHandler(handler) {
        this.handlers = this.handlers.filter((p) => p != handler)
    }

    //------------------------------------------------------------------------------
    // add dispatcher
    addDispatcher(dispatcher) {
        this.dispatchers.push(dispatcher)
    }

    //------------------------------------------------------------------------------
    // remove dispatcher
    removeDispatcher(dispatcher) {
        this.dispatchers = this.dispatchers.filter((p) => p != dispatcher)
    }

    //------------------------------------------------------------------------------
    // add client
    addClient(client) {
        this.clients.push(client)
    }

    //------------------------------------------------------------------------------
    // remove client
    removeClient(client) {
        this.clients = this.clients.filter((p) => p != client)
    }

    //------------------------------------------------------------------------------
    // remove client by ws
    removeClientByWs(ws) {
        this.clients = this.clients.filter((p) => p.ws != ws)
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
    static sendPacket(ws, protocolId, packetId, packet) {
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
        // console.log(buff.byteLength)
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
    static sendPacketBinary(ws, protocolId, packetId, packet, packetLen) {
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
        //console.log(buff.byteLength);
    }

    //------------------------------------------------------------------------------
    // send packet with Packet class (binary format)
    // ws: websocket
    // protocolId: number
    // packetId: number
    // packet: Packet class
    static sendPacketBinary2(ws, protocolId, packetId, packet) {
        WsSockServer.sendPacketBinary(
            ws,
            protocolId,
            packetId,
            packet.buff,
            packet.offset
        )
    }

    //------------------------------------------------------------------------------
    // clear
    clear() {
        this.handlers = []
    }

    //--------------------------------------------------------------------------------
    // receive packet
    // ws: websocket
    // message: receive packet data
    receivePacket(ws, message) {
        let packet = new Packet()
        packet.initWithArrayBuffer(new Uint8Array(message).buffer)
        const protocolId = packet.getUint32()
        // const packetId = packet.getUint32();
        this.dispatchers.forEach((dispatcher) => {
            if (
                protocolId == dispatcher.protocolId ||
                0 == dispatcher.protocolId // all.Dispatcher
            )
                dispatcher.dispatch(this, ws, packet, this.handlers)
        })
    }

    //--------------------------------------------------------------------------------
    // close client
    closeClient(ws, code) {
        this.onClose(ws, code)

        // remove client
        this.removeClientByWs(ws)
    }

    //--------------------------------------------------------------------------------
    // force close client, test
    forceCloseClient(ws) {
        ws.terminate()
    }

    //------------------------------------------------------------------------------
    // add client event, overriding this method
    onAddClient(ws) {}

    //------------------------------------------------------------------------------
    // close client socket event, overriding this method
    onClose(ws, code) {}
}
