#pragma once

/*
#ifdef DLL_EXPORT
	#define DLL_FUNC __declspec(dllexport)
#else
	#define DLL_FUNC __declspec(dllimport)
#endif


extern "C"
{
	// 모션 스크립트 파일을 읽는 함수
	// 성공하면 1을 리턴하고, 그렇지 않으면 0을 리턴한다.
	DLL_FUNC int MotionInit(int hWnd, char *fileName);


	// 매 루프때마다 호출해야 하는 함수
	// deltaSeconds : 1초가 지나면 1
	DLL_FUNC void MotionUpdate(const float deltaSeconds);


	// 초기화
	DLL_FUNC void Clear();


	// 모션장치와 연결을 시도한다.
	// 모션장치를 On 시키고, Stop 상태에서 대기한다.
	// 성공하면 1을 리턴하고, 그렇지 않으면 0을 리턴한다.
	DLL_FUNC int MotionStart();


	// 모션장치를 Off 시킨다.
	// 모션장치와 연결이 끊어진다.
	// 성공하면 1을 리턴하고, 그렇지 않으면 0을 리턴한다.
	DLL_FUNC int MotionStop();


	// 출력 포맷을 지정한다.
	// :output - :format 을 설정한다.
	// outputIndex : :output 이 2개 이상일 경우, 모션 스크립트에서 나오는 순서대로 0,1~
	// 로 정해진다.
	// formatIndex : :format 이 2개 이상일 경우, 모션 스크립트에서 나오는 순서대로 0,1~
	// 로 정해진다.
	DLL_FUNC void MotionSetOutputFormat(int outputIndex, int formatIndex);


	// 모션 스크립트 Varible을 float형으로 변환해서 가져온다.
	DLL_FUNC float MotionGetSymbolFloat(char *symbolName);


	// 모션 스크립트 Varible을 int형으로 변환해서 가져온다.
	DLL_FUNC int MotionGetSymbolInt(char *symbolName);


	// 모션 스크립트 Varible을 float형으로 저장한다.
	DLL_FUNC void MotionSetSymbolFloat(char *symbolName, float val);


	// 모션 스크립트 Varible을 int형으로 저장한다.
	DLL_FUNC void MotionSetSymbolInt(char *symbolName, int val);



	//----------------------------------------------------------------------------
	// 이미지 인식 초기화
	DLL_FUNC int MatchReadScript(char *fileName, char *executeLabel);

	// 이미지 캡쳐, 인식하기
	DLL_FUNC void MatchCaptureAndDetect(char *result);

	// 이미지 캡쳐 지연
	DLL_FUNC void MatchCaptureDelay(int delayMilliSeconds);

}
*/
