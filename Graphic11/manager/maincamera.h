// 전역 카메라 클래스.
//

#pragma once


namespace graphic
{

	class cMainCamera : public common::cSingleton<cMainCamera>
	{
	public:
		cMainCamera();
		virtual ~cMainCamera();

		void PushCamera(cCamera *cam);
		void PopCamera();
		cCamera* Top();


	public:
		vector<cCamera*> m_cams;
	};

	inline cCamera& GetMainCamera() { return *cMainCamera::Get()->Top(); }
	inline cCamera* cMainCamera::Top() { return m_cams.back(); }



	class cAutoCam
	{
	public:
		cAutoCam(cCamera *cam) {cMainCamera::Get()->PushCamera(cam);}
		~cAutoCam() {cMainCamera::Get()->PopCamera();}
	};
}
