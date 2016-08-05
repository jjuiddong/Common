
#include "stdafx.h"
#include "flowcontrol.h"
#include "matchprocessor.h"

using namespace cvproc;
using namespace cvproc::imagematch;

cFlowControl::cFlowControl()
	: m_state(WAIT)
	, m_currentNode(NULL)
	, m_nextNode(NULL)
	, m_delayTime(0)
	, m_isInitDelay(false)
	, m_initDelayTime(0)
	, m_nextMenuIdx(0)
	, m_currentMenuIdx(0)
	, m_screenCaptureKey(VK_SNAPSHOT)
	, m_genId(100000)
	, m_matchResultBuffIdx(0)
	, m_isLog(false)
{
}

cFlowControl::~cFlowControl()
{
}


bool cFlowControl::Init(const string &imageMatchScriptFileName, 
	const string &flowScriptFileName
	, const int screenCaptureKey) // screenCaptureKey=VK_SNAPSHOT
{
	Cancel();

	cMatchProcessor::Get()->m_isLog = false;
	if (!m_matchScript.Read(imageMatchScriptFileName))
	{
		dbg::ErrLog("Error!! cFlowControl::Init(), read error image match script \n");
		return false;
	}

	if (!m_flowScript.Read(flowScriptFileName))
	{
		dbg::ErrLog("Error!! cFlowControl::Init(), read error menu script \n");
		return false;
	}

	m_screenCaptureKey = screenCaptureKey;

	return true;
}


// 매 루프때마다 호출 되어야 한다.
// 
cFlowControl::STATE cFlowControl::Update(const float deltaSeconds, const cv::Mat &img, OUT int &key)
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


cFlowControl::STATE cFlowControl::OnDelay(const float deltaSeconds, const cv::Mat &img, OUT int &key)
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


cFlowControl::STATE cFlowControl::OnCapture(const cv::Mat &img, OUT int &key)
{
	key = m_screenCaptureKey;
	return PROC;
}


// 이미지 영상 처리
// img 를 인식해, 현재 씬을 판단하고, 목표 씬까지 이동한다.
cFlowControl::STATE cFlowControl::OnProc(const cv::Mat &img, OUT int &key)
{
	if (!img.data)
		return PROC;

	if (m_nextNode && m_nextNode->noProc)
	{
		// 씬 매칭을 하지 않고, 키보드 엔터를 누른 후, 
		// 다음 씬으로 넘어간다.
		m_currentNode = m_nextNode;

		// 목표 위치까지 경로 탐색
		m_path.clear();
		if (m_flowScript.FindRoute(m_nextNode, m_moveTo, m_path))
		{
			if (m_path.empty() || (m_path.size() == 1))
			{
				// 이미 도착한 상태
				return REACH;
			}

			// 다음 노드를 선택한다.
			if (m_path.size() > 1)
			{
				m_nextNode = m_path[1];
			}
		}
		else
		{
			dbg::ErrLog("Error!! cFlowControl::OnProc(), not found route [%s] \n", m_moveTo.c_str());
			return ERR;
		}

		key = (m_nextNode->key)? m_nextNode->key : VK_RETURN;
		Delay(m_currentNode->delay, CAPTURE);
		return m_state;
	}


	const int t1 = timeGetTime();

	sTreeMatchData data;
	data.node = NULL;
	data.loopCnt = 0;

	if (m_nextNode)
	{
		TreeMatch(m_nextNode, img.clone(), data, 1);
	}
	else
	{
		cMatchResult &matchResult = GetMatchResult();
		matchResult.Init(&m_matchScript, &img, "", 0,
			(sParseTree*)m_matchScript.FindTreeLabel("@detect_scene"),
			true, true);
		cMatchProcessor::Get()->Match(matchResult);
		
		data.result = matchResult.m_resultStr;
		data.node = m_flowScript.Find(matchResult.m_resultStr);

 		if (m_isLog)
 			dbg::Log("cFlowControl::OnProc result=%s, loop=%d, time=%d\n", data.result.c_str(), 
				data.loopCnt, timeGetTime()-t1);
	} 

	// 인식에 실패하면, 다시 영상을 인식한다.
	if (!data.node)
	{
		++m_tryMachingCount;
		if (m_tryMachingCount > 2)
		{
			m_tryMachingCount = 0;
			m_nextNode = NULL; // 전체 씬을 매칭한다.
		}

		key = m_screenCaptureKey;
		return PROC;
	}

	m_currentNode = data.node;
	m_nextNode = NULL;
	m_tryMachingCount = 0;

	// 목표 위치까지 경로 탐색
	m_path.clear();
	if (m_flowScript.FindRoute(data.node, m_moveTo, m_path))
	{
		if (m_path.empty() || (m_path.size()==1))
		{
			// 이미 도착한 상태
			return REACH;
		}

		// 다음 노드를 선택한다.
		if (m_path.size() > 1)
		{
			m_nextNode = m_path[1];
		}
	}
	else
	{
		dbg::ErrLog("Error!! cFlowControl::OnProc(), not found route [%s] \n", m_moveTo.c_str());
		return ERR;
	}

	// 다음 씬까지 이동을 위한 키 선택
	const bool IsUpMenu = CheckMenuUpandDown(m_path[0], m_path[1]);

	// goto parent menu
	if (IsUpMenu)
	{
		key = VK_ESCAPE;
		Delay(3.f, CAPTURE);
		return m_state;
	}

	// 씬에 있는 메뉴가 하나 이상일 경우, 메뉴를 선택하는 상태로 넘어간다.
 	m_currentMenuIdx = 0;
 	m_nextMenuIdx = 0;
	key = 0;

	const int nextMenuCount = GetNextMenuCount(m_path[0], m_path[1]);
	const int nextMenuIdx = CheckNextMenuIndex(m_path[0], m_path[1]);
	if (nextMenuIdx < 0) // next parent menu
	{
		key = VK_ESCAPE;
		Delay(3.f, CAPTURE);
	}
	else if (nextMenuCount == 1) // 다음 메뉴가 하나밖에 없는 경우..
	{
		key = (m_nextNode->key) ? m_nextNode->key : VK_RETURN;
		Delay(m_currentNode->delay, CAPTURE);
	}
	else if (nextMenuIdx >= 0)
	{
		return OnMenuDetect(img, key);
	}

	return m_state;
}


cFlowControl::STATE cFlowControl::OnMenu(const cv::Mat &img, OUT int &key)
{
	switch (m_state)
	{
	case MENU_MOVE:
		if (m_currentMenuIdx == m_nextMenuIdx)
		{
			key = VK_RETURN;
			Delay(m_nextNode->delay, CAPTURE);
		}
		else
		{
			if (m_currentMenuIdx < m_nextMenuIdx)
			{
				++m_currentMenuIdx;
				key = (m_currentNode->isSideMenu)? VK_RIGHT : VK_DOWN;
			}
			else
			{
				--m_currentMenuIdx;
				key = (m_currentNode->isSideMenu)? VK_LEFT : VK_UP;
			}

			Delay(0.3f, MENU_MOVE);
		}
		break;

	case MENU_DETECT:
	case WAIT:
	case DELAY:
	case REACH:
	case CAPTURE:
	case PROC:
	case ERR:
		break;

	default: 
		assert(0); 
		break;
	}

	return m_state;
}


