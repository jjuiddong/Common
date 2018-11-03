
#include "stdafx.h"
#include "spathfinder.h"

using namespace ai;

namespace ai
{
	enum { MAX_VERTEX  = 1024};
	float g_edges_len2[MAX_VERTEX][MAX_VERTEX];
	bool  g_edges_visit2[MAX_VERTEX][MAX_VERTEX];
}


cSPathFinder::cSPathFinder()
	: m_map(NULL)
{
}

cSPathFinder::~cSPathFinder()
{
	Clear();
}


bool cSPathFinder::Read(const char *fileName)
{
	auto size = GetRowsCols(fileName);
	if (size.first <= 0)
		return false;

	Clear();

	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	m_rows = size.first;
	m_cols = size.second;
	m_map = new sVertex[size.first * size.second];
	ZeroMemory(m_map, size.first * size.second);

	int y = 0;
	string line;
	while (getline(ifs, line))
	{
		if (line.empty())
			break;

		int x = 0;
		for (u_int i = 0; i < line.size(); ++i)
		{
			int n = line[i] - '0';
			if (abs(n) > 9)
				return false;

			m_map[y * m_cols + x].type = (BYTE)n;
			++x;
		}
		++y;
	}
	return true;
}


// a-star path finder
bool cSPathFinder::Find(const Vector2i &startPos
	, const Vector2i &endPos
	, OUT vector<Vector2i> &out)
{
	ZeroMemory(g_edges_visit2, sizeof(g_edges_visit2));
	ZeroMemory(g_edges_len2, sizeof(g_edges_len2));

	vector<Vector2i> candidate;
	candidate.reserve(m_rows * m_cols);
	candidate.push_back(startPos);

	const int startIdx = startPos.y*m_cols + startPos.x;
	const int endIdx = endPos.y*m_cols + endPos.x;
	m_map[startIdx].startLen = 0;
	m_map[startIdx].endLen = Distance_Manhatan(startPos, endPos);

	int loopCount1 = 0; // debug, loop count
	int loopCount2 = 0; // debug, insertion count
	bool isFind = false;
	while (!candidate.empty())
	{
		const Vector2i curPos = candidate.front();
		rotatepopvector(candidate, 0);

		const int curIdx = curPos.y*m_cols + curPos.x;
		sVertex &curVtx = m_map[curIdx];

		if (endPos == curPos)
		{
			isFind = true;
			break;
		}

		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			const Vector2i nextPos = curPos + offsetPos[i];
			if ((nextPos.x < 0)
				|| (nextPos.x >= m_cols)
				|| (nextPos.y < 0)
				|| (nextPos.y >= m_rows))
				continue;
			
			const int nextIdx = nextPos.y*m_cols + nextPos.x;
			sVertex &nextVtx = m_map[nextIdx];
			if (0 == nextVtx.type)
				continue; // wall node

			if (g_edges_visit2[curIdx][nextIdx])
				continue;

			float alpha = 0.f;
			if (1 == nextVtx.type)
				alpha = 1.1f;

			nextVtx.startLen = curVtx.startLen + Distance_Manhatan(curPos, nextPos) + alpha + 0.00001f;
			nextVtx.endLen = Distance_Manhatan(endPos, nextPos);
			g_edges_visit2[curIdx][nextIdx] = true;
			g_edges_visit2[nextIdx][curIdx] = true;
			g_edges_len2[curIdx][nextIdx] = nextVtx.startLen + nextVtx.endLen;
			g_edges_len2[nextIdx][curIdx] = nextVtx.startLen + nextVtx.endLen;

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				++loopCount1;

				sVertex &compVtx = m_map[candidate[k].y * m_cols + candidate[k].x];
				if ((compVtx.endLen + compVtx.startLen)
					> (nextVtx.endLen + nextVtx.startLen))
				{
					++loopCount2;

					candidate.push_back(nextPos);
					common::rotateright2(candidate, k);
					isInsert = true;
					break;
				}
			}

			if (!isInsert)
				candidate.push_back(nextPos);
		}
	}

	if (!isFind)
		return false;

	// tracking end point to start point
	vector<Vector2i> verticesIndices;

	out.push_back(endPos);
	verticesIndices.push_back(endPos);

	ZeroMemory(g_edges_visit2, sizeof(g_edges_visit2));

	Vector2i curPos = endPos;
	while ((curPos != startPos) && (verticesIndices.size() < 1000))
	{
		float minEdge = FLT_MAX;
		Vector2i nextPos(-1,-1);

		const int curIdx = curPos.y*m_cols + curPos.x;
		sVertex &vtx = m_map[curIdx];
		
		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			const Vector2i edgePos = curPos + offsetPos[i];
			if ((edgePos.x < 0)
				|| (edgePos.x >= m_cols)
				|| (edgePos.y < 0)
				|| (edgePos.y >= m_rows))
				continue;

			const int edgeIdx = edgePos.y*m_cols + edgePos.x;
			if (g_edges_visit2[curIdx][edgeIdx])
				continue;

			const float len = g_edges_len2[curIdx][edgeIdx];
			if (0 == len)
				continue;

			if (minEdge > len)
			{
				minEdge = len;
				nextPos = edgePos;
			}
		}

		if (nextPos.x < 0)
		{
			assert(0);
			break; // error occur
		}

		const int nextIdx = nextPos.y*m_cols + nextPos.x;

		g_edges_visit2[curIdx][nextIdx] = true;
		g_edges_visit2[nextIdx][curIdx] = true;
		out.push_back(nextPos);
		verticesIndices.push_back(nextPos);
		curPos = nextPos;
	}

	assert(verticesIndices.size() < 1000);

	std::reverse(out.begin(), out.end());
	std::reverse(verticesIndices.begin(), verticesIndices.end());

	return true;
}


// Manhatan Distance
float cSPathFinder::Distance_Manhatan(const Vector2i &p0, const Vector2i &p1) const
{
	return (float)(abs(p0.x - p1.x) + abs(p0.y - p1.y));
}


// return map file rows, cols size
// 1 character size
std::pair<int, int> cSPathFinder::GetRowsCols(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return { 0, 0 };

	int rows = 0;
	int cols = 0;

	string line;
	while (getline(ifs, line))
	{
		if (line.empty())
			break;
		cols = max((int)line.size(), cols);
		++rows;
	}

	return { rows, cols };
}


void cSPathFinder::Clear()
{
	SAFE_DELETE(m_map);
}
