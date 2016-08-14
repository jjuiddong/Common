
#include "stdafx.h"
#include "matchthread.h"
#include "matchprocessor.h"

using namespace cv;
using namespace cvproc;
using namespace cvproc::imagematch;


unsigned __stdcall MatchThreadSub(void *ptr)
{
	cMatchProcessor::sExecuteTreeArg *arg = (cMatchProcessor::sExecuteTreeArg*)ptr;
	arg->node->isRun = true;
	arg->matchResult->m_data[arg->node->id].nodesRun = true;

	arg->result = (char)cMatchProcessor::Get()->executeTreeEx(*arg);

	arg->matchResult->m_data[arg->node->id].nodesRun = false;
	arg->matchResult->m_data[arg->node->id].result = arg->result;
	arg->node->isRun = false;
	arg->isEnd = true;

	return 0;
}


const sParseTree* FindMostFitnessNode(const cMatchResult &matchResult, OUT string &resultStr);


// multi thread, image match
unsigned __stdcall MatchThreadMain(void *ptr)
{
	cMatchResult *matchResult = (cMatchResult*)ptr;

	if (cMatchProcessor::Get()->m_isLog)
		dbg::Log("cMatchScript2::ExecuteEx Start >>\n");

	const Mat &input = matchResult->m_input;
	const string inputName = matchResult->m_inputName;
	const int inputImageId = matchResult->m_inputImageId;
	const bool isRemoveInput = matchResult->m_removeInput;
	cMatchScript2 *matchScript = matchResult->m_script;

	matchResult->m_result = 0;
	matchResult->m_resultStr = "~ fail ~";

	matchScript->clearResultTree();
	char nodeResults[1024]; // maximum 1024 nodes, -1:not visit, 0:fail, 1:success, 2:final success
	memset(nodeResults, -1, sizeof(nodeResults));

	// find node correspond to label
	sParseTree *nodeLabel = matchResult->m_nodeLabel;

	if (!nodeLabel->child)
	{
		matchResult->m_resultStr = "not found label";
		matchResult->m_result = -2;
		matchResult->m_isEnd = true;
		return -2; // label node is empty
	}

	const int MAX_CORE = 9;
	HANDLE handles[MAX_CORE];
	cMatchProcessor::sExecuteTreeArg args[MAX_CORE];
	ZeroMemory(handles, sizeof(handles));
	ZeroMemory(args, sizeof(args));

	typedef pair<sParseTree*, sParseTree*> ItemType; //<parent node, child node>
	vector< ItemType > q; // work like queue
	q.reserve(8);
	q.push_back(ItemType(NULL, nodeLabel->child));
	nodeLabel->processCnt = 1;
	nodeLabel->result = 1;
	nodeLabel->child->processCnt = 1;
	nodeLabel->child->result = 1;

	int coreCnt = 0;
	bool isThreadWork = true; // 모든 쓰레드가 끝나야, 루프가 종료된다.
	while ((!q.empty() || isThreadWork) && !matchResult->m_isTerminate)
	{
		isThreadWork = false;
		int emptyIdx = -1;

		for (int i = 0; i < MAX_CORE; ++i)
		{
			if (args[i].isUsed)
			{
				isThreadWork = true; // 동작하는 쓰레드가 있음
				break;
			}
		}

		// find empty thread
		for (int i = 0; i < MAX_CORE; ++i)
		{
			if (!args[i].isUsed)
			{
				emptyIdx = i;
				break;
			}
		}

		// find finish thread
		int endIdx = -1;
		for (int i = 0; i < MAX_CORE; ++i)
		{
			if (args[i].isUsed && args[i].isEnd)
			{
				endIdx = i;
				break;
			}
		}

		if ((emptyIdx < 0) && (endIdx < 0))
		{
			Sleep(1); // 모든 쓰레드가 동작 중이라면, 잠깐 쉬고, 다시 계산한다.
			continue;
		}

		// finish thread 처리
		if (endIdx >= 0)
		{
			sParseTree *node = args[endIdx].node;
			// 초기화
			args[endIdx].isUsed = false;
			args[endIdx].isEnd = false;
			handles[endIdx] = NULL;
			node->result = args[endIdx].result;
			nodeResults[node->id] = args[endIdx].result;

			if (args[endIdx].result > 0)
			{
				sParseTree *child = node->child;
				if (!child) // success!! terminal node, end fuction
				{
					node->result = 2;
					nodeResults[node->id] = 2;
					matchResult->m_result = 1;
					matchResult->m_resultStr = node->name;

					if (matchResult->m_traverseType == 0) // if match success return
						break; // success, loop terminate
					if (matchResult->m_data[node->id].max >= 1.f) // m_traverseType 와 상관없이 max 가 높으면 종료.
						break;
				}
				else if (child && !child->child) // terminal node
				{
					node->result = 2;
					child->processCnt = 1;
					child->result = 2;
					nodeResults[node->id] = 2;
					matchResult->m_result = 1;
					matchResult->m_resultStr = child->name;

					if (matchResult->m_traverseType == 0) // if match success return
						break; // success, loop terminate
					if (matchResult->m_data[node->id].max >= 1.f) // m_traverseType 와 상관없이 max 가 높으면 종료.
						break;
				}
				else
				{
					q.push_back(ItemType(node, child)); // high priority
					rotateright(q); // push front
				}
			}
			--coreCnt;
		}

		// match node 처리
		if (emptyIdx >= 0 && !q.empty())
		{
			ItemType item = q.front();
			rotatepopvector(q, 0); // pop front
			sParseTree *parent = item.first;
			sParseTree *node = item.second;

			if (nodeResults[node->id] >= 0)
			{
				if (nodeResults[node->id] > 0) // success node
				{
					sParseTree *child = node->child;
					if (!child) // success!! terminal node, end function
					{
						node->result = 2;
						nodeResults[node->id] = 2;
						matchResult->m_result = 1;
						matchResult->m_resultStr = node->name;

						if (matchResult->m_traverseType == 0) // if match success return
							break; // success, loop terminate
						if (matchResult->m_data[node->id].max >= 1.f) // m_traverseType 와 상관없이 max 가 높으면 종료.
							break;
					}
					else
					{
						q.push_back(ItemType(node, child)); // high priority
						rotateright(q); // push front
					}
				}
			}
			else
			{
				if ('@' == node->name[0]) // link node
				{
					sParseTree *child = node->child;
					if (child)
					{
						node->processCnt = 1;
						node->result = 1;
						q.push_back(ItemType(node, child)); // high priority
						rotateright(q); // push front
					}
				}
				else
				{
					isThreadWork = true; // 스레드 실행 예정
					ZeroMemory(&args[emptyIdx], sizeof(args[emptyIdx]));
					args[emptyIdx].isUsed = true;
					args[emptyIdx].matchResult = matchResult;
					args[emptyIdx].node = node;
					args[emptyIdx].parent = parent;
					handles[emptyIdx] = (HANDLE)_beginthreadex(NULL, 0, MatchThreadSub, &args[emptyIdx], 0, NULL);
					++coreCnt;
				}
			}

			// push sibling node (breath first loop)
			if (node->next)
			{
				q.push_back(ItemType(parent, node->next)); // low priority
				if (parent) // if parent node, high priority
					rotateright(q); // push front, 
			}
		}
	}

	// 말단 노드 중, 가장 적합도가 높은 노드를 찾는다.
	if (matchResult->m_traverseType == 1)
	{
		matchResult->m_result = 1; // m_traverseType=1 이면, 결과를 무조건 내게 되어있다.

		if (sParseTree *mostFitnessNode = (sParseTree*)FindMostFitnessNode(*matchResult, matchResult->m_resultStr))
		{
			mostFitnessNode->result = 2;
			matchResult->m_data[mostFitnessNode->id].result = 2;
		}
	}

	// 매칭 종료
	matchResult->m_isEnd = true;

	for (int i = 0; i < MAX_CORE; ++i)
	{
		if (handles[i])
		{
			WaitForSingleObject(handles[i], INFINITE);
			handles[i] = NULL;
		}
	}

	// 입력으로 저장되었던 정보를 제거한다.
	if (isRemoveInput)
	{
		cMatchProcessor::Get()->RemoveInputImage(inputName, inputImageId);
	}

	return 0;
}