// 현재 메뉴의 위치를 확인하고, 목표 메뉴로 이동한다.
// Left, Right Button or Up, Down Button
cFlowControl::STATE cFlowControl::OnMenuDetect(const cv::Mat &img, OUT int &key)
{
	cGraphScript::sNode *node = m_currentNode;
	if (!node)
		node = m_flowScript.m_root;

	cMatchResult &matchResult = GetMatchResult();
	const string label = node->tag.empty() ? node->id : node->tag;
	matchResult.Init(&m_matchScript, &img, "", 0,
		(sParseTree*)m_matchScript.FindTreeLabel(string("@") + label + "_menu"),
		true, true);
	matchResult.m_traverseType = 1; // search all

	cMatchProcessor::Get()->Match(matchResult);

	if (m_isLog)
		dbg::Log("cFlowControl::OnMenuDetect label=%s, result=%s\n", label.c_str(), matchResult.m_resultStr.c_str());

	if (matchResult.m_result <= 0)
	{
		// 인식에 실패하면 다시 시도한다
		return CAPTURE;
	}

	const string selectMenuId = matchResult.m_resultStr;
	if (selectMenuId == m_nextNode->id)
	{
		// 다음으로 넘어갈 메뉴를 선택하고 있는 상태
		// 엔터키를 눌러 다음 씬으로 넘어간다.
		key = VK_RETURN;
		Delay(m_nextNode->delay, CAPTURE);
	}
	else
	{
		m_nextMenuIdx = CheckNextMenuIndex(m_path[0], selectMenuId, m_path[1]);
		m_currentMenuIdx = 0;

		if (m_nextMenuIdx == 0)
		{
			// 도착, 현재 위치를 다시 파악한 후, 종료한다.
			return CAPTURE;
		}

		// m_nextMenuIdx 만큼 메뉴를 이동한다.
		return MENU_MOVE;
	}

	return m_state;
}


void cFlowControl::Delay(const float delaySeconds, const STATE nextState)
{
	m_isInitDelay = true;
	m_initDelayTime = delaySeconds;
	m_delayTime = delaySeconds;
	m_nextState = nextState;
	m_state = DELAY;
}


// 씬 이동 명령이 오면, 영상을 캡쳐해, 현재 위치 파악을 먼저 한다.
void cFlowControl::Move(const string &sceneName)
{
	Cancel();

	if (!sceneName.empty())
	{
		m_moveTo = sceneName;
		m_state = CAPTURE;
	}
}


// 메뉴 이동 중지
void cFlowControl::Cancel()
{
	m_state = WAIT;
	m_currentNode = NULL;
	m_nextNode = NULL;
	m_nextMenuIdx = 0;
	m_currentMenuIdx = 0;
	m_tryMachingCount = 0;
	m_path.clear();
	m_moveTo.clear();
}


string cFlowControl::GenerateInputID()
{
	stringstream ss;
	ss << "@input" << m_genId++;
	if (m_genId > 100000)
		m_genId = 0;
	return ss.str();
}


cMatchResult& cFlowControl::GetMatchResult()
{
	const int MAX_LEN = sizeof(m_matchResult) / sizeof(cMatchResult);

	++m_matchResultBuffIdx;
	if (m_matchResultBuffIdx >= MAX_LEN)
		m_matchResultBuffIdx = 0;
	return m_matchResult[m_matchResultBuffIdx];
}


bool cFlowControl::TreeMatch(cGraphScript::sNode *current, const cv::Mat &img, 
	OUT sTreeMatchData &out, const int loopCnt) // loopCnt=-1
{
	RETV(!current, false);

	// 중복 순회를 막기위해 쓰인다.
	m_flowScript.CheckClearAllNode();

	queue<cGraphScript::sNode*> q;
	q.push(current);

	const string inputName = GenerateInputID();

	vector<sMatchInfo> matchInfo;
	cMatchProcessor::Get()->SetInputImage(img, inputName);

	// breath first search
	int loop = 0;
	string result;
	while (!q.empty() && ((loopCnt==-1) || (loop < loopCnt)))
	{
		cGraphScript::sNode *node = q.front();
		q.pop();

		if (node->check) // 이미 한번 계산했으면, 넘어간다.
			continue;

		cMatchResult &matchResult = GetMatchResult();
		const string label = node->tag.empty() ? node->id : node->tag;
		matchResult.Init(&m_matchScript, &img, inputName, 0,
			(sParseTree*)m_matchScript.FindTreeLabel(string("@") + label),
			true, true);

		if (m_isLog)
			dbg::Log("cFlowControl::TreeMatch label = %s\n", label.c_str());

		cMatchProcessor::Get()->Match(matchResult);
		result = matchResult.m_resultStr;

		if (m_isLog)
			dbg::Log("    - result = %s \n", result.c_str());

		if ((result != "~ fail ~") && (result != "not found label"))
		{
			out.node = node;
			out.result = result;
			out.loopCnt = loop;
			return true; // success find~ return
		}

		node->check = true;

		// 아래를 먼저 삽입해서, 먼저 검사하게 한다. (위로 올라갈수록 속도가 느림, main, option)
		for each (auto p in node->out)
		{
			if (!p->check)
				q.push(p);
		}

		if (loopCnt == -1) // 전체를 검사할 때만, 위로 올라가서 검사한다.
		{
			for each (auto p in node->in)
			{
				if (!p->check)
					q.push(p);
			}
		}

		if (result != "not found label") // 라벨이 없는 것은 검사한 것으로 간주하지 않는다.
			++loop;
	}

	out.loopCnt = loop;
	return false;
}


