//
// 2016-07-12, jjuiddong
//
// 모션 움직임 크기를 제어하는 모듈.
// 움직임 진폭이 크면, 진폭을 낮추는 기능을 한다.
//
// 입력:
//		@roll_fin, @pitchl_fin,  @yaw_fin
//		3축의 크기를 입력으로 한다.
//
// 출력: 
//		@amplitude : 변환된 진폭값을 나타냄
//		@amplitude0 : 원본 진폭값을 나타냄
//		@roll_fin, @pitchl_fin,  @yaw_fin : 최종 진폭 값
//
//
#pragma once


namespace motion
{

	class cAmplitudeModule : public cModule
	{
	public:
		cAmplitudeModule();
		virtual ~cAmplitudeModule();

		bool Init(const string &mathScript, const string &modulatorScript);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	public:
		Vector3 m_prevAxis; // previews roll, pitch, yaw
		cMathParser m_mathParser;
		mathscript::cMathInterpreter m_matInterpreter;
		cSimpleModulator m_modulator;
	};

}