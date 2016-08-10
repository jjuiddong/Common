
#include "stdafx.h"
#include "matchprocessor.h"


using namespace cv;
using namespace cvproc;
using namespace cvproc::imagematch;

Mat cMatchProcessor::m_nullMat; // static 변수 선언


cMatchProcessor::cMatchProcessor()
	: m_detector(cv::xfeatures2d::SURF::create(400))
	, m_isMatchThreadLoop(false)
	, m_threadHandle(NULL)
	, m_isThreadTerminate(false)
	, m_inputImageId(0)
	, m_isLog(true)
	, m_isLog2(true)
	, m_lastMatchResult(NULL)
	, m_tessIdx(0)
{

	for (int i = 0; i < 5; ++i)
	{
		tess::cTessWrapper *p = new tess::cTessWrapper();
		p->Init("./", "eng", "dictionary.txt");
		m_tess.push_back(p);
	}
}

cMatchProcessor::~cMatchProcessor()
{
	Clear();
}


bool cMatchProcessor::Match(INOUT cMatchResult &matchResult )
{
	m_lastMatchResult = &matchResult;

	// find node correspond to label
	const sParseTree *nodeLabel = matchResult.m_nodeLabel;
	if (!nodeLabel)
	{
		matchResult.m_result = 0;
		matchResult.m_resultStr = "not found label";
		return false;
	}

	matchResult.m_matchCount = 0;

	// 중복된 input name을 쓸 경우, 비동기처리시 문제가 발생한다.
	// 쓰레드가 끝나지 않은 상황에서, 같은 input name을 바꿀 수 있다.
	// 그래서 중복되지 않는 input name을 설정해야 한다.
	string inputId;

	++m_inputImageId;
	if (m_inputImageId > 1000000) // limit check
		m_inputImageId = 0;

	if (matchResult.m_registerInput)
	{
		stringstream ss;
		ss << "@input" << m_inputImageId;
		inputId = ss.str();
		SetInputImage(matchResult.m_input, inputId);
	}
	else
	{
		inputId = matchResult.m_inputName;
		if (inputId.empty())
			inputId = "@input";
	}
	//

	m_isMatchThreadLoop = true;
	m_isThreadTerminate = false;

	matchResult.m_input = loadImage(inputId).clone();
	matchResult.m_inputName = inputId;
	matchResult.m_inputImageId = m_inputImageId;
	matchResult.m_isEnd = false;
	matchResult.m_result = 0;

	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, MatchThreadMain, &matchResult, 0, NULL);

	if (matchResult.m_blockMode)
	{
		// 쓰레드를 종료 확인하고, 리턴
		while (!matchResult.m_isEnd)
			Sleep(1);
		//WaitForSingleObject(m_threadHandle, INFINITE);
		m_threadHandle = NULL;
		m_isMatchThreadLoop = false;
	}

	return true;
}