// 상위 메뉴로 올라가면, true 를 리턴하고,
// 하위 메뉴로 내려가면, false를 리턴한다.
bool cFlowControl::CheckMenuUpandDown(
	const cGraphScript::sNode *current, 
	const cGraphScript::sNode *next)
{
	for each (auto p in current->out)
	{
		if (p == next)
			return false;
	}

	for each (auto p in current->in)
	{
		if (p == next)
			return true;
	}

	return true;
}


// 현재 노드에서 다음노드로 넘어갈 때 선택해야 할, 분기 메뉴 갯수를 리턴한다.
int cFlowControl::GetNextMenuCount(
	const cGraphScript::sNode *current, 
	const cGraphScript::sNode *next)
{
	// menu down
	bool isMenuDown = false;
	int cnt1 = 0;
	for each (auto p in current->out)
	{
		if (p == next)
			isMenuDown = true;
		if (!p->isAuto)
			++cnt1;
	}
	if (isMenuDown)
	{
		if (current->isEnterChild) // 엔터키로 다음 씬으로 넘어갈 때는, 메뉴가 하나만 있는 것처럼 작동하게 한다.
			return 1;
		return cnt1;
	}

	// menu up
	bool isMenuUp = false;
	int cnt2 = 0;
	for each (auto p in current->in)
	{
		if (p == next)
			isMenuUp = true;
		if (!p->isAuto)
			++cnt2;
	}
	if (isMenuUp)
		return cnt2;

	return 0;
}


// node 
// 음수이면, 아래에서 위로 올라가거나, 오른쪽에서 왼쪽으로 이동하는 메뉴를 뜻한다.
int cFlowControl::CheckNextMenuIndex(const cGraphScript::sNode *current, 
	const cGraphScript::sNode *next)
{
	RETV(!current || !next, 0);

	int cnt1 = 0;
	for each (auto p in current->out)
	{
		if (p == next)
		{
			return cnt1;
		}
		++cnt1;
	}

	int cnt2 = 0;
	for each (auto p in current->in)
	{
		if (p == next)
		{
			return cnt2;
		}
		--cnt2;
	}

	return 0;
}


// 음수이면, 아래에서 위로 올라가거나, 오른쪽에서 왼쪽으로 이동하는 메뉴를 뜻한다.
int cFlowControl::CheckNextMenuIndex(
	const cGraphScript::sNode *current,
	const string &selectMenuId,
	const cGraphScript::sNode *next)
{
	RETV(!current || !next, 0);

	int targetMenuIndex = 0;
	for each (auto p in current->out)
	{
		if (p == next)
			break;
		++targetMenuIndex;
	}

	int currentMenuIndex = 0;
	for each (auto p in current->out)
	{
		if (p->id == selectMenuId)
			break;
		++currentMenuIndex;
	}

	return targetMenuIndex - currentMenuIndex;
}

