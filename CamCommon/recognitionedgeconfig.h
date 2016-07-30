//
// 2016-01-21, jjuiddong
//
// 외곽선을 검출하는데 필요한 파라메터를 읽고, 쓰는 기능을 한다.
//
#pragma once


struct sRecognitionEdgeConfigAttribute
{
	int threshold1;
	int threshold2;
	int minArea;
	double minCos;
	double maxCos;
};


class cRecognitionEdgeConfig : public common::cConfig
{
public:
	virtual void InitDefault() override;
	virtual void UpdateParseData() override;
	bool Write(const string &fileName);

	cRecognitionEdgeConfig& operator = (const cRecognitionEdgeConfig &rhs);


public:
	sRecognitionEdgeConfigAttribute m_attr;
};

