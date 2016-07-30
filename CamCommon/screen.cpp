
#include "stdafx.h"
#include "screen.h"
#include <fstream>

using namespace cv;

cScreen g_screen;


cScreen::cScreen() : 
	m_oldT(0)
	, m_show(false)
{
}

cScreen::~cScreen()
{
	cvDestroyWindow("Screen");

	if (m_screenContour.size() >= 4)
	{
		std::ofstream ofs("screenContour.txt");
		ofs << m_screenContour[0].x << std::endl;
		ofs << m_screenContour[0].y << std::endl;
		ofs << m_screenContour[1].x << std::endl;
		ofs << m_screenContour[1].y << std::endl;
		ofs << m_screenContour[2].x << std::endl;
		ofs << m_screenContour[2].y << std::endl;
		ofs << m_screenContour[3].x << std::endl;
		ofs << m_screenContour[3].y << std::endl;
	}

}


// 스크린 해상도와 영상에서 인식된 스크린의 크기를 설정한다.
void cScreen::Init(const cv::Rect &screenResolution, const vector<cv::Point> &screenContour)
{
	m_show = g_config.m_conf.showScreen;
	m_screenResolution = screenResolution;
	m_screenContour = screenContour;

	namedWindow("Screen", 0);
	cvResizeWindow("Screen", screenResolution.width, screenResolution.height);

	m_screen = Mat(screenResolution.height, screenResolution.width, CV_8UC(3));
	m_screen.setTo(Scalar(255, 255, 255));

	imshow("Screen", m_screen);
//	namedWindow("Screen", CV_WINDOW_NORMAL);
//	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
}


