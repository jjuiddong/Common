//
// Point를 인식하는데 필요한 파라메터들을 정의한다.
// 주로 cv::SimpleBlobDetector::Params 변수들을 정의한다.
//
#pragma once


struct sRecognitionConfigAttribute
{
	int detectPointThreshold;
	float detectPointMinArea;
	float detectPointMaxArea;
	float detectPointCircularity;
	float detectPointMinConvexity;
	float detectPointMinInertia;
	bool printDetectPoint;
	float detectPointMinLineDist;
	float detectPointMaxLineDist;
	int detectPointLoopCount;

	float detectLineMaxDot;
	float detectLineMinAngle;
	float detectLineMinOldAngle; // 그 전 라인과 최소 각도 차
	float detectLineMinDiffDistance; // 그전 라인과의 거리가 일정이상 멀어졌을 때의 예외처리

	// ThresholdFinder
	int thresholdFinderOffset;

};


class cRecognitionConfig : public common::cConfig
{
public:
	virtual void InitDefault() override;
	virtual void UpdateParseData() override;
	bool Write(const string &fileName);

	cRecognitionConfig& operator = (const cRecognitionConfig &rhs);


public:
	sRecognitionConfigAttribute m_attr;
};