// return value 0 : fail
//					    1 : success
int cMatchProcessor::executeTreeEx(INOUT sExecuteTreeArg &arg)
{
	cMatchScript2 &script = *arg.matchResult->m_script;
	const cv::Mat &input = arg.matchResult->m_input;
	const string &inputName = arg.matchResult->m_inputName;
	const int inputImageId = arg.matchResult->m_inputImageId;
	sParseTree *parent = arg.parent;
	sParseTree *node = arg.node;
	cv::Mat *out = arg.out;

	if (!node)
		return 0;
	if (!node->child)
		return 1; // terminal node, success finish
	if ('@' == node->name[0]) // link node, excute child node
		return 1;

	int matchType = 0; // template match
	if (((script.m_matchType == 1) && (node->matchType == -1)) || (node->matchType == 1))
		matchType = 1; // feature match
	else if (node->matchType == 2)
		matchType = 2; // ocr match

	if (matchType == 2)
	{
		return executeOcr(arg);
	}

	const Mat &matObj = loadImage(node->name);
	if (matObj.empty())
		return 0;

	const cv::Size csize(input.cols - matObj.cols + 1, input.rows - matObj.rows + 1);
	if ((csize.height < 0) || csize.width < 0)
		return 0;

	// roi x,y,w,h
	cv::Rect roi(0, 0, input.cols, input.rows);
	if (!node->IsEmptyRoi())
		roi = { node->roi[0], node->roi[1], node->roi[2], node->roi[3] };
	if (node->isRelation && parent)
	{
		roi.x += parent->matchLoc.x;
		roi.y += parent->matchLoc.y;
	}

	// roi limit check
	roi.x = max(0, roi.x);
	roi.y = max(0, roi.y);
	if (input.cols < roi.x + roi.width)
		roi.width = input.cols - roi.x;
	if (input.rows < roi.y + roi.height)
		roi.height = input.rows - roi.y;

	if ((roi.width < 0) || (roi.height < 0))
	{
		// too small source image
		return 0;
	}

	double min=0, max=0;
	Point left_top;
	const float maxThreshold = (node->threshold == 0) ? 0.9f : node->threshold;
	bool isDetect = false;

	if (m_isLog)
		dbg::Log("executeTree %s, matchType=%d \n", node->name, matchType);

	const Mat *src = &input;

	// channel match
	if (!node->IsEmptyBgr())
	{
		src = &loadScalarImage(inputName, inputImageId, Scalar(node->scalar[0], node->scalar[1], node->scalar[2]), node->scale); // BGR
	}

	// hsv match
	if (!node->IsEmptyHsv())
	{
		src = &loadHsvImage(inputName, inputImageId, Scalar(node->hsv[0], node->hsv[1], node->hsv[2]), Scalar(node->hsv[3], node->hsv[4], node->hsv[5]));
	}

	if (!src->data)
		return 0; // fail

	arg.matchResult->m_matchCount++;
	node->processCnt++; // count node match

	if (matchType == 0) // --> templatematch
	{
		if (((roi.width >= matObj.cols) && (roi.height >= matObj.rows))
			&& (src->channels() == matObj.channels()))
		{
			Mat matResult(csize, IPL_DEPTH_32F);
			cv::matchTemplate((*src)(roi), matObj, matResult, CV_TM_CCOEFF_NORMED);
			cv::minMaxLoc(matResult, &min, &max, NULL, &left_top);
		}

		isDetect = max > maxThreshold;
		node->max = max;

		// 매칭 결과 저장
		const Point lt = left_top + Point(roi.x, roi.y); // relative location
		arg.matchResult->m_data[node->id].max = max;
		arg.matchResult->m_data[node->id].matchRect = Rect(lt.x, lt.y, matObj.cols, matObj.rows);

		// 매칭된 장면을 복사해 리턴한다.
		if (out)
		{
			*out = (*src)(Rect(left_top.x, left_top.y, left_top.x + matObj.cols, left_top.y + matObj.rows));
		}

		if (m_isLog)
			dbg::Log("%s --- templatematch max=%f, IsDetection = %d \n", node->name, max, isDetect);
	}
	else if (matchType == 1) // feature match
	{
		vector<KeyPoint> *objectKeyPoints, *sceneKeyPoints;
		Mat *objectDescriotor, *sceneDescriptor;
		loadDescriptor(node->name, &objectKeyPoints, &objectDescriotor);
		loadDescriptor(inputName, &sceneKeyPoints, &sceneDescriptor);

		if (!objectKeyPoints || !sceneKeyPoints || !sceneKeyPoints || !sceneDescriptor)
			return 0; // 이미지 로딩 실패

		if (!sceneKeyPoints->empty() && sceneDescriptor->data)
		{
			cFeatureMatch match(m_isGray, script.m_isDebug);
			match.m_isLog = m_isLog;
			//isDetect = match.Match((*src)(roi), *sceneKeyPoints, *sceneDescriptor, matObj, 
			//	*objectKeyPoints, *objectDescriotor, node->str);

			// TODO: feature match roi
			isDetect = match.Match(*src, *sceneKeyPoints, *sceneDescriptor, matObj,
				*objectKeyPoints, *objectDescriotor, node->name);
			max = isDetect ? 1 : 0;

			// 매칭 결과 저장
			arg.matchResult->m_data[node->id].max = max;
			if (match.m_rect.m_contours.size() >= 4)
			{
				arg.matchResult->m_data[node->id].matchRect2[0] = match.m_rect.m_contours[0];
				arg.matchResult->m_data[node->id].matchRect2[1] = match.m_rect.m_contours[1];
				arg.matchResult->m_data[node->id].matchRect2[2] = match.m_rect.m_contours[2];
				arg.matchResult->m_data[node->id].matchRect2[3] = match.m_rect.m_contours[3];
			}
		}
	}

	// 성공이든, 실패든, 매칭된 위치는 저장한다.
	node->matchLoc = left_top + Point(roi.x, roi.y);

	if (isDetect)
	{
		return 1;
	}

	return 0;
}


