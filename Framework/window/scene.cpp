
#include "stdafx.h"
#include "scene.h"

using namespace graphic;
using namespace framework;


cScene::cScene(const int id, const string &scenName)
	: cWindow(NULL, id, scenName)
{
}

cScene::~cScene()
{
}


bool cScene::Init(graphic::cRenderer &renderer)
{
	return __super::Init(renderer);
}


bool cScene::Update(const float deltaSeconds)
{
	return __super::Update(deltaSeconds);
}


void cScene::Render(graphic::cRenderer &renderer, const Matrix44 &parentTm)
{
	__super::Render(renderer, parentTm);
}


bool cScene::MessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::MessageProc(message, wParam, lParam);
}
