
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
