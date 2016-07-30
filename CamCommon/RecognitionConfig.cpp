
#include "stdafx.h"
#include "RecognitionConfig.h"



void cRecognitionConfig::InitDefault()
{
	m_attr.detectPointThreshold = 33;
	m_attr.detectPointMinArea = 50;
	m_attr.detectPointMaxArea = 200;
	m_attr.detectPointCircularity = 0.1f;
	m_attr.detectPointMinConvexity = 0.87f;
	m_attr.detectPointMinInertia = 0.01f;
	m_attr.printDetectPoint = false;
	m_attr.detectPointMinLineDist = 20;
	m_attr.detectPointMaxLineDist = 60;
	m_attr.detectPointLoopCount = 5;

	m_attr.detectLineMaxDot = 0.1f;
	m_attr.detectLineMinAngle = 0;
	m_attr.detectLineMinOldAngle = 0;

	m_attr.thresholdFinderOffset = 5;
}


void cRecognitionConfig::UpdateParseData()
{
	m_attr.detectPointThreshold = GetInt("detect_point_threshold");
 	m_attr.detectPointMinArea = GetFloat("detect_point_minarea");
 	m_attr.detectPointMaxArea = GetFloat("detect_point_maxarea");
 	m_attr.detectPointCircularity = GetFloat("detect_point_circularity");
 	m_attr.detectPointMinConvexity = GetFloat("detect_point_minconvexity");
 	m_attr.detectPointMinInertia = GetFloat("detect_point_minInertia");
 	m_attr.printDetectPoint = GetBool("print_detect_point");
 	m_attr.detectPointMinLineDist = GetFloat("detect_point_minLineDist");
 	m_attr.detectPointMaxLineDist = GetFloat("detect_point_maxLineDist");
 	m_attr.detectPointLoopCount = GetInt("detect_point_loopCount");
 	if (m_attr.detectPointLoopCount < 5)
 		m_attr.detectPointLoopCount = 5;
 
 	m_attr.detectLineMaxDot = GetFloat("detect_line_max_dot");
 	if (m_attr.detectLineMaxDot <= 0)
 		m_attr.detectLineMaxDot = 0.2f;
 
 	m_attr.detectLineMinAngle = GetFloat("detect_line_min_angle");
 	m_attr.detectLineMinOldAngle = GetFloat("detect_line_min_oldangle");
 	m_attr.detectLineMinDiffDistance = GetFloat("detect_line_min_pos_distance");

	m_attr.thresholdFinderOffset = GetInt("thresholdfinder_offset", 5);
}


cRecognitionConfig& cRecognitionConfig::operator = (const cRecognitionConfig &rhs)
{
	if (this != &rhs)
	{
		m_attr.detectPointThreshold = rhs.m_attr.detectPointThreshold;
		m_attr.detectPointMinArea = rhs.m_attr.detectPointMinArea;
		m_attr.detectPointMaxArea = rhs.m_attr.detectPointMaxArea;
		m_attr.detectPointCircularity = rhs.m_attr.detectPointCircularity;
		m_attr.detectPointMinConvexity = rhs.m_attr.detectPointMinConvexity;
		m_attr.detectPointMinInertia = rhs.m_attr.detectPointMinInertia;
		m_attr.printDetectPoint = rhs.m_attr.printDetectPoint;
		m_attr.detectPointMinLineDist = rhs.m_attr.detectPointMinLineDist;
		m_attr.detectPointMaxLineDist = rhs.m_attr.detectPointMaxLineDist;
		m_attr.detectPointLoopCount = rhs.m_attr.detectPointLoopCount;

		m_attr.detectLineMaxDot = rhs.m_attr.detectLineMaxDot;
		m_attr.detectLineMinAngle = rhs.m_attr.detectLineMinAngle;
		m_attr.detectLineMinOldAngle = rhs.m_attr.detectLineMinOldAngle;
		m_attr.detectLineMinDiffDistance = rhs.m_attr.detectLineMinDiffDistance;

		m_attr.thresholdFinderOffset = rhs.m_attr.thresholdFinderOffset;
	}

	return *this;
}


bool cRecognitionConfig::Write(const string &fileName)
{
	m_options["detect_point_threshold"] = common::format("%d", m_attr.detectPointThreshold);
	m_options["detect_point_minarea"] = common::format("%f", m_attr.detectPointMinArea);
	m_options["detect_point_maxarea"] = common::format("%f", m_attr.detectPointMaxArea);
	m_options["detect_point_circularity"] = common::format("%f", m_attr.detectPointCircularity);
	m_options["detect_point_minconvexity"] = common::format("%f", m_attr.detectPointMinConvexity);
	m_options["detect_point_minInertia"] = common::format("%f", m_attr.detectPointMinInertia);
	m_options["print_detect_point"] = common::format("%d", m_attr.printDetectPoint);
	m_options["detect_point_minLineDist"] = common::format("%f", m_attr.detectPointMinLineDist);
	m_options["detect_point_maxLineDist"] = common::format("%f", m_attr.detectPointMaxLineDist);
	m_options["detect_point_loopCount"] = common::format("%d", m_attr.detectPointLoopCount);
	m_options["detect_line_max_dot"] = common::format("%f", m_attr.detectLineMaxDot);
	m_options["detect_line_min_angle"] = common::format("%f", m_attr.detectLineMinAngle);
	m_options["detect_line_min_oldangle"] = common::format("%f", m_attr.detectLineMinOldAngle);
	m_options["detect_line_min_pos_distance"] = common::format("%f", m_attr.detectLineMinDiffDistance);
	m_options["thresholdfinder_offset"] = common::format("%d", m_attr.thresholdFinderOffset);

	cConfig::Write(fileName);

	return true;
}
