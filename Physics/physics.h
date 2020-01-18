//
// 2020-01-18, jjuiddong
// physics engine root header
//		- PhysX wrapping class
//
#pragma once


// physx release macro
#if !defined(PHY_SAFE_RELEASE)
	#define PHY_SAFE_RELEASE(p) {if((p)) {p->release(); p = nullptr;}}
#endif

#include "PxPhysicsAPI.h" // PhysX header

#include "util/errorcallback.h"
#include "util/utility.h"
#include "actor/rigidactor.h"
#include "joint/joint.h"
#include "manager/physicssync_interface.h"
#include "manager/physicssync.h"
#include "manager/physicsengine.h"





#if defined(_DEBUG)
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/Lib/vc15win32/PhysX3DEBUG_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3CookingDEBUG_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3CommonDEBUG_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3ExtensionsDEBUG.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxFoundationDEBUG_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxPvdSDKDEBUG_x86.lib")
#else
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/Lib/vc15win32/PhysX3_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3Cooking_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3Common_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3Extensions.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxFoundation_x86.lib")
	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxPvdSDK_x86.lib")
#endif
