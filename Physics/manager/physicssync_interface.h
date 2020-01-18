//
// 2020-01-18, jjuiddong
// phsics object sync interface
//
#pragma once


namespace phys
{
	class cPhysicsEngine;

	interface iPhysicsSync : public physx::PxDeletionListener
	{

		virtual bool Sync() = 0;

		// PxDeletionListener interface
		virtual void onRelease(const physx::PxBase* observed, void* userData
			, physx::PxDeletionEventFlag::Enum deletionEvent) override  { assert(0); }

	};

}

