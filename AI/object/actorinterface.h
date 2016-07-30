// 인공지능 객체 인터페이스
// 인공지능을 적용하려는 객체는 이 인터페이스를 상속받아야 한다.
#pragma once


namespace ai
{
	static Matrix44 g_tempMat;


	template<class T>
	class iActorInterface
	{
	public:
		iActorInterface(T *p)  : m_ptr(p) {}
		virtual ~iActorInterface() {}

		virtual Matrix44& aiGetTransform() { return g_tempMat; }
		virtual void aiSetAnimation(const string &animationName) {}


	public:
		T *m_ptr;
	};

}
