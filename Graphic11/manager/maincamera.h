// 전역 카메라 클래스.
//

#pragma once


namespace graphic
{

	//DECLARE_TYPE_NAME_SCOPE(graphic, cMainCamera)
	class cMainCamera : public common::cSingleton<cMainCamera>
									//public cCamera
									//, public common::cSingleton<cMainCamera>
									//, public memmonitor::Monitor<cMainCamera, TYPE_NAME(cMainCamera)>
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


//	inline cMainCamera* GetMainCamera() { return cMainCamera::Get(); }
	inline cCamera* GetMainCamera() { return cMainCamera::Get()->Top(); }


	class cAutoCam
	{
	public:
		cAutoCam(cCamera *cam) {
			cMainCamera::Get()->PushCamera(cam);
		}
		~cAutoCam() {
			cMainCamera::Get()->PopCamera();
		}
	};
}
