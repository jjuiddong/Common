// 매인 인공지능 전역 함수
// 인공지능 라이브러리가 동작하기 위해서는 Loop() 함수를
// 호출해줘야 한다.
#pragma once


namespace ai
{

	void Loop(const float deltaSeconds);
	void Clear();

}
