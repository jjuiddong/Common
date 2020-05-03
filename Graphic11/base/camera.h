//
// 2017-11-03, jjuiddong
// Camera Interface
// 
#pragma once


namespace graphic
{

	class cCamera
	{
	public:
		cCamera(const char *name);
		virtual ~cCamera();

		virtual void SetCamera(const Vector3 &eyePos, const Vector3 &up);
		virtual void SetCamera(const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up);
		virtual void SetCamera2(const Vector3 &eyePos, const Vector3 &direction, const Vector3 &up);
		virtual void SetProjection(const float fov, const float aspect, const float nearPlane, const float farPlane);
		virtual void SetProjectionOrthogonal(const float width, const float height, const float nearPlane, const float farPlane);

		virtual bool Is2DMode() const;
		virtual void Update(const float deltaSeconds);
		virtual void Bind(cRenderer &renderer) {}

		virtual Ray GetRay(const int sx = -1, const int sy = -1) const;
		virtual void GetRay(const int windowWidth, const int windowHeight, const int sx, const int sy
			, OUT Vector3 &orig, OUT Vector3 &dir) const;

		const Vector3& GetLookAt() const;
		const Vector3& GetEyePos() const;
		virtual Vector3 GetUpVector() const;

		virtual Vector3 GetDirection() const;
		virtual Vector3 GetRight() const;
		virtual float GetDistance() const;
		virtual void SetEyePos(const Vector3 &eye);
		virtual void SetLookAt(const Vector3 &lookAt);
		virtual void SetUpVector(const Vector3 &up);

		const Matrix44& GetViewMatrix() const;
		const Matrix44& GetProjectionMatrix() const;
		const Matrix44& GetViewProjectionMatrix();
		virtual void SetViewMatrix(const Matrix44 &view);
		virtual void SetProjectionMatrix(const Matrix44 &proj);
		virtual void ReCalcProjection(const float nearPlane, const float farPlane);

		virtual void SetViewPort(const float width, const float height);

		virtual void Move(const Vector3 &lookAt
			, const float velocity = 0.f);
		virtual void Move(const Vector3 &eyePos, const Vector3 &lookAt
			, const float velocity = 0.f);
		virtual void MoveNext(const Vector3 &eyePos, const Vector3 &lookAt
			, const float velocity = 0.f);
		virtual void MoveCancel();
		virtual void Move(const cBoundingBox &bbox);

		virtual void MoveFront(const float len);
		virtual void MoveFrontHorizontal(const float len);
		virtual void MoveUp(const float len);
		virtual void MoveUp2(const float len);
		virtual void MoveRight(const float len);
		virtual void MoveRight2(const float len);
		virtual void MoveAxis(const Vector3 &dir, const float len);
		virtual void Trace(cNode *trace, const Matrix44 &tm);

		// LookAt 이 이동한다.
		virtual void Pitch(const float radian);
		virtual void Yaw(const float radian);
		virtual void Roll(const float radian);

		virtual void Zoom(const float len);
		virtual void Zoom(const Vector3 &dir, const float len);
		virtual Matrix44 GetZoomMatrix() const;
		virtual float GetZoom() const;
		virtual void UpdateViewMatrix() {}

		Vector2 GetScreenPos(const Vector3& vPos) const;

		virtual void GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt) const;
		virtual void FitFrustum(const cCamera &camera, const float farPlaneRate = 1.f);
		virtual void FitFrustum(const Matrix44 &matViewProj);
		virtual void FitQuad(const Vector3 vertices[4]);
		virtual cBoundingSphere GetBoundingSphere() const;
		virtual cBoundingSphere GetBoundingSphere(const float nearPlane, const float farPlane) const;


	protected:
		virtual void CheckBoundingBox();
		virtual void UpdateMove(const float deltaSeconds);
		virtual void UpdateTrace(const float deltaSeconds);


	public:
		struct eState { 
			enum Enum { STOP, MOVE, TRAC }; 
		};

		eState::Enum m_state;
		StrId m_name;
		Vector3 m_eyePos;
		Vector3 m_lookAt;
		Vector3 m_up;
		Matrix44 m_view; // View Matrix
		Matrix44 m_proj; // Projection Matrix
		Matrix44 m_viewProj; // m_view X m_proj
		bool m_isOrthogonal;
		bool m_isKeepHorizontal; //default: true (for 3D Camera)

		struct eBoundingType {
			enum Enum {
				NONE, BOX, HALF_SPHERE
			};
		};
		eBoundingType::Enum m_boundingType;
		cBoundingHemiSphere m_boundingHSphere;
		cBoundingBox m_boundingBox;

		cNode *m_traceNode;
		Matrix44 m_traceTm;

		// ViewPort
		// left, top = {0,0}
		float m_fov;
		float m_aspect;
		float m_width;
		float m_height;
		float m_near;
		float m_far;
		float m_oldWidth;
		float m_oldHeight;

		// Animation
		struct sCamMoving
		{
			Vector3 eyePos;
			Vector3 lookAt;
			float velocityPos;
			float velocityLookAt;
		};
		vector<sCamMoving> m_mover;
		Vector3 m_oldPosDir;
		Vector3 m_oldLookAtDir;

		// Camera Movement PID
		float m_kp;
		float m_kd;
	};


	inline const Vector3& cCamera::GetEyePos() const { return m_eyePos; }
	inline const Vector3& cCamera::GetLookAt() const { return m_lookAt; }
	inline const Matrix44& cCamera::GetViewMatrix() const { return m_view; }
	inline const Matrix44& cCamera::GetProjectionMatrix() const { return m_proj; }
	inline const Matrix44& cCamera::GetViewProjectionMatrix() { m_viewProj = m_view * m_proj; return m_viewProj; }
}
