
#include "stdafx.h"
#include "tesswrapper.h"
#include <baseapi.h>

using namespace tess;
using namespace tesseract;


#ifdef _DEBUG
	#pragma comment(lib, "opencv_core310d.lib")
	#pragma comment(lib, "tesseract305d.lib")
#else
	#pragma comment(lib, "opencv_core310.lib")
	#pragma comment(lib, "opencv_highgui310.lib")
	#pragma comment(lib, "tesseract305.lib")
#endif



cTessWrapper::cTessWrapper()
	: m_tessApi(NULL)
{	
}

cTessWrapper::~cTessWrapper()
{
	if (m_tessApi)
	{
		m_tessApi->End();
		SAFE_DELETE(m_tessApi);
	}
}


bool cTessWrapper::Init(const string &dataPath, const string &language)
{
	m_tessApi = new TessBaseAPI();
	if (m_tessApi->Init(dataPath.c_str(), language.c_str()))
	{
		SAFE_DELETE(m_tessApi);
		return false;
	}

	return true;
}


// 문자 인식
string cTessWrapper::Recognize(cv::Mat &img)
{
	RETV(!m_tessApi, "");

	m_tessApi->SetImage((uchar*)img.data, img.size().width, img.size().height, img.channels(), img.step1());
	m_tessApi->Recognize(0);

	char *outText = m_tessApi->GetUTF8Text();
	string result = outText;
	SAFE_DELETEA(outText);
	return result;
}
