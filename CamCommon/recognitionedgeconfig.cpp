
#include "stdafx.h"
#include "recognitionedgeconfig.h"



void cRecognitionEdgeConfig::InitDefault()
{
	m_attr.threshold1 = 100;
	m_attr.threshold2 = 200;
	m_attr.minArea = 100;
	m_attr.minCos = -0.2f;
	m_attr.maxCos = 0.2f;

}


void cRecognitionEdgeConfig::UpdateParseData()
{
	m_attr.threshold1 = GetInt("threshold1");
	m_attr.threshold2 = GetInt("threshold2");
	m_attr.minArea = GetInt("minArea");
	m_attr.minCos = GetDouble("minCos");
	m_attr.maxCos = GetDouble("maxCos");
}


cRecognitionEdgeConfig& cRecognitionEdgeConfig::operator = (const cRecognitionEdgeConfig &rhs)
{
	if (this != &rhs)
	{
		m_attr.threshold1 = rhs.m_attr.threshold1;
		m_attr.threshold2 = rhs.m_attr.threshold2;
		m_attr.minArea = rhs.m_attr.minArea;
		m_attr.minCos = rhs.m_attr.minCos;
		m_attr.maxCos = rhs.m_attr.maxCos;
	}

	return *this;
}


bool cRecognitionEdgeConfig::Write(const string &fileName)
{
	m_options["threshold1"] = common::format("%d", m_attr.threshold1);
	m_options["threshold2"] = common::format("%d", m_attr.threshold2);
	m_options["minArea"] = common::format("%d", m_attr.minArea);
	m_options["minCos"] = common::format("%f", m_attr.minCos);
	m_options["maxCos"] = common::format("%f", m_attr.maxCos);

	cConfig::Write(fileName);

	return true;
}