// 스크린 해상도 설정.
void cScreen::InitResolution(const cv::Rect &screenResolution)
{
	m_show = g_config.m_conf.showScreen;
	m_screenResolution = screenResolution;

// 	namedWindow("Screen", 0);
// 	cvResizeWindow("Screen", screenResolution.width, screenResolution.height);
	namedWindow("Screen", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	m_screen = Mat(screenResolution.height, screenResolution.width, CV_8UC(3));
	m_screen.setTo(Scalar(255, 255, 255));
	imshow("Screen", m_screen);
}


// 화면에 주사하는 스크린의 해상도의 너비를 리턴한다.
int cScreen::GetWidth()
{
	return m_screenResolution.width;
}


// 화면에 주사하는 스크린의 해상도의 높이를 리턴한다.
int cScreen::GetHeight()
{
	return m_screenResolution.height;
}


// 영상에서 인식된 스크린의 너비를 리턴한다.
int cScreen::GetRecogWidth()
{
	return m_screenContour[1].x - m_screenContour[0].x;
}


// 영상에서 인식된 스크린의 높이를 리턴한다.
int cScreen::GetRecogHeight()
{
	return m_screenContour[3].y - m_screenContour[0].y;
}


// 화면 해상도 와 실제 인식한 영상간의 비율을 리턴한다.
// width, height 순서대로 리턴한다.
pair<float, float> cScreen::GetResolutionRecognitionRate()
{
	const float rate = (float)(GetRecogWidth() * GetRecogHeight()) / (float)(GetWidth() * GetHeight());
	return pair<float, float>(rate, rate);
}


// 체스보드 패턴을 출력한다.
void cScreen::ShowChessBoard(const int rows, const int cols)
{
	m_screen.setTo(Scalar(255, 255, 255));
	horizLines(m_screen, m_screenResolution.width, m_screenResolution.height, 5);
	imshow("Screen", m_screen);
}


// 카메라가 스크린을 향해 있어야 한다.
// 스크린을 인식해서, 스크린 크기 정보를 저장한다.
// minScreenArea : 최소 스크린 넓이.
// 스크린을 찾으면 m_screenContour 값이 업데이트 된다.
void cScreen::DetectScreen(cCamera &camera0, const float minScreenArea)
{
	int threshold0 = g_config.m_screen_BinaryThreshold;
	bool detectScreen = false;
	int detectScreenCount = 0;
	std::vector<cv::Point> screenLines(4);
	Rect screenRect;

	Mat camera, binImage, binOutput;

	if (m_show)
	{
		cvNamedWindow("HUV05-screen", 0);
		cvNamedWindow("HUV05-binarization", 0);
		cvResizeWindow("HUV05-binarization", g_config.m_screenBinRect.width, g_config.m_screenBinRect.height);
		cvResizeWindow("HUV05-screen", g_config.m_screenRecogRect.width, g_config.m_screenRecogRect.height);
		cvMoveWindow("HUV05-binarization", g_config.m_screenBinRect.x, g_config.m_screenBinRect.y);
		cvMoveWindow("HUV05-screen", g_config.m_screenRecogRect.x, g_config.m_screenRecogRect.y);
		cvCreateTrackbar("Threshold", "HUV05-binarization", &threshold0, 255, NULL);
	}

	while (1) {
		Mat &camera1 = camera0.ShowCaptureUndistortion();
		if (camera1.empty())
			continue;
	
		camera = camera1.clone();

		// 카메라 영상을 이진화 한다.
		if (binImage.empty()) {
			binImage = Mat(camera0.Resolution(), CV_8UC1); // 흑백 이미지 생성
			binOutput = Mat(camera0.Resolution(), CV_8UC1); // 흑백 이미지 생성
		}

		//cvCvtColor(camera, binImage, CV_RGB2GRAY); // 컬러를 흑백으로 변환
		cvtColor(camera, binImage, CV_RGB2GRAY); // 컬러를 흑백으로 변환
		// 영상의 각 픽셀(x,y) 값이 threshold 값의 초과는 255 로, 그 이하는 0 으로 변환
		threshold(binImage, binOutput, threshold0, 255, CV_THRESH_BINARY);
		//binOutput->origin = camera->origin; // 방향이 뒤집어 진것을 바로 잡아줌

		if (cvWaitKey(10) >= 0)
			break;

		if (m_show)
			imshow("HUV05-binarization", binOutput);

		//----------------------------------------------------------------------------------------------
		// 외곽선 뽑아내기.
		Mat src(camera);

		// Use Canny instead of threshold to catch squares with gradient shading
		cv::Mat bw;
		cv::Canny(Mat(binOutput), bw, 0, 50, 5);

		// Find contours
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<cv::Point> approx;
		cv::Mat dst(camera);// src.clone();

		int rectIndex = -1;
		bool findMask = false;
		Rect bigScreen;

		for (u_int i = 0; i < contours.size(); i++)
		{
			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

			// Skip small or non-convex objects 
			if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
				continue;

			if (approx.size() == 4) // 사각형만 찾는다.
			{
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				std::vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				// Sort ascending the cosine values
				std::sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4 && mincos >= -0.2f && maxcos <= 0.8f)
				{
 					vector<cv::Point> lines(4);
					OrderedContours(approx, lines);

					const Rect rect((int)lines[0].x, (int)lines[0].y,
						(int)(lines[1].x - lines[0].x),
						(int)(lines[2].y - lines[0].y));

					if ((rect.width * rect.height) > (bigScreen.width * bigScreen.height))
					{
						bigScreen = rect;

						// bigScreen 크기가 어느정도 커야 한다. 작다면 찾지 못한걸로 간주한다.
						// 최소 200 X 200 으로 간주한다.
						if (rect.width * rect.height > minScreenArea)
						{
							rectIndex = i;
							screenLines = lines;
						}
					}
				}
			}
		}


		if (!detectScreen)
		{
			if (rectIndex >= 0)
			{
				++detectScreenCount;

				// 안정적으로 스크린 위치를 찾았다면, threshold를 변경하는 것을 멈춘다.				
				if (detectScreenCount > 10)
				{
					detectScreen = true;
					screenRect = bigScreen;
				}
			}
			else
			{
				detectScreenCount = 1;
			}
		}

		if (detectScreen)
			break;

		setLabel(dst, "Screen", screenLines);
		DrawLines(dst, screenLines, Scalar(255,0,0), 2);

		if (m_show)
			imshow("HUV05-screen", dst);
	}

	cvWaitKey(1000);

	if (m_show)
	{
		cvDestroyWindow("HUV05-binarization");
		cvDestroyWindow("HUV05-screen");
	}


 	m_screenContour = screenLines;
}


