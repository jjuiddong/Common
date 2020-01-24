//
// 2020-01-18, jjuiddong
// default error callback
//
#pragma once


namespace phys
{

	class cDefaultErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message
			, const char* file, int line) {
			//assert(0);
		}
	};

}
