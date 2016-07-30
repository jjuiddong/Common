
#include "stdafx.h"
#include "CubeCar.h"

using namespace graphic;


cCubeCar::cCubeCar()
{
}

cCubeCar::~cCubeCar()
{

}


// 클래스 초기화.
bool cCubeCar::Init(cRenderer &renderer)
{
	m_mesh[0].SetCube(renderer, Vector3(-1, -1, -1), Vector3(1, 1, 1));
	m_mesh[1].SetCube(renderer, Vector3(1, -1, -1), Vector3(3, 1, 1));
	m_mesh[2].SetCube(renderer, Vector3(-1, 1, -1), Vector3(1, 2, 1));
	const float weight = 0.15f;
	m_mesh[3].SetCube(renderer, Vector3(-4, -weight, -weight), Vector3(6, weight, weight));
	m_mesh[4].SetCube(renderer, Vector3(-weight, -6, -weight), Vector3(weight, 6, weight));
	m_mesh[5].SetCube(renderer, Vector3(-weight, -weight, -6), Vector3(weight, weight, 6));

	m_mesh[0].GetMaterial().InitBlue();
	m_mesh[1].GetMaterial().InitBlue();
	m_mesh[2].GetMaterial().InitBlue();
	m_mesh[3].GetMaterial().InitRed();
	m_mesh[4].GetMaterial().InitGreen();
	m_mesh[5].GetMaterial().InitWhite();

	return true;
}


// 화면에 출력한다.
void cCubeCar::Render(graphic::cRenderer &renderer)
{
	m_mesh[0].Render(renderer, m_tm);
	m_mesh[1].Render(renderer, m_tm);
	m_mesh[2].Render(renderer, m_tm);
	m_mesh[3].Render(renderer, m_tm);
	m_mesh[4].Render(renderer, m_tm);
	m_mesh[5].Render(renderer, m_tm);
}


// 매 프레임마다 연산할 것을 추가한다.
void cCubeCar::Update(const float deltaSeconds)
{
}


// 큐브 각도를 오일러 각으로 설정한다.
// Radian 각을 인자로 한다.
void cCubeCar::SetEulerAngle(const float roll, const float pitch, const float yaw, const float heave)
{
	m_rotate.Euler2(Vector3(roll, yaw, pitch));
	//m_tm = m_rotate.GetMatrix();

	Matrix44 t;
	t.SetPosition(Vector3(0, heave, 0));

	m_tm = m_rotate.GetMatrix() * t;
}

