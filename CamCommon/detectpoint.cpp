
#include "stdafx.h"
#include "detectpoint.h"
#include "config.h"


using namespace cv;

cDetectPoint::cDetectPoint(const string &windowName) :
	m_pos(0, 0)
	, m_show(false)
	, m_windowName(windowName)
	, m_makeKeypointImage(false)
{
	m_show = g_config.m_conf.showDetectPoint;

	if (m_show)
	{
		namedWindow(m_windowName, 0);
		cvResizeWindow(m_windowName.c_str(), g_config.m_detectPointBinaryScreenRect.width, g_config.m_detectPointBinaryScreenRect.height);
		cvMoveWindow(m_windowName.c_str(), g_config.m_detectPointBinaryScreenRect.x, g_config.m_detectPointBinaryScreenRect.y);
	}
}


cDetectPoint::cDetectPoint(const cRecognitionConfig &recogConfig, const string &windowName) :
	m_pos(0,0)
	, m_show(false)
	, m_windowName(windowName)
{
	m_show = g_config.m_conf.showDetectPoint;

	if (m_show)
	{
		namedWindow(m_windowName, 0);
		cvResizeWindow(m_windowName.c_str(), g_config.m_detectPointBinaryScreenRect.width, g_config.m_detectPointBinaryScreenRect.height);
		cvMoveWindow(m_windowName.c_str(), g_config.m_detectPointBinaryScreenRect.x, g_config.m_detectPointBinaryScreenRect.y);
	}

	UpdateParam(recogConfig);
}

cDetectPoint::~cDetectPoint()
{
	cvDestroyWindow(m_windowName.c_str());
}


// 파라메터정보를 환경설정 파일로부터 업데이트 한다.
void cDetectPoint::UpdateParam(const cRecognitionConfig &recogConfig)
{
	m_show = g_config.m_conf.showDetectPoint;
	m_printDetectPoint = recogConfig.m_attr.printDetectPoint;

	//	SimpleBlobDetector::Params params;
	//	ZeroMemory(&m_params, sizeof(m_params));

	m_params.thresholdStep = 10;
	m_params.minRepeatability = 0;
	m_params.minDistBetweenBlobs = 10;

	// Change thresholds
	m_params.minThreshold = 1;
	m_params.maxThreshold = 20;

	// Filter by Area.
	m_params.filterByArea = true;
	m_params.minArea = recogConfig.m_attr.detectPointMinArea;
	m_params.maxArea = recogConfig.m_attr.detectPointMaxArea;

	// Filter by Circularity
	m_params.filterByCircularity = true;
	m_params.minCircularity = recogConfig.m_attr.detectPointCircularity;
	m_params.maxCircularity = 1;

	// Filter by Convexity
	m_params.filterByConvexity = true;
	m_params.minConvexity = recogConfig.m_attr.detectPointMinConvexity;
	m_params.maxConvexity = 1;

	// Filter by Inertia
	m_params.filterByInertia = true;
	m_params.minInertiaRatio = recogConfig.m_attr.detectPointMinInertia;
	m_params.maxInertiaRatio = 1.f;

	m_thresholdValue = recogConfig.m_attr.detectPointThreshold;
	//m_params = params;
}


