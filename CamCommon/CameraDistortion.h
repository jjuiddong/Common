/////////////////////////////////////////////////////////////////////
// CameraDistortion.h
// written  by darkpgmr (http://darkpgmr.tistory.com), 2013

#pragma once

#include "use_opencv.h"

//using namespace cv;

class CameraDistortion
{
	double cx,cy,fx,fy,k1,k2,k3,p1,p2,skew_c;
public:
	CameraDistortion(void);
	~CameraDistortion(void);

	void UndistortImage(const cv::Mat& frame, cv::Mat& frame_undistort);

	void SetParams(double cx, double cy, double fx, double fy, double k1, double k2, double k3, double p1, double p2, double skew_c);
	void Normalize(double &x, double& y);
	void Denormalize(double &x, double& y);
	void Distort(double& x, double& y);
	void DistortPixel(int& x, int& y);
	void UndistortPixel(int& x, int& y);
};
