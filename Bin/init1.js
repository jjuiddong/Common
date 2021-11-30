
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

