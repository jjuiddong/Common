//
// 2017-11-03, jjuiddong
// 카메라 클래스. Refactoring
//
#pragma once


namespace graphic
{

	class cCamera3D : public cCamera
	{
	public:
		cCamera3D(const char *name);
		cCamera3D(const char *name, const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up);
		virtual ~cCamera3D();

		// EyePos 가 이동한다.
		void Pitch2(const float radian);
		void Pitch2(const float radian, const Vector3 &up);
		void Yaw2(const float radian);
		void Yaw2(const float radian, const Vector3 &up);
		void Roll2(const float radian);

		void Pitch3(const float radian, const Vector3 &target);
		void Yaw3(const float radian, const Vector3 &target);
		void Roll3(const float radian, const Vector3 &target);

		void Yaw4(const float radian, const Vector3 &up, const Vector3 &target);
		void Pitch4(const float radian, const Vector3 &up, const Vector3 &target);

		void KeepHorizontal();
		void UpdateParameterFromViewMatrix();
		virtual cBoundingSphere GetBoundingSphere() const override;
		virtual cBoundingSphere GetBoundingSphere(const float nearPlane, const float farPlane) const override;


		// Override;
		virtual void Update(const float deltaSeconds) override;
		virtual void Bind(cRenderer &renderer) override;

		virtual Vector3 GetUpVector() const override;
		virtual Vector3 GetDirection() const override;
		virtual Vector3 GetRight() const override;
		virtual float GetDistance() const override;

		virtual void UpdateViewMatrix() override;
	};

}
