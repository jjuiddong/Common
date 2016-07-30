
#include "stdafx.h"
#include "featurematch.h"


using namespace cvproc;
using namespace cv;
using namespace cv::xfeatures2d;

void CallbackFeatureMatch(int event, int x, int y, int flags, void* userdata);

cFeatureMatch::cFeatureMatch(const bool gray, const bool debug) //gray=true, debug=false
	: m_gray(gray)
	, m_debug(debug)
	, m_IsDetection(false)
	, m_isLog(true)
{
}

cFeatureMatch::~cFeatureMatch()
{
}


// 이미지 피쳐 매치
bool cFeatureMatch::Match(const string &srcFileName, const string &templFileName, const string &option, const string &comment)
//option="", comment=""
{
	Mat A = imread(srcFileName, (m_gray) ? IMREAD_GRAYSCALE : IMREAD_COLOR);
	Mat B = imread(templFileName, (m_gray) ? IMREAD_GRAYSCALE : IMREAD_COLOR);
	RETV(!A.data, false);
	RETV(!B.data, false);

	if (!option.empty())
	{
		A = imread(srcFileName);
		cConvertImage cvt;
		cvt.Convert(A, A, option);
	}

	return Match(A, B, comment);
}


// 이미지 피쳐 매치
bool cFeatureMatch::Match(const cv::Mat &img, const cv::Mat &templ, 
	const string &comment, OUT cv::Mat *out) //comment="", out=NULL
{
	const Mat &img_scene = img;
	const Mat &img_object = templ;

	if (m_isLog)
		dbg::Log("cFeatureMatch::Match() \n");

	const int t1 = timeGetTime();

	//-- Step 1: Detect the keypoints and extract descriptors using SURF
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);

	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	detector->detectAndCompute(img_object, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	const bool result = Match(img_scene, keypoints_scene, 
		descriptors_scene, img_object, keypoints_object, 
		descriptors_object, comment, out);

	return result;
}


bool cFeatureMatch::Match(const cv::Mat &img, const vector<KeyPoint> &imgKeyPoints, const cv::Mat &imgDescriptors,
	const cv::Mat &templ, const vector<KeyPoint> &templKeyPoints, const cv::Mat &templDescriptors,
	const string &comment, OUT cv::Mat *out) // comment="", out=NULL
{
	const Mat &img_scene = img;
	const Mat &img_object = templ;
	const std::vector<KeyPoint> &keypoints_object = templKeyPoints;
	const std::vector<KeyPoint> &keypoints_scene = imgKeyPoints;
	const Mat &descriptors_object = templDescriptors;
	const Mat &descriptors_scene = imgDescriptors;

	//-- Step 2: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	m_max = 0; m_min = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < m_min) m_min = dist;
		if (dist > m_max) m_max = dist;
	}

// 	dbg::Log("-- Max dist : %f \n", m_max);
// 	dbg::Log("-- Min dist : %f \n", m_min);

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * m_min)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, RANSAC);
	if (!H.data)
	{
		if (m_isLog)
			dbg::Log("not find Homography \n");
		return false;
	}

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);


	// 투영된 영역이, 원본과 차이가 크면 잘 못 인식한 것으로 간주한다.
	m_IsDetection = true;
	m_rect.Init(scene_corners);
	const float f1 = (abs(m_rect.Width() - img_object.cols) / (float)img_object.cols);
	const float f2 = (abs(m_rect.Height() - img_object.rows) / (float)img_object.rows);
	if ((f1 > 0.2f) || (f2 > 0.2f))
	{
		m_IsDetection = false;
	}

	if (m_isLog)
	{
		dbg::Log("%s --- IsDetection = %d \n", comment.c_str(), m_IsDetection);
		dbg::Log("%s --- f1=%f, f2=%f \n", comment.c_str(), f1, f2);
		dbg::Log("%s --- goodmatch size=%d, rect_w=%d, rect_h=%d, obj_w=%d, obj_h=%d\n",
			comment.c_str(), good_matches.size(), m_rect.Width(), m_rect.Height(), img_object.cols, img_object.rows);
	}

	if (m_debug || out)
	{
		//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		Mat img_matches;
		drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		line(img_matches, scene_corners[0] + Point2f((float)img_object.cols, 0), scene_corners[1] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[1] + Point2f((float)img_object.cols, 0), scene_corners[2] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[2] + Point2f((float)img_object.cols, 0), scene_corners[3] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[3] + Point2f((float)img_object.cols, 0), scene_corners[0] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);

		//-- Show detected matches
		stringstream ss;
		ss << comment << ", max=" << m_max << ", " << ((m_IsDetection) ? "Detection Success" : "Detection Fail");
		putText(img_matches, ss.str(), Point(20, 20), 1, 1.3f, Scalar(0, 0, 255), 2);

		// 최대 화면 크기만큼, 이미지 사이즈를 조정한다.
		const int screenW = GetSystemMetrics(SM_CXSCREEN);
		const int screenH = GetSystemMetrics(SM_CYSCREEN);
		const int w = MIN(img_matches.cols, screenW);
		const int h = MIN(img_matches.rows, screenH);
		Mat dst(h, w, img_matches.flags);
		resize(img_matches, dst, Size(w, h));

		if (out)
		{
			*out = dst;
		}
		else
		{
			imshow("Good Matches & Object detection", dst);
			setMouseCallback("Good Matches & Object detection", CallbackFeatureMatch, NULL);
			waitKey(1);
		}
		m_matResult = img_matches;
	}

	return m_IsDetection;
}


// SIFT Match
bool cFeatureMatch::SIFTMatch(const cv::Mat &img, const cv::Mat &templ, const string &comment) //comment=""
{
	const Mat &img_1 = img;
	const Mat &img_2 = templ;

	cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();

	// --Step 1: Detect the keypoints :
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	f2d->detect(img_1, keypoints_1);
	f2d->detect(img_2, keypoints_2);

	//-- Step 2: Calculate descriptors (feature vectors)
	Mat descriptors_1, descriptors_2;
	f2d->compute(img_1, keypoints_1, descriptors_1);
	f2d->compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors using BFMatcher :
	BFMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	if (m_debug)
	{
		printf("-- Max dist : %f \n", max_dist);
		printf("-- Min dist : %f \n", min_dist);

		//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
		//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
		//-- small)
		//-- PS.- radiusMatch can also be used here.
		std::vector< DMatch > good_matches;
		for (int i = 0; i < descriptors_1.rows; i++)
		{
			if (matches[i].distance <= max(2 * min_dist, 0.02))
			{
				good_matches.push_back(matches[i]);
			}
		}

		//-- Draw only "good" matches
		drawMatches(img_1, keypoints_1, img_2, keypoints_2,
		good_matches, m_matResult, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		//-- Show detected matches
		// 최대 화면 크기만큼, 이미지 사이즈를 조정한다.
		const int screenW = GetSystemMetrics(SM_CXSCREEN);
		const int screenH = GetSystemMetrics(SM_CYSCREEN);
		const int w = MIN(m_matResult.cols, screenW);
		const int h = MIN(m_matResult.rows, screenH);
		Mat dst(h, w, m_matResult.flags);
		resize(m_matResult, dst, Size(w, h));
		imshow("feature match result", dst);
		setMouseCallback("feature match result", CallbackFeatureMatch, NULL);
		waitKey(1);
	}

	return m_max > 0.5f;
}


void CallbackFeatureMatch(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		//destroyWindow("feature match result");
		destroyWindow("Good Matches & Object detection");
	}
}
