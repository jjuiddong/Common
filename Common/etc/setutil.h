//
// 2019-06-17, jjuiddong
// set algorithm
//
#pragma once


namespace common
{

	bool CollectAllSubset(const vector<int> &initial
		, OUT vector<vector<int>> &out
		, const uint minSetSize = 0);

	bool CollectAllSubset(const uint subSetSize, const vector<int> &initial
		, OUT vector<vector<int>> &out);

}
