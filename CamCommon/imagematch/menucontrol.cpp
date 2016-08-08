
#include "stdafx.h"
#include "menucontrol.h"

using namespace cvproc;
using namespace cvproc::imagematch;


cMenuControl::cMenuControl()
	: m_state(WAIT)
	, m_matchScript(NULL)
	, m_screenCaptureKey(VK_SNAPSHOT)
	, m_currentCmdIdx(-1)
	, m_currentNode(NULL)
{
}

cMenuControl::~cMenuControl()
{
}


bool cMenuControl::Init( cMatchScript2 *matchScript, 
	const string &menuControlScriptFileName, 
	const int screenCaptureKey) // screenCaptureKey = VK_SNAPSHOT
{
	Cancel();

	m_matchScript = matchScript;

	if (!m_menuScript.Read(menuControlScriptFileName))
	{
		dbg::ErrLog("Error!! cMenuControl::Init(), read error menu script \n");
		return false;
	}

	m_screenCaptureKey = screenCaptureKey;

	return true;
}


cMenuControl::STATE cMenuControl::Update(const float deltaSeconds, const cv::Mat &img, OUT int &key)
{
	STATE nextState = m_state;

	switch (m_state)
	{
	case WAIT:
		break;

	case DELAY:
		nextState = OnDelay(deltaSeconds, img, key);
		break;

	case REACH:
		break;

	case CAPTURE:
		nextState = OnCapture(img, key);
		break;

	case PROC:
		nextState = OnProc(img, key);
		break;

	case MENU_MOVE:
		nextState = OnMenu(img, key);
		break;

	case MENU_DETECT:
		nextState = OnMenuDetect(img, key);
		break;

	case ERR:
		break;

	default:
		assert(0);
		break;
	}

	m_state = nextState;

	return nextState;
}


cMenuControl::STATE cMenuControl::OnDelay(const float deltaSeconds, const cv::Mat &img, OUT int &key)
{
	// 한프레임을 스킵하고, 다음 프레임 부터 시작
	// 매칭 계산 때문에, deltaSeconds 값이 큰 경우를 넘어가기 위한 코드.
	if (m_isInitDelay)
	{
		m_isInitDelay = false;
		return DELAY;
	}

	m_delayTime -= deltaSeconds;
	if (m_delayTime < 0)
	{
		return m_nextState;
	}

	return DELAY;
}


cMenuControl::STATE cMenuControl::OnCapture(const cv::Mat &img, OUT int &key)
{
	key = m_screenCaptureKey;
	return PROC;
}


cMenuControl::STATE cMenuControl::OnProc(const cv::Mat &img, OUT int &key)
{
	if (!img.data)
		return PROC;

	cMatchResult *mr = cMatchProcessor::Get()->AllocMatchResult();
	mr->Init(m_matchScript, img, "", 0,
		(sParseTree*)m_matchScript->FindTreeLabel(m_headName),
		true, true);
	cMatchProcessor::Get()->Match(*mr);

	const string result = mr->m_resultStr;
	cMatchProcessor::Get()->FreeMatchResult(mr);

	if (!m_currentNode)
	{
		// 현재 위치 파악

	}

	return m_state;
}


cMenuControl::STATE cMenuControl::OnMenu(const cv::Mat &img, OUT int &key)
{
	return m_state;
}


cMenuControl::STATE cMenuControl::OnMenuDetect(const cv::Mat &img, OUT int &key)
{
	return m_state;
}


// {category1 category2 item}
bool cMenuControl::StartMenuSetting(const string &menuHeadName, const string &cmd)
{
	m_headName = menuHeadName;
	m_currentCmdIdx = -1;
	m_cmds.clear();

	vector<string> lines;
	tokenizer(cmd, "\n", "", lines);
	for each (auto &line in lines)
		m_cmds.push_back(line);

	if (m_cmds.empty())
		return false;

	m_currentCmdIdx = 0;
	return true;
}


void cMenuControl::Cancel()
{
	m_currentCmdIdx = -1;
	m_cmds.clear();
}