int cMatchProcessor::executeOcr(INOUT sExecuteTreeArg &arg)
{
	//cMatchScript2 &script = *arg.matchResult->m_script;
	const cv::Mat &input = arg.matchResult->m_input;
	const string &inputName = arg.matchResult->m_inputName;
	const int inputImageId = arg.matchResult->m_inputImageId;
	//sParseTree *parent = arg.parent;
	sParseTree *node = arg.node;
	//cv::Mat *out = arg.out;

	const Mat *src = &input;

	if (!node->IsEmptyBgr())
	{
		src = &loadScalarImage(inputName, inputImageId, Scalar(node->scalar[0], node->scalar[1], node->scalar[2]), node->scale); // BGR
	}

	if (!node->IsEmptyHsv())
	{
		src = &loadHsvImage(inputName, inputImageId, Scalar(node->hsv[0], node->hsv[1], node->hsv[2]), Scalar(node->hsv[3], node->hsv[4], node->hsv[5]));
	}

	cDeSkew deSkew;
	Mat dst = src->clone();
	deSkew.DeSkew(dst, 0.005f, 0, true);

	float maxFitness = -FLT_MAX;
	tess::cTessWrapper *tess = GetTesseract();
	dbg::Log("ocr crash check1 \n");
	const string srcStr = tess->Recognize(deSkew.m_tessImg);
	dbg::Log("ocr crash check2 \n");
	const string result = tess->Dictionary(node->name, srcStr, maxFitness);
	dbg::Log("ocr crash check3 \n");

	bool isDetect = true;

	// 스트링 테이블이 있다면, 스트링 테이블에 포함된 문자일 때만,
	// 매칭에 성공한 것을 간주한다.
	if (node->table)
	{
		isDetect = false;
		for each (auto &str in *node->table)
		{
			if (str == result)
			{
				isDetect = true;
				break;
			}
		}
	}

	arg.matchResult->m_matchCount++;
	node->processCnt++; // count node match
	arg.matchResult->m_data[node->id].max = 0;

	if (isDetect)
	{
	 	arg.matchResult->m_data[node->id].max = maxFitness;

		// 인식한 문장 저장
		const int MAX_STR_LEN = sizeof(arg.matchResult->m_data[node->id].str);
		ZeroMemory(arg.matchResult->m_data[node->id].str, MAX_STR_LEN);
		memcpy(arg.matchResult->m_data[node->id].str, result.c_str(), MIN(result.length(), MAX_STR_LEN - 1));

		if (m_isLog2)
			dbg::Log("executeOcr detect src=%s, result=%s, maxFitness=%3.3f \n", srcStr.c_str(), result.c_str(), maxFitness);
	}
	else
	{
		if (m_isLog2)
			dbg::Log("executeOcr not detect src = %s, result=%s, maxFitness=%3.3f \n", srcStr.c_str(), result.c_str(), maxFitness);
	}

	// 성공이든, 실패든, 매칭된 위치는 저장한다.
	node->matchLoc = deSkew.m_deSkewPoint1;
	arg.matchResult->m_data[node->id].matchRect2[0] = deSkew.m_pts[0];
	arg.matchResult->m_data[node->id].matchRect2[1] = deSkew.m_pts[1];
	arg.matchResult->m_data[node->id].matchRect2[2] = deSkew.m_pts[2];
	arg.matchResult->m_data[node->id].matchRect2[3] = deSkew.m_pts[3];

	return isDetect;
}


Mat& cMatchProcessor::loadImage(const string &fileName)
{
	AutoCSLock cs(m_loadImgCS);

	auto it = m_imgTable.find(fileName);
	if (m_imgTable.end() != it)
	{
		if (it->second)
			return *it->second;
	}

	Mat *img = new Mat();
	*img = imread(fileName);
	if (!img->data)
	{
		dbg::ErrLog("Error!! loadImage name=%s\n", fileName.c_str());
		return m_nullMat;
	}

	if (m_isGray)
		cvtColor(*img, *img, CV_BGR2GRAY);
	m_imgTable[fileName] = img;
	return *img;
}


cv::Mat& cMatchProcessor::loadScalarImage(const string &fileName, const int imageId, const cv::Scalar &scalar, const float scale)
{
	const __int64 key = ((__int64)imageId << 32) |
		((__int64)scalar[0]) << 24 | ((__int64)scalar[1]) << 16 | ((__int64)scalar[2]) << 8 | ((__int64)(scale * 10));

	AutoCSLock cs(m_loadScalarImgCS);
	auto it = m_scalarImgTable.find(key);
	if (m_scalarImgTable.end() != it)
		return *it->second;

	Mat &src = loadImage(string("color") + fileName);
	if (!src.data)
		return m_nullMat;

	Mat *mat = new Mat();
	*mat = src.clone() & scalar;
	if (m_isGray)
	{
		cvtColor(*mat, *mat, CV_BGR2GRAY);
		if (scale > 0)
			*mat *= scale;
	}

	m_scalarImgTable[key] = mat;
	return *mat;
}


