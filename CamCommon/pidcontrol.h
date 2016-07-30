//
// PID 제어 클래스
// 
// 2차원 값만 제어 할 수 있다.
//
#pragma once


class cPIDControl
{
public:
	cPIDControl();
	virtual ~cPIDControl();

	cv::Point2f Update(const cv::Point2f &pos);


public:
	cv::Point2f m_originalPos;
	cv::Point2f m_Pos;
	cv::Point2f m_oldDiff;

	float m_Kp; // proportional 
	float m_Ki; // integral
	float m_Kd; // derivative


};
