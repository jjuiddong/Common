
#include "stdafx.h"
#include "setutil.h"

using namespace common;


// 전체 집합에서, 부분집합을 구해서 리턴한다.
// 공집합 제외
// ex) initial:{0,1,2}
//	   return: {0}, {1}, {2}, {0,1}, {0,2}, {1,2}, {0,1,2}
bool common::CollectAllSubset(const vector<int> &initial
	, OUT vector<vector<int>> &out
	, const uint minSetSize //= 0
)
{
	if (initial.empty())
		return false;

	if ((minSetSize == 0)
		|| ((minSetSize > 0) && (minSetSize <= initial.size()))
		)
		out.push_back(initial);

	for (uint i=0; i < initial.size(); ++i)
	{
		vector<int> new_set(initial);
		common::rotatepopvector(new_set, i);
		CollectAllSubset(new_set, out);
	}

	return true;
}


// 전체 집합에서, 부분집합을 구해서 리턴한다.
//공집합 제외
// subSetSize크기의 부분집합만 저장한다.
bool common::CollectAllSubset(const uint subSetSize, const vector<int> &initial
	, OUT vector<vector<int>> &out)
{
	if (initial.empty())
		return false;

	if (subSetSize == initial.size())
		out.push_back(initial);

	for (uint i = 0; i < initial.size(); ++i)
	{
		vector<int> new_set(initial);
		common::rotatepopvector(new_set, i);
		CollectAllSubset(subSetSize, new_set, out);
	}
	return true;
}
