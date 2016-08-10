
#include "stdafx.h"
#include "MatchResult.h"

using namespace cv;
using namespace cvproc;
using namespace cvproc::imagematch;


cMatchResult::cMatchResult()
{
	Clear();
}

cMatchResult::~cMatchResult()
{
}


void cMatchResult::Init(cMatchScript2 *p, const cv::Mat &input, const string &inputName,
	const int inputImageId, sParseTree *labelTree, const bool isRegisterInput, const bool isBlockMode)
{
	Clear();

	m_script = p;
	m_isRun = true;
	m_isTerminate = false;
	m_input = input.clone();
	m_srcImage = input.clone();
	m_inputName = inputName;
	m_inputImageId = inputImageId;
	m_registerInput = isRegisterInput;
	m_removeInput = isRegisterInput;
	m_blockMode = isBlockMode;
	m_nodeLabel = labelTree;
	m_beginTime = timeGetTime();
}


void cMatchResult::Clear()
{
	m_traverseType = 0;
	m_script = NULL;
	m_input = Mat();
 	m_srcImage = Mat();
 	m_inputName.clear();
	m_inputImageId = -1;
	m_registerInput = true;
	m_blockMode = false;
	m_removeInput = false;
	m_nodeLabel = NULL;
	m_resultStr.clear();
	m_isRun = false;
	m_isTerminate = false;
	m_isEnd = false;
	m_result = 0;
	m_matchCount = 0;
	m_beginTime = 0;

	ZeroMemory(m_data, sizeof(m_data));
}


void cMatchResult::TerminateMatch()
{
	m_isTerminate = true;
}


void cMatchResult::MatchComplete()
{
	m_isRun = false;
	m_isEnd = false;
	m_isTerminate = false;
}