Mat& cMatchProcessor::loadHsvImage(const string &fileName, const int imageId, const cv::Scalar &hsv1, const cv::Scalar &hsv2)
{
	// make key
	const __int64 key = ((__int64)hsv1[0]) << 16 | ((__int64)hsv1[1]) << 8 | ((__int64)hsv1[2]) |
		((__int64)hsv2[0]) << 40 | ((__int64)hsv2[1]) << 32 | ((__int64)hsv2[2]) << 24 |
		((__int64)imageId << 48);

	AutoCSLock cs(m_loadHsvImgCS);
	auto it = m_hsvImgTable.find(key);
	if (m_hsvImgTable.end() != it)
		return *it->second;

	Mat &src = loadImage(string("color") + fileName);
	if (!src.data)
		return m_nullMat;

	Mat *mat = new Mat();
	*mat = src.clone();
	cvtColor(src, *mat, CV_BGR2HSV);
	inRange(*mat, hsv1, hsv2, *mat);
	//GaussianBlur(*mat, *mat, cv::Size(9, 9), 2, 2);

	m_hsvImgTable[key] = mat;
	return *mat;
}


void cMatchProcessor::loadDescriptor(const string &fileName, OUT vector<KeyPoint> **keyPoints, OUT Mat **descriptor)
{
	if (fileName.empty())
	{
		*keyPoints = NULL;
		*descriptor = NULL;
		return;
	}

	{
		AutoCSLock cs(m_loadDescriptCS); // 소멸자 호출을 위한 지역변수
		auto it = m_keyPointsTable.find(fileName);
		if (m_keyPointsTable.end() != it)
		{
			*keyPoints = it->second;
			*descriptor = m_descriptorTable[fileName];
			return;
		}
	}

	const Mat &src = loadImage(fileName);
	loadDescriptor(fileName, src, keyPoints, descriptor);
}


void cMatchProcessor::loadDescriptor(const string &keyName, const cv::Mat &img,
	OUT vector<KeyPoint> **keyPoints, OUT Mat **descriptor, const bool isSearch) //isSearch=true
{
	if (keyName.empty())
	{
		*keyPoints = NULL;
		*descriptor = NULL;
		return;
	}

	AutoCSLock cs(m_loadDescriptCS);

	if (isSearch)
	{
		auto it = m_keyPointsTable.find(keyName);
		if (m_keyPointsTable.end() != it)
		{
			*keyPoints = it->second;
			*descriptor = m_descriptorTable[keyName];
			return; // find, and return
		}
	}

	// not found, Compute keyPoints, Descriptor
	vector<KeyPoint> *keyPts = new vector<KeyPoint>();
	Mat *descr = new Mat();
	m_detector->detectAndCompute(img, Mat(), *keyPts, *descr);

	delete m_keyPointsTable[keyName];
	m_keyPointsTable[keyName] = keyPts;
	delete m_descriptorTable[keyName];
	m_descriptorTable[keyName] = descr;

	*keyPoints = keyPts;
	*descriptor = descr;
}


// 스레드가 실행 중이라면, 강제 종료시킨다.
void cMatchProcessor::FinishMatchThread()
{
	if (m_threadHandle)
	{
		m_isThreadTerminate = true;
		WaitForSingleObject(m_threadHandle, INFINITE); // 5 seconds wait
		m_threadHandle = NULL;
		m_isMatchThreadLoop = false;
	}
}


// 입력 영상을 등록한다.
// 컬러입력 영상일 경우, 흑백 영상으로 저장하고, color+name 으로 원본 영상을 저장한다.
void cMatchProcessor::SetInputImage(const cv::Mat &img, const string &name) // name=@input
{
	const string colorName = string("color") + name;

	{
		AutoCSLock cs(m_loadImgCS); // 소멸자 호출을 위한 지역변수

		delete m_imgTable[name];
		delete m_imgTable[colorName];

		// 1 channel
		if (img.channels() == 1) // Gray
		{
			m_imgTable[name] = new Mat();
			*m_imgTable[name] = img.clone(); // register input image
		}
		else if (img.channels() >= 3) // BGR -> Gray
		{
			m_imgTable[name] = new Mat();
			m_imgTable[colorName] = new Mat();

			*m_imgTable[colorName] = img.clone(); // source color image
			cvtColor(img, *m_imgTable[name], CV_BGR2GRAY);
		}
	}

	{
		AutoCSLock cs(m_loadDescriptCS); // 소멸자 호출을 위한 지역변수

		delete m_keyPointsTable[name];
		m_keyPointsTable.erase(name);
		delete m_descriptorTable[name];
		m_descriptorTable.erase(name);

		delete m_keyPointsTable[colorName];
		m_keyPointsTable.erase(colorName);
		delete m_descriptorTable[colorName];
		m_descriptorTable.erase(colorName);
	}
}


void cMatchProcessor::RemoveInputImage(const string &name, const int imageId)
{
	{
		AutoCSLock cs(m_loadImgCS); // 소멸자 호출을 위한 지역변수

		auto it = m_imgTable.find(name);
		if (it != m_imgTable.end())
		{
			delete it->second;
			m_imgTable.erase(it);
		}

		const string colorName = string("color") + name;
		auto it2 = m_imgTable.find(colorName);
		if (it2 != m_imgTable.end())
		{
			delete it2->second;
			m_imgTable.erase(it2);
		}
	}

	{
		AutoCSLock cs(m_loadDescriptCS); // 소멸자 호출을 위한 지역 변수

		delete m_keyPointsTable[name];
		m_keyPointsTable.erase(name);

		delete m_descriptorTable[name];
		m_descriptorTable.erase(name);
	}

	RemoveScalarImage(imageId);
	RemoveHsvImage(imageId);
}


void cMatchProcessor::RemoveScalarImage(const int imageId)
{
	AutoCSLock cs(m_loadScalarImgCS);

	auto it = m_scalarImgTable.begin();
	while (m_scalarImgTable.end() != it)
	{
		const int id = (int)(it->first >> 32);
		if (id == imageId)
		{
			delete it->second;
			it = m_scalarImgTable.erase(it++);
		}
		else
		{
			++it;
		}
	}
}


void cMatchProcessor::RemoveHsvImage(const int imageId)
{
	AutoCSLock cs(m_loadHsvImgCS);

	auto it = m_hsvImgTable.begin();
	while (m_hsvImgTable.end() != it)
	{
		const int id = (int)(it->first >> 48);
		if (id == imageId)
		{
			delete it->second;
			m_hsvImgTable.erase(it++);
		}
		else
		{
			++it;
		}
	}
}


void cMatchProcessor::Clear()
{
	{
		AutoCSLock cs(m_loadImgCS); // 소멸자 호출을 위한 지역변수

		for each (auto kv in m_imgTable)
			delete kv.second;
		m_imgTable.clear();
	}

	{
		AutoCSLock cs(m_loadDescriptCS); // 소멸자 호출을 위한 지역변수

		for each (auto it in m_keyPointsTable)
			delete it.second;
		m_keyPointsTable.clear();

		for each (auto it in m_descriptorTable)
			delete it.second;
		m_descriptorTable.clear();
	}

	{
		AutoCSLock cs(m_loadScalarImgCS);
		for each (auto it in m_scalarImgTable)
			delete it.second;
		m_scalarImgTable.clear();
	}

	{
		AutoCSLock cs(m_loadHsvImgCS);
		for each (auto it in m_hsvImgTable)
			delete it.second;
		m_hsvImgTable.clear();
	}

	for each (auto mr in m_matchResults)
		delete mr.p;
	m_matchResults.clear();

	for each (auto p in m_tess)
		delete p;
	m_tess.clear();
}


// MatchResult 를 생성해서 리턴한다.
cMatchResult* cMatchProcessor::AllocMatchResult()
{
	for each (auto mr in m_matchResults)
	{
		if (!mr.used)
		{
			mr.used = true;
			return mr.p;
		}
	}

	cMatchResult *p = new cMatchResult;
	m_matchResults.push_back({ true, p });
	return p;
}


void cMatchProcessor::FreeMatchResult(cMatchResult *p)
{
	for each (auto mr in m_matchResults)
	{
		if (mr.p == p)
		{
			mr.used = false;
			return;
		}
	}

	dbg::ErrLog("cMatchProcessor::FreeMatchResult() Not Found pointer \n");
}


tess::cTessWrapper* cMatchProcessor::GetTesseract()
{
	AutoCSLock cs(m_tessCS);

	tess::cTessWrapper *p = m_tess[m_tessIdx++];
	if ((int)m_tess.size() <= m_tessIdx)
		m_tessIdx = 0;

	return p;
}
