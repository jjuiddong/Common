//
// 2020-12-29, jjuiddong
// json parsing utility using boost
//	- code snipet
//
#pragma once


/*
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

template <class T, size_t MAX>
bool GetArray(boost::property_tree::ptree &ptree, const string &key
	, OUT T out[])
{
	int idx = 0;
	auto it = ptree.get_child(key);
	for (auto &kv : it)
	{
		out[idx++] = kv.second.get<T>("");
		if (idx >= MAX)
			break;
	}
	return true;
}


template <class T>
bool GetVector(boost::property_tree::ptree &ptree, const string &key
	, OUT vector<T> &out)
{
	auto it = ptree.get_child(key);
	for (auto &kv : it)
		out.push_back(kv.second.get<T>(""));
	return true;
}


// json string parsing and return vec3
// "key" : [1, 2, 3]
Vector3 GetVector3(boost::property_tree::ptree &ptree, const string &key)
{
	float ar[3];
	GetArray<float, 3>(ptree, key, ar);
	return Vector3(ar[0], ar[1], ar[2]);
}

// json string parsing and return vec4
// "key" : [1, 2, 3, 4]
Vector4 GetVector4(boost::property_tree::ptree &ptree, const string &key)
{
	float ar[4];
	GetArray<float, 4>(ptree, key, ar);
	return Vector4(ar[0], ar[1], ar[2], ar[3]);
}

*/
