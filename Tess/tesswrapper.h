//
// 2016-07-27, jjuiddong
//
// tesseract 래핑 클래스
// tesseract를 그대로 사용하면, 컴파일 오류가 생겨, 따로 프로젝트로 만들었다.
//
#pragma once

#include <opencv2/highgui/highgui.hpp>


namespace tesseract {
	class TessBaseAPI;
}


namespace tess
{

	class cDictionary2;
	class cTessWrapper
	{
	public:
		cTessWrapper();
		virtual ~cTessWrapper();

		bool Init(const string &dataPath, const string &language, 
			const string &dictionaryFileName);

		string Recognize(cv::Mat &img);

		string Dictionary(const string &dictionaryFileName, const string &src, 
			OUT float &maxFitness, const int flags=0);

		string Dictionary2(const string &dictionaryFileName, const string &src, 
			OUT string &out, OUT float &maxFitness, OUT int &t1, OUT int &t2); // debugging


	protected:
		cDictionary2* GetDictionary(const string &dictionaryFileName);


	public:
		tesseract::TessBaseAPI *m_tessApi;
		map<string, cDictionary2*> m_dicts;
	};

}