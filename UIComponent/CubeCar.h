//
// 큐브 비행체 정보를 관리한다.
//
//
#pragma once


class cCubeCar
{
public:
	cCubeCar();
	virtual ~cCubeCar();

	bool Init(graphic::cRenderer &renderer);
	void Render(graphic::cRenderer &renderer);
	void Update(const float deltaSeconds);

	void SetEulerAngle(const float roll, const float pitch, const float yaw, const float heave);
	const Quaternion& GetRotation() const;


protected:
	graphic::cCube2 m_mesh[6];
	Vector3 m_locate;
	Quaternion m_rotate;
	Matrix44 m_tm;
};


inline const Quaternion& cCubeCar::GetRotation() const { return m_rotate; }
