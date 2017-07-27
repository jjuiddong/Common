// 카메라 클래스.
#pragma once


namespace graphic
{
	class cRenderer;

	class cCamera
	{
	public:
		cCamera();
		cCamera(const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up);
		virtual ~cCamera();

		void Init(cRenderer *renderer);
		void Update(const float deltaSeconds);
		void Render(cRenderer &renderer);
		void Bind(cShader &shader);
		void Bind(cRenderer &renderer);

		void SetCamera(const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up);
		void SetProjection(const float fov, const float aspect, const float nearPlane, const float farPlane);
		void SetProjectionOrthogonal(const float width, const float height, const float nearPlane, const float farPlane);
		void ReCalcProjection(const float nearPlane, const float farPlane);
		void SetViewPort(const int width, const int height);
		void SetEyePos(const Vector3 &eye);
		void SetLookAt(const Vector3 &lookAt);
		void SetUpVector(const Vector3 &up);
		void SetViewMatrix(const Matrix44 &view);
		void SetProjectionMatrix(const Matrix44 &proj);
		const Vector3& GetEyePos() const;
		const Vector3& GetLookAt() const;
		Vector3 GetUpVector() const;
		const Matrix44& GetViewMatrix() const;
		const Matrix44& GetProjectionMatrix() const;
		const Matrix44& GetViewProjectionMatrix();
		Vector3 GetDirection() const;
		Vector3 GetRight() const;
		float GetDistance() const; // lookAt - eyePos 사이 거리.

		void GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt);
		void FitFrustum(const cCamera &camera, const float farPlaneRate =1.f);
		void FitFrustum(const Matrix44 &matViewProj);
		void FitQuad(const Vector3 vertices[4]);


		// LookAt 이 이동한다.
		void Pitch( const float radian );
		void Yaw( const float radian );
		void Roll( const float radian );

		// EyePos 가 이동한다.
		void Pitch2( const float radian);
		void Pitch2(const float radian, const Vector3 &up);
		void Yaw2( const float radian);
		void Yaw2(const float radian, const Vector3 &up);
		void Roll2( const float radian);

		void Pitch3(const float radian, const Vector3 &target);
		void Yaw3(const float radian, const Vector3 &target);
		void Roll3(const float radian, const Vector3 &target);

		void Yaw4(const float radian, const Vector3 &up, const Vector3 &target);
		void Pitch4(const float radian, const Vector3 &up, const Vector3 &target);

		void KeepHorizontal();

		void Move(const Vector3 &eyePos, const Vector3 &lookAt);
		void MoveNext(const Vector3 &eyePos, const Vector3 &lookAt);
		void MoveCancel();
		void Move(const cBoundingBox &bbox);

		void MoveFront( const float len );
		void MoveFrontHorizontal(const float len);
		void MoveUp( const float len );
		void MoveRight( const float len );
		void MoveAxis( const Vector3 &dir, const float len );
		void Zoom( const float len );
		void Zoom( const Vector3 &dir, const float len);

		Vector3 GetScreenPos(const int viewportWidth, const int viewportHeight, const Vector3& vPos);
		Vector3 GetScreenPos(const Vector3& vPos);
		void GetRay(OUT Vector3 &orig, OUT Vector3 &dir);
		void GetRay(const int sx, const int sy, OUT Vector3 &orig, OUT Vector3 &dir);
		void GetRay(const int windowWidth, const int windowHeight, const int sx, const int sy
			, OUT Vector3 &orig, OUT Vector3 &dir);

		void UpdateParameterFromViewMatrix();
		void UpdateViewMatrix(const bool updateUp=true);


	protected:
		void UpdateProjectionMatrix();


	public:
		struct eState { enum Enum { STOP, MOVE}; };

		eState::Enum m_state;
		bool m_isOrthogonal;
		Vector3 m_eyePos;
		Vector3 m_lookAt;
		Vector3 m_up;
		Matrix44 m_view; // 카메라 행렬.
		Matrix44 m_proj; // 투영 행렬.
		Matrix44 m_viewProj; // m_view X m_proj
		//cLine m_lines[3]; // Front, Up, Right
		float m_fov;
		float m_aspect;
		float m_nearPlane;
		float m_farPlane;
		int m_oldWidth;
		int m_oldHeight;
		int m_width; // ViewPort
		int m_height; // ViewPort
		cRenderer *m_renderer;

		// Animation
		struct sCamMoving
		{
			Vector3 eyePos;
			Vector3 lookAt;
			float velocityPos;
			float velocityLookAt;
		};
		vector<sCamMoving> m_mover;
	};


	inline void cCamera::Init(cRenderer *renderer) { m_renderer = renderer;  }
	inline void cCamera::SetEyePos(const Vector3 &eye) { m_eyePos = eye; UpdateViewMatrix(); }
	inline void cCamera::SetLookAt(const Vector3 &lookAt) { m_lookAt = lookAt; UpdateViewMatrix(); }
	inline void cCamera::SetUpVector(const Vector3 &up) { m_up = up; UpdateViewMatrix(); }
	inline void cCamera::SetViewMatrix(const Matrix44 &view) { m_view = view; }
	inline void cCamera::SetProjectionMatrix(const Matrix44 &proj) { m_proj = proj; UpdateProjectionMatrix(); }
	inline const Vector3& cCamera::GetEyePos() const { return m_eyePos; }
	inline const Vector3& cCamera::GetLookAt() const { return m_lookAt; }
	//inline const Vector3& cCamera::GetUpVector() const { return m_up; }
	inline const Matrix44& cCamera::GetViewMatrix() const { return m_view; }
	inline const Matrix44& cCamera::GetProjectionMatrix() const { return m_proj; }
	inline const Matrix44& cCamera::GetViewProjectionMatrix() { m_viewProj = m_view * m_proj; return m_viewProj; }		 
}
