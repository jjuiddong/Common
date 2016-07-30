
#pragma once


#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif

// #ifdef _WIN32
// #  define snprintf _snprintf
// #endif

// #include <stdlib.h>					// malloc(), free()
// #  include <GL/glut.h>
// #include <AR/config.h>
// #include <AR/video.h>
// #include <AR/param.h>			// arParamDisp()
// #include <AR/ar.h>
// #include <AR/gsub.h>
// #include <AR/gsub_lite.h>

#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/gsub_lite.h>



// qrcode table map
 #include "arengine.h"
#include "arutil.h"
#include "qrcode.h"
#include "qrtablemap.h"
#include "qrquadrant.h"



#ifdef _DEBUG
	#pragma comment(lib, "ARd.lib")
	#pragma comment(lib, "ARICPd.lib")
	#pragma comment(lib, "ARgsubd.lib")
	#pragma comment(lib, "ARgsub_lited.lib")
	#pragma comment(lib, "ARvideod.lib")
	#pragma comment(lib, "pthreadVC2.lib")
#else
	#pragma comment(lib, "AR.lib")
	#pragma comment(lib, "ARICP.lib")
	#pragma comment(lib, "ARgsub.lib")
	#pragma comment(lib, "ARgsub_lite.lib")
	#pragma comment(lib, "ARvideo.lib")
//	#pragma comment(lib, "pthreadVC2.lib")
#endif