// 가장 적합도가 높은 노드를 찾는다.
// 부모 노드 적합도의 평균을 내서 계산한다.
const sParseTree* FindMostFitnessNode(const cMatchResult &matchResult, OUT string &resultStr)
{
	bool visitNode[1024]; // 재방문을 막기위해 쓰임.
	double fitness[1024]; // 각 노드당 적합도, 부모 적합도를 상속받는다.
	int parentCount[1024]; // 부모 노드 갯수, 연관된 모든 노드의 갯수로 평균을 구한다.
	ZeroMemory(visitNode, sizeof(visitNode));
	ZeroMemory(fitness, sizeof(fitness));
	ZeroMemory(parentCount, sizeof(parentCount));

	typedef pair<const sParseTree*, const sParseTree*> ItemType; //<parent node, child node>
	vector<ItemType> q; // work like stack
	q.reserve(16);
	q.push_back( ItemType(NULL, matchResult.m_nodeLabel->child) );

	const sParseTree *mostFitnessNode = NULL;
	double max = -FLT_MAX;
	while (!q.empty())
	{
		ItemType item = q.back();
		q.pop_back();
		const sParseTree *parent = item.first;
		const sParseTree *node = item.second;

		if (visitNode[node->id])
			continue;
		visitNode[node->id] = true;
		if (!node)
			continue;

		// 적합도 계산
		if (parent)
		{
			fitness[node->id] = fitness[parent->id]; // 부모 적합도 상속
			parentCount[node->id] = parentCount[parent->id]; // 부모 노드 갯수 상속
		}

		if (('@' != node->name[0])) // 링크노드가 아닐때만 처리한다.
		{
			++parentCount[node->id];
			fitness[node->id] += matchResult.m_data[node->id].max;
		}

		const sParseTree *child = node->child;
		if (!child || (child && !child->child)) // terminal node
		{
			double avr = fitness[node->id] / parentCount[node->id];
			avr += (avr * (parentCount[node->id] - 1) * 0.13f); // 깊은 트리일수록, 가중치를 준다.

			if (max < avr)
			{
				mostFitnessNode = node;
				max = avr;
			}
		}
		else
		{
			q.push_back( ItemType(node, child) );
		}

 		if (node->next)
 			q.push_back( ItemType(parent, node->next) );
	}

	if (!mostFitnessNode)
	{
		resultStr = ""; // error return
		return NULL;
	}

	if (mostFitnessNode->child && !mostFitnessNode->child->child)
	{
		if (string("return_parent") == mostFitnessNode->child->name)
			resultStr = matchResult.m_data[mostFitnessNode->id].str;
		else
			resultStr = mostFitnessNode->child->name;
	}
	else
	{
		resultStr = mostFitnessNode->name;
	}

	return mostFitnessNode;
}