// Skew 변환 행렬을 계산해서 리턴한다.
// scale 값 만큼 m_screenContour 를 늘려 잡는다.
Mat cScreen::SkewTransform(const int width, const int height, const float scale)
{
	return ::SkewTransform(m_screenContour, width, height, scale);
}



// 스크린 안에서 볼이 움직이는 애니메이션을 한다.
Point2f cScreen::ShowPingpong(const cv::Point2f &ballPos, const cv::Point2f &velocity, const cv::Size2f &ballSize)
{
	const int w = m_screenResolution.width;
	const int h = m_screenResolution.height;

	// 포인터 움직임.
	const int curT = timeGetTime();
	float incT = (float)(curT - m_oldT) / 1000.f;
	if (incT > 1000.f)
	{
		// 처음 시작된 경우.
		incT = 1.f;
		m_ballPos = ballPos;
		m_ballVelocity = velocity;
	}
	else if (incT > 0.1f)
	{
		// 시간 지연이 있었을 경우.
		incT = 0.1f;
	}

	m_oldT = curT;


	const Point2f pointCurPos = m_ballPos + m_ballVelocity * incT;
	if (pointCurPos.x - ballSize.width/2 < 0 && (m_ballVelocity.x < 0))
		m_ballVelocity.x = -m_ballVelocity.x;
	else if (pointCurPos.x + ballSize.width/2 > w && (m_ballVelocity.x > 0))
		m_ballVelocity.x = -m_ballVelocity.x;
	if (pointCurPos.y - ballSize.height/2 < 0 && (m_ballVelocity.y < 0))
		m_ballVelocity.y = -m_ballVelocity.y;
	else if (pointCurPos.y + ballSize.height/2 > h && (m_ballVelocity.y > 0))
		m_ballVelocity.y = -m_ballVelocity.y;
	m_ballPos = pointCurPos;

	m_screen.setTo(Scalar(255, 255, 255));
	ShowBoxLines(10, 10, false);
	ellipse(m_screen, RotatedRect(pointCurPos, ballSize, 0), Scalar(0, 0, 0), CV_FILLED);

	imshow("Screen", m_screen);

	return pointCurPos;
}


// 격자무늬 선을 그린다.
void cScreen::ShowBoxLines(const int rows, const int cols, const bool isClear)
{
	::ShowBoxLines(m_screen, rows, cols, isClear);
}


// 포인트를 출력한다.
void cScreen::ShowPoints(const vector<cv::Point2f> &points, const cv::Size2f &size, const cv::Scalar &color, 
	const bool isClear, const cv::Scalar &bgColor)
// color=Scalar(0,0,0), isClear = false, bgColor=cv::Scalar(255, 255, 255)
{
	if (isClear)
		m_screen.setTo(bgColor);

	for each(auto &pt in points)
	{
		ellipse(m_screen, RotatedRect(pt, size, 0), color, CV_FILLED);
	}

	imshow("Screen", m_screen);
}


void cScreen::CloseScreen()
{
	cvDestroyWindow("Screen");
}


void cScreen::SetScreenContour(const cv::Point contours[4])
{
	m_screenContour.resize(4);

	m_screenContour[0] = contours[0];
	m_screenContour[1] = contours[1];
	m_screenContour[2] = contours[2];
	m_screenContour[3] = contours[3];
}
