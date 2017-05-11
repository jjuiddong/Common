#include "stdafx.h"
#include "maincamera.h"

using namespace graphic;


cMainCamera::cMainCamera() 
{
	// Default Camera
	cCamera *cam = new cCamera();
	cam->SetCamera(Vector3(100,100,-500), Vector3(0,0,0), Vector3(0,1,0));
	cam->SetProjection( D3DX_PI / 4.f, 1024.f/768.f, 1.f, 10000.0f);
	PushCamera(cam);
}

cMainCamera::~cMainCamera()
{
	for (auto &p : m_cams)
		delete p;
	m_cams.clear();
}


void cMainCamera::PushCamera(cCamera *cam)
{
	m_cams.push_back(cam);
}


void cMainCamera::PopCamera()
{
	m_cams.pop_back();
}


cCamera* cMainCamera::Top()
{
	return m_cams.back();
}
