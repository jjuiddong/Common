//
// Common Library Include
//
//
#pragma once


// Pre Processor Environment Setting
//#ifndef _VS2015
//	#define _VS2015
//#endif

#ifndef _VS2017
	#define _VS2017
#endif


#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
#endif


// https://stackoverflow.com/questions/4913922/possible-problems-with-nominmax-on-visual-c
// https://bspfp.pe.kr/archives/591
// remove min, max macro in "Windows.h"
#ifndef NOMINMAX
	#define NOMINMAX
#endif



// 매크로 정의
#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) {if (p) { delete p; p=NULL;} }
#endif
#ifndef SAFE_DELETEA
	#define SAFE_DELETEA(p) {if (p) { delete[] p; p=NULL;} }
#endif
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif
#ifndef SAFE_RELEASE2
	#define SAFE_RELEASE2(p) {if (p) { p->release(); p=NULL;} }
#endif
#ifndef DX_SAFE_RELEASE
	#define DX_SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#define RET(exp)	{if((exp)) return; } // exp가 true이면 리턴
#define RET2(exp)	{if((exp)) {assert(0); return;} } // exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define RETV2(exp,val)	{if((exp)) {assert(0); return val;} }
#define ASSERT_RET(exp)	{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)	{assert(exp); RETV(!(exp),val ); }
#define BRK(exp)	{if((exp)) break; } // exp가 break


typedef unsigned int u_int;
typedef unsigned char u_char;
typedef unsigned int hashcode;

#ifndef wchar
	typedef wchar_t wchar;
#endif

#ifndef uint
	typedef unsigned int uint;
#endif

#ifndef int64
	typedef __int64 int64;
#endif

#ifndef uint64
	typedef unsigned __int64 uint64;
#endif 


// 포함 파일들.
#include <windows.h>
#include <comdef.h> // _variant_t type
#include <string>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>
#include <queue>
#include <thread>
#include <process.h>
#include <shlwapi.h>
#include <iterator>

#include <atlbase.h>
#include <atlstr.h>
#include <setupapi.h>
#include <winspool.h>
#include <WBemCli.h>
#include <msports.h>

using std::string;
using std::wstring;
using std::vector;
using std::map;
using std::list;
using std::set;
using std::queue;
using std::stringstream;
using std::wstringstream;
using std::min;
using std::max;


#include "container/vectorhelper.h"
#include "container/shmmap.h"
#include "container/vectormap.h"
#include "container/simplestring.h"
#include "container/circularqueue.h"
#include "container/circularqueue2.h"
#include "math/Math.h"
#include "etc/recttype.h"
#include "etc/singleton.h"
#include "etc/filepath.h"
#include "etc/stringfunc.h"
#include "etc/observer.h"
#include "etc/observer2.h"
#include "etc/genid.h"
#include "etc/memorypool.h"
#include "etc/memorypool2.h"
#include "etc/memorypool3.h"
#include "etc/memorypool4.h"
#include "etc/rand.h"
#include "etc/config.h"
#include "etc/autocs.h"
#include "etc/Serial.h"
#include "etc/BufferedSerial.h"
#include "etc/SerialAsync.h"
#include "etc/ip.h"
#include "etc/date.h"
#include "etc/date2.h"
#include "etc/inifile.h"
#include "etc/shmmem.h"
#include "etc/fastmemloader.h"
#include "etc/path.h"
#include "etc/timer.h"
#include "thread/semaphore.h"
#include "thread/task.h"
#include "thread/mutex.h"
#include "thread/thread.h"
#include "thread/wqsemaphore.h"
#include "thread/tpsemaphore.h"
#include "etc/filelogger.h"
#include "etc/dbg.h"
#include "etc/macro.h"
#include "etc/simpledata.h"
#include "etc/port.h"

#include "interpreter/definition.h"
#include "interpreter/event.h"
#include "interpreter/symboltable.h"
#include "interpreter/intermediatecode.h"
#include "interpreter/virtualmachine.h"
#include "interpreter/interpreter.h"
#include "interpreter/debugger.h"
#include "interpreter/vprogfile.h"
