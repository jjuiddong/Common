//
// 2016-07-13, jjuiddong
// 이미지 매칭 쓰레드 정의
//
#pragma once


namespace cvproc {
	namespace imagematch {

} }

unsigned __stdcall MatchThreadSub(void *ptr);
unsigned __stdcall MatchThreadMain(void *ptr);
