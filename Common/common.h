
#pragma once


#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
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

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#define RET(exp)		{if((exp)) return; }			// exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }
#define BRK(exp)		{if((exp)) break; }			// exp가 break


typedef unsigned int    u_int;


// 포함 파일들.
#include <windows.h>
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
#include <shlwapi.h>
#include <boost/foreach.hpp>


using std::string;
using std::wstring;
using std::vector;
using std::map;
using std::list;
using std::set;
using std::queue;
using std::stringstream;
using std::wstringstream;


#include "container/vectorhelper.h"
#include "container/shmmap.h"
#include "container/vectormap.h"
#include "math/Math.h"
#include "etc/singleton.h"
#include "etc/filepath.h"
#include "etc/stringfunc.h"
#include "etc/dbg.h"
#include "etc/observer.h"
#include "etc/observer2.h"
#include "etc/rect.h"
#include "etc/genid.h"
#include "etc/memorypool.h"
#include "etc/rand.h"
#include "etc/config.h"
#include "etc/autocs.h"
#include "etc/Serial.h"
#include "etc/BufferedSerial.h"
#include "etc/SerialAsync.h"
#include "etc/ip.h"
#include "etc/date.h"
#include "etc/inifile.h"
#include "etc/shmmem.h"

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
//#pragma comment( lib, "gdiplus.lib" ) 

