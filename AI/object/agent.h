//
// 인공지능 객체 인터페이스
// 인공지능을 적용하려는 객체는 이 인터페이스를 상속받아야 한다.
//
// 2018-10-03
//		- agent 클래스를 ai에서 제외함. 구조가 복잡해져서, 추상클래스는 되도록 줄이고 있음
//
#pragma once


namespace graphic {
	class cNode;
}

namespace ai
{
	using namespace common;
	static Transform g_tempTransform;

	template<class T>
	class iAgent
	{
	public:
		iAgent(T *p)  : m_ptr(p) {}
		virtual ~iAgent() {}

		virtual Transform& aiGetTransform() { return g_tempTransform; }
		virtual void aiSetAnimation(const Str64 &animationName) {}


	public:
		T *m_ptr;
	};

}
