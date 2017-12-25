// 인공지능 객체 인터페이스
// 인공지능을 적용하려는 객체는 이 인터페이스를 상속받아야 한다.
#pragma once


namespace graphic {
	class cBoundingSphere;
	class cBoundingPlane;
	class cNode;
}

namespace ai
{
	static Transform g_tempTransform;

	template<class T>
	class iActorInterface
	{
	public:
		iActorInterface(T *p)  : m_ptr(p) {}
		virtual ~iActorInterface() {}

		virtual Transform& aiGetTransform() { return g_tempTransform; }

		virtual void aiSetAnimation(const Str64 &animationName) {}

		virtual graphic::cNode* aiCollision(const graphic::cBoundingSphere &srcBSphere
			, OUT graphic::cBoundingSphere &collisionSphrere) {return NULL;}

		virtual bool aiCollisionWall(OUT graphic::cBoundingPlane &out) {return false;}


	public:
		T *m_ptr;
	};

}
