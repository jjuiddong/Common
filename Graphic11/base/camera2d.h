//
// 2017-11-02, jjuiddong
// 2D Camera
// X-Z Plane Camera
//
// 카메라 방향은 항상 -Y 축을 향해있다.
// 카메라 위치와 Up벡터를 조정해서 카메라를 조정한다.
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCamera2D : public cCamera
	{
	public:
		cCamera2D(const char *name);
		cCamera2D(const char *name, const Vector3 &eyePos, const Vector3 &up);
		virtual ~cCamera2D();

		virtual bool Is2DMode() const;
		virtual void Update(const float deltaSeconds) override;
		virtual void Bind(cRenderer &renderer) override;

		void SetProjectionOrthogonal(const float width, const float height, const float nearPlane, const float farPlane);
		void ReCalcProjection(const float nearPlane, const float farPlane);

		virtual Vector3 GetLookAt() const;
		virtual Vector3 GetUpVector() const;
		virtual void SetUpVector(const Vector3 &up);

		virtual Vector3 GetDirection() const override;
		virtual Vector3 GetRight() const override;
		virtual float GetDistance() const override;
		virtual Matrix44 GetZoomMatrix() const;

		void GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt) const;
		void FitFrustum(const cCamera &camera, const float farPlaneRate = 1.f);
		void FitFrustum(const Matrix44 &matViewProj);
		void FitQuad(const Vector3 vertices[4]);

		virtual void SetViewPort(const float width, const float height) override;

		// LookAt 이 이동한다.
		virtual void Roll(const float radian) override;
		// EyePos 가 이동한다.
		void Roll2(const float radian);

		virtual void Move(const Vector3 &eyePos, const Vector3 &lookAt) override;
		virtual void MoveNext(const Vector3 &eyePos, const Vector3 &lookAt) override;
		void Move(const cBoundingBox &bbox);

		virtual void MoveFront(const float len) override;
		virtual void MoveUp(const float len) override;
		virtual void MoveRight(const float len) override;
		virtual void MoveAxis(const Vector3 &dir, const float len) override;
		virtual void Trace(cNode *trace, const Matrix44 &tm);
		virtual void Zoom(const float len);
		virtual void Zoom(const Vector3 &dir, const float len);

		Vector3 GetScreenPos(const int viewportWidth, const int viewportHeight, const Vector3& vPos);
		Vector3 GetScreenPos(const Vector3& vPos);

		Ray GetRay(const int sx = -1, const int sy = -1);
		void GetRay(const int windowWidth, const int windowHeight, const int sx, const int sy
			, OUT Vector3 &orig, OUT Vector3 &dir);

		void UpdateParameterFromViewMatrix();
		virtual void UpdateViewMatrix() override;


	protected:
		virtual void CheckBoundingBox() override;
		virtual void UpdateTrace(const float deltaSeconds) override;


	public:
		float m_zoom;
		float m_minZoom;
		float m_maxZoom;
	};


	inline Vector3 cCamera2D::GetDirection() const { return Vector3(0, -1, 0); }
}
