
#include "pch.h"
#include "cfg.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


cCfg::cCfg(const string &fileName)
{
	if (!fileName.empty())
		Parse(fileName);
}

cCfg::~cCfg()
{
	Clear();
}


bool cCfg::Parse(const string &fileName)
{
	using boost::property_tree::ptree;

	Clear();

	try
	{
		ptree props;
		boost::property_tree::read_json(fileName, props);

		m_output = common::GetFileNameExceptExt(fileName);
		m_output += ".js";

		auto it = props.get_child("source");
		for (auto &kv : it)
			m_sources.push_back(kv.second.get<string>(""));
		auto it1 = props.get_child("dispatcher");
		for (auto &kv : it1)
			m_dispatchers.push_back(kv.second.get<string>(""));
		auto it2 = props.get_child("handler");
		for (auto &kv : it2)
			m_handlers.push_back(kv.second.get<string>(""));
	}
	catch (std::exception&e)
	{
		string err = e.what();
		std::cout << err << std::endl;
		return false;
	}
	return true;
}


bool cCfg::IsValid()
{
	return !m_output.empty() && !m_sources.empty();
}


void cCfg::Clear()
{
	m_sources.clear();
	m_dispatchers.clear();
	m_handlers.clear();
	m_output.clear();
}
