
#include "stdafx.h"
#include "tesswrapper.h"
#include "dictionary2.h"
#include <baseapi.h>
#include <mmsystem.h>

using namespace tess;
using namespace tesseract;


#ifdef _DEBUG
	#pragma comment(lib, "tesseract305d.lib")
#else
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
	
	for each (auto kv in m_dicts)
		SAFE_DELETE(kv.second);
	m_dicts.clear();
}


bool cTessWrapper::Init(const string &dataPath, const string &language
	,const string &dictionaryFileName)
{
	m_tessApi = new TessBaseAPI();
	if (m_tessApi->Init(dataPath.c_str(), language.c_str()))
	{
		SAFE_DELETE(m_tessApi);
		return false;
	}

	cDictionary2 *dict = new cDictionary2();
	if (!dict->Init(dictionaryFileName))
	{
		delete dict;
		return false;
	}

	m_dicts[dictionaryFileName] = dict;
	return true;
}


// 문자 인식
string cTessWrapper::Recognize(cv::Mat &img)
{
	RETV(!m_tessApi, "");
	RETV(!img.data, "");

	m_tessApi->SetImage((uchar*)img.data, img.size().width, img.size().height, img.channels(), img.step1());
	m_tessApi->Recognize(0);

	char *outText = m_tessApi->GetUTF8Text();
	string result = outText;
	trim(result);
	SAFE_DELETEA(outText);
	return result;
}


// 사전에 등록된 단어를 리턴
// maxFitness : 적합도, 최대 1
//					    FastSearch() 에서 인식되면 1.
// flags = 0 : FastSearch + ErrorCorrectionSearch
//				1 : FastSearch
string cTessWrapper::Dictionary(const string &dictionaryFileName, 
	const string &src, OUT float &maxFitness, const int flags) //flags=0
{
	cDictionary2 *dict = GetDictionary(dictionaryFileName);

	maxFitness = 1;
	vector<string> out;
	string result = dict->FastSearch(src, out);
	if ((flags==0) && result.empty())
		result = dict->ErrorCorrectionSearch(src, maxFitness);

	return result;
}


// out : fast search result
// maxFitness : 적합도, 최대 1
//					    FastSearch() 에서 인식되면 1.
// t1 : fast search time
// t2 : fastsearch + errorcorrectsearch
string cTessWrapper::Dictionary2(const string &dictionaryFileName, 
	const string &src, OUT string &out, OUT float &maxFitness, OUT int &t1, OUT int &t2)
{
	cDictionary2 *dict = GetDictionary(dictionaryFileName);

	maxFitness = 1;
	const int t0 = timeGetTime();
	vector<string> strs;
	string result = dict->FastSearch(src, strs);
	t1 = timeGetTime() - t0;

	out = result;
	if (result.empty())
		result = dict->ErrorCorrectionSearch(src, maxFitness);

	t2 = timeGetTime() - (t0+t1);

	return result;
}


cDictionary2* cTessWrapper::GetDictionary(const string &dictionaryFileName)
{
	cDictionary2 *dict = NULL;
	auto it = m_dicts.find(dictionaryFileName);
	if (m_dicts.end() == it)
	{
		cDictionary2 *p = new cDictionary2();
		p->Init(dictionaryFileName);
		m_dicts[dictionaryFileName] = p;
		dict = p;
	}
	else
	{
		dict = it->second;
	}

	return dict;
}