// src 이미지상에서 점을 찾는다.
// 포인트를 찾았다면 해당 위치를 리턴하고, 그렇지 않다면, 전의 위치를 리턴한다.
bool cDetectPoint::DetectPoint(const Mat &src, Point &out, bool isCvtGray)
{
	Mat grayscaleMat = src.clone();
	if (isCvtGray)
		cvtColor(src, grayscaleMat, CV_RGB2GRAY); // 컬러를 흑백으로 변환

	if (m_binMat.empty())
		m_binMat.create(grayscaleMat.size(), grayscaleMat.type());
	threshold(grayscaleMat, m_binMat, m_thresholdValue, 255, cv::THRESH_BINARY_INV);

	//-------------------------------------------------------q---------------------------------------
	// 스크린 이미지에서 포인터를 찾는다.
	SimpleBlobDetector detector;
	detector.create(m_params);

	// Detect blobs.
	std::vector<KeyPoint> keypoints;
	detector.detect(m_binMat, keypoints);

	// 포인트를 찾았다면 해당 위치를 리턴하고, 그렇지 않다면, 전의 위치를 리턴한다.
	Point pos = (keypoints.empty()) ? m_pos : keypoints[0].pt;

	if (keypoints.size() >= 2)
		pos = keypoints[1].pt;

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	if ((m_show || m_makeKeypointImage) && !keypoints.empty())
	{
		drawKeypoints(m_binMat, keypoints, m_binWithKeypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		if (m_show)
			imshow(m_windowName, m_binWithKeypoints);
	}

	if (keypoints.empty())
	{
		m_points.clear();
	}
	else
	{
		m_points.resize(keypoints.size());
		for (u_int i = 0; i < keypoints.size(); ++i)
			m_points[i] = keypoints[ i].pt;
	}

	m_pos = pos;
	out = pos;

	if (!keypoints.empty() && m_printDetectPoint)
		printf("pt.x = %d, pt.y = %d \n", pos.x, pos.y);

	return true;
}


// approxPolyDP() 를 이용해, 점을 근사화 한 후 찾는다.
// 점이 늘어졌을 때도 인식시키기 위해서다.
bool cDetectPoint::DetectPoint2(const Mat &src, OUT Point &out, bool isCvtGray)
{
	if (src.empty())
		return false;

	Mat grayscaleMat = src.clone();
	if (isCvtGray)
		cvtColor(src, grayscaleMat, CV_RGB2GRAY); // 컬러를 흑백으로 변환

	//blur(grayscaleMat, grayscaleMat, Size(3, 3));

	if (m_binMat.empty())
		m_binMat.create(grayscaleMat.size(), grayscaleMat.type());
	threshold(grayscaleMat, m_binMat, m_thresholdValue, 255, cv::THRESH_BINARY_INV);

	
	//----------------------------------------------------------------------------------------------
	// 스크린 이미지에서 포인터를 찾는다.
	if (m_bw.empty())
		m_bw = m_binMat.clone();
	cv::Canny(m_binMat, m_bw, 0, 50, 5);
	
	// Find contours
	vector< vector<cv::Point> > contours;
	cv::findContours(m_bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


	if ((m_show || m_makeKeypointImage) && !m_binWithKeypoints.empty())
		m_binWithKeypoints.setTo(Scalar(255, 255, 255));

	vector<Point> keypoints;
	keypoints.reserve(32);
	vector<cv::Point> approx;

	for (u_int i = 0; i < contours.size(); i++)
	{
		const double epsilon = 2;
		cv::approxPolyDP(cv::Mat(contours[i]), approx, epsilon, true);

		if ((m_show || m_makeKeypointImage) && !keypoints.empty())
		{
			if (!m_binWithKeypoints.empty())
				cv::drawContours(m_binWithKeypoints, contours, (int)i, Scalar::all(-1), 1, 8);
		}

		if (!cv::isContourConvex(approx))
			continue;
		const double area = std::fabs(cv::contourArea(contours[i]));
		if ((area < m_params.minArea) || (area > m_params.maxArea))
			continue;

		if (approx.size() == 3)
		{
			//setLabel(dst, "TRI", contours[i]);    // Triangles
		}
		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			double mincos = FLT_MAX;
			double maxcos = FLT_MIN;
			const int vtc = approx.size();
			for (int j = 2; j < vtc + 1; j++)
			{
				const double an = angle(approx[j%vtc], approx[j - 2], approx[j - 1]);
				if (an > maxcos)
					maxcos = an;
				if (an < mincos)
					mincos = an;
			}

			if (vtc == 4 && mincos >= -1.f && maxcos <= 1.f) // Rect
				keypoints.push_back(GetContourCenter(approx));
			else if (vtc == 5 && mincos >= -1.f && maxcos <= 1.f) // Penta
				keypoints.push_back(GetContourCenter(approx));
			else if (vtc == 6 && mincos >= -0.95 && maxcos <= 1.f) // Hexa
				keypoints.push_back(GetContourCenter(approx));
		}
		else
		{
			// Detect and label circles
			const double area = cv::contourArea(contours[i]);
			const cv::Rect r = cv::boundingRect(contours[i]);
			const int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
				std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
				keypoints.push_back(GetContourCenter(approx));
		}
	}

	// 포인트를 찾았다면 해당 위치를 리턴하고, 그렇지 않다면, 전의 위치를 리턴한다.
	Point pos = (keypoints.empty()) ? m_pos : keypoints[0];

	if (keypoints.size() >= 2)
		pos = keypoints[1];

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	if ((m_show || m_makeKeypointImage) && !keypoints.empty())
	{
		//drawKeypoints(m_binMat, keypoints, m_binWithKeypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		if (m_show && !m_binWithKeypoints.empty())
			imshow(m_windowName, m_binWithKeypoints);
	}

	if (keypoints.empty())
	{
		m_points.clear();
	}
	else
	{
		m_points.resize(keypoints.size());
		for (u_int i = 0; i < keypoints.size(); ++i)
			m_points[i] = keypoints[i];
	}

	m_pos = pos;
	out = pos;

	if (!keypoints.empty() && m_printDetectPoint)
		printf("pt.x = %d, pt.y = %d \n", pos.x, pos.y);

	return true;
}

