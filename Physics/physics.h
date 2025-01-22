//
// 2020-01-18, jjuiddong
// physics engine root header
//		- PhysX wrapping class
//
// 2025-01-12
//		- migration physx3.4 -> 4.0
//		- using vcpkg install physix:x64-windows
//		- reference
//			- Migrating From PhysX SDK 3.4 to 4.0
//			- https://nvidia-omniverse.github.io/PhysX/physx/5.4.0/docs/MigrationTo40.html
//
// 2025-01-16
//		- migration physx4.0 -> 5.5
//		- source code
//			-  https://github.com/NVIDIA-Omniverse/PhysX/
//		- build physx library
//			- https://www.youtube.com/watch?v=kfay4cjYEKQ
//		- reference
//			- Migrating From PhysX SDK 4.0 to 5.1
//			- https://nvidia-omniverse.github.io/PhysX/physx/5.1.0/docs/MigrationTo51.html#migrating-from-physx-sdk-4-0-to-5-1
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
#include "articulation/articulation.h"
#include "manager/physicssync_interface.h"
#include "manager/physicssync.h"
#include "manager/physicsengine.h"



#if defined(_WIN64)
	#if defined(_DEBUG)
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/debug/PhysX_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/debug/PhysXCommon_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/debug/PhysXCooking_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/debug/PhysXFoundation_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/debug/PhysXExtensions_static_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/debug/PhysXPvdSDK_static_64.lib")
	#else
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/release/PhysX_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/release/PhysXCommon_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/release/PhysXCooking_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/release/PhysXFoundation_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/release/PhysXExtensions_static_64.lib")
		#pragma comment(lib, "../External/PhysX-5.5/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static_64.lib")
	#endif
#endif


//#if defined(_DEBUG)
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/Lib/vc15win32/PhysX3DEBUG_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3CookingDEBUG_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3CommonDEBUG_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3ExtensionsDEBUG.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxFoundationDEBUG_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxPvdSDKDEBUG_x86.lib")
//#else
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/Lib/vc15win32/PhysX3_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3Cooking_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3Common_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PhysX_3.4/lib/vc15win32/PhysX3Extensions.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxFoundation_x86.lib")
//	#pragma comment(lib, "../External/PhysX-3.4/PxShared/lib/vc15win32/PxPvdSDK_x86.lib")
//#endif
