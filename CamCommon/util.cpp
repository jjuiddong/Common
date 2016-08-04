
#include "stdafx.h"
#include "util.h"
//#include <gdiplus.h>


using namespace cv;


/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/
double angle(cv::Point pt1, const cv::Point &pt2, const cv::Point &pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
* Helper function to display text in the center of a contour
*/
void setLabel(cv::Mat& im, const std::string &label, const std::vector<cv::Point>& contour, const cv::Scalar &color)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, color, thickness, 8);
}


// 화면 크기, w,h
// 화면을 몇개로 분할할지 div 로 설정
void horizLines(cv::Mat &image, const int w, const int h, const int div)
{
	const bool startBlank = true;
	const int h2 = h / (div * 2);
	int yOffset = 0;

	for (int k = 0; k < div; ++k)
	{
		yOffset = k * h2 * 2;
		if (startBlank)
			yOffset += h2;

		for (int i = 0; i < h2; ++i)
			line(image, Point(0, i + yOffset), Point(w, i + yOffset), Scalar(0, 0, 0));
	}
}


// 화면 크기, w,h
// 화면을 몇개로 분할할지 div 로 설정
void verticalLines(Mat &image, const int w, const int h, const int div)
{
	const bool startBlank = true;
	const int w2 = w / (div * 2);
	int xOffset = 0;

	for (int k = 0; k < div; ++k)
	{
		xOffset = k * w2 * 2;
		if (startBlank)
			xOffset += w2;

		for (int i = 0; i < w2; ++i)
			line(image, Point(i + xOffset, 0), Point(i + xOffset, h), Scalar(0, 0, 0));
	}
}


// 체스보드 출력
void chessboard(cv::Mat &image, const int w, const int h, const int rows, const int cols)
{

}


// 격자무늬 출력
// 스크린 사이즈 (w,h)
// 몇개의 선을 그릴지, (rows, cols)
void gridboard(cv::Mat &image, const int w, const int h, const int rows, const int cols)
{
	// horizontal
	const float hsize = (float)h / (float)rows;
	for (int i = 0; i < rows; ++i)
		line(image, Point(0, (int)(i*hsize)), Point(w, (int)(i*hsize)), Scalar(0, 0, 0));

	// vertical
	const float wsize = (float)w / (float)cols;
	for (int i = 0; i < cols; ++i)
		line(image, Point((int)(i*wsize), 0), Point((int)(i*wsize), h), Scalar(0, 0, 0));
}


// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |				    |
// |					|
// |					|
// 2 ---------- 3
bool OrderedContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst)
{
	Point arr[4];
	Point out[4];

	arr[0] = src[0];
	arr[1] = src[1];
	arr[2] = src[2];
	arr[3] = src[3];

	const bool reval = OrderedContours(arr, out);
	if (reval)
	{
		dst[0] = out[0];
		dst[1] = out[1];
		dst[2] = out[2];
		dst[3] = out[3];
	}
	else
	{
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];
	}

	return reval;
}


// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |				    |
// |					|
// |					|
// 2 ---------- 3
bool OrderedContours2(const vector<cv::Point> &src, OUT vector<cv::Point> &dst)
{
	Point arr[4];
	Point out[4];

	arr[0] = src[0];
	arr[1] = src[1];
	arr[2] = src[2];
	arr[3] = src[3];

	OrderedContours2(arr, out);
	dst[0] = out[0];
	dst[1] = out[1];
	dst[2] = out[2];
	dst[3] = out[3];

	return true;
}

// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |				    |
// |					|
// |					|
// 3 ---------- 2
//
// chIndices 가 NULL이 아니면, src 인덱스가 바뀐 정보를 리턴한다.
void OrderedContours2(const cv::Point src[4], OUT cv::Point dst[4], OUT int *chIndices)
{
	// 가장 큰 박스를 찾는다.
	Point center = src[0];
	center += src[1];
	center += src[2];
	center += src[3];

	// 중심점 계산
	center.x /= 4;
	center.y /= 4;

	bool check[4] = { false, false, false, false };
	bool check2[4] = { false, false, false, false };

	// 시계방향으로 left top 부터 순서대로 0, 1, 2, 3 위치에 포인트가 저장된다.
	for (int k = 0; k < 4; ++k)
	{
		if (!check[0] && (src[k].x < center.x) && (src[k].y < center.y))
		{
			dst[0] = src[k];
			check[0] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[0] = k;
		}
		if (!check[3] && (src[k].x < center.x) && (src[k].y > center.y))
		{
			dst[3] = src[k];
			check[3] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[3] = k;
		}
		if (!check[1] && (src[k].x > center.x) && (src[k].y < center.y))
		{
			dst[1] = src[k];
			check[1] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[1] = k;
		}
		if (!check[2] && (src[k].x > center.x) && (src[k].y > center.y))
		{
			dst[2] = src[k];
			check[2] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[2] = k;
		}
	}

	// 사각형 꼭지점 중, 설정되지 않는 점이 있다면, 
	// 원본(src)에서 빠진 점을 채운다.
	// 정확한 사각형이 되는 것을 보장하지 않는다.
	for (int i = 0; i < 4; ++i)
	{
		if (!check[i])
		{
			for (int k = 0; k < 4; ++k)
			{
				if (!check2[k])
				{
					check[i] = true;
					check2[k] = true;
					if (chIndices)
						chIndices[i] = k;

					dst[i] = src[k];
				}
			}
		}
	}
}


// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |				    |
// |					|
// |					|
// 3 ---------- 2
//
// chIndices 가 NULL이 아니면, src 인덱스가 바뀐 정보를 리턴한다.
bool OrderedContours(const cv::Point src[4], OUT cv::Point dst[4], OUT int *chIndices)
{
	//--------------------------------------------------------------------
	// 4 point cross check
	int crossIndices[3][4] =
	{
		{ 0, 1, 2, 3 },
		{ 0, 2, 1, 3 },
		{ 0, 3, 1, 2 },
	};

	int crossIdx = -1;
	for (int i = 0; i < 3; ++i)
	{
		// line1 = p1-p2 
		// line2 = p3-p4
		const cv::Point p1 = src[crossIndices[i][0]];
		const cv::Point p2 = src[crossIndices[i][1]];
		const cv::Point p3 = src[crossIndices[i][2]];
		const cv::Point p4 = src[crossIndices[i][3]];

		cv::Point2f tmp;
		if (GetIntersectPoint(p1, p2, p3, p4, &tmp))
		{
			crossIdx = i;
			break;
		}
	}

	if (crossIdx < 0)
		return false;

	// p1 ------ p2
	// |              |
	// |              |
	// |              |
	// p4 ------ p3
	const cv::Point p1 = src[crossIndices[crossIdx][0]];
	const cv::Point p2 = src[crossIndices[crossIdx][2]];
	const cv::Point p3 = src[crossIndices[crossIdx][1]];
	const cv::Point p4 = src[crossIndices[crossIdx][3]];


	//--------------------------------------------------------------------
	// 중점 계산
	// 가장 큰 박스를 찾는다.
	Point center = p1;
	center += p2;
	center += p3;
	center += p4;

	// 중심점 계산
	center.x /= 4;
	center.y /= 4;

	Vector3 v1 = Vector3((float)p1.x, (float)p1.y, 0) - Vector3((float)center.x, (float)center.y, 0);
	v1.Normalize();
	Vector3 v2 = Vector3((float)p2.x, (float)p2.y, 0) - Vector3((float)center.x, (float)center.y, 0);
	v2.Normalize();

	// 0 ---------- 1
	// |				    |
	// |					|
	// |					|
	// 3 ---------- 2
	const Vector3 crossV = v1.CrossProduct(v2);
	if (crossV.z > 0)
	{
		dst[0] = p1;
		dst[1] = p2;
		dst[2] = p3;
		dst[3] = p4;

		if (chIndices)
		{
			chIndices[0] = crossIndices[crossIdx][0];
			chIndices[1] = crossIndices[crossIdx][2];
			chIndices[2] = crossIndices[crossIdx][1];
			chIndices[3] = crossIndices[crossIdx][3];
		}
	}
	else
	{
		dst[0] = p2;
		dst[1] = p1;
		dst[2] = p4;
		dst[3] = p3;

		if (chIndices)
		{
			chIndices[0] = crossIndices[crossIdx][2];
			chIndices[1] = crossIndices[crossIdx][0];
			chIndices[2] = crossIndices[crossIdx][3];
			chIndices[3] = crossIndices[crossIdx][1];
		}
	}

	return true;
}


// contours 를 x가 작은 순서대로 정렬한다.
// 0 --- 1 --- 2 --- N
void OrderedLineContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst)
{
	dst = src;
	for (u_int i = 0; i < dst.size() - 1; ++i)
	{
		for (u_int k = i + 1; k < dst.size(); ++k)
		{
			if (dst[i].x > dst[k].x)
			{
				Point tmp = dst[i];
				dst[i] = dst[k];
				dst[k] = tmp;
			}
		}
	}
}


// 선을 그린다.
void DrawLines(Mat &dst, const vector<cv::Point> &lines, const cv::Scalar &color, const int thickness,
	const bool isLoop)
{
	if (lines.size() < 2)
		return;

	for (u_int i = 0; i < lines.size() - 1; ++i)
		line(dst, lines[i], lines[i + 1], color, thickness);

	if (isLoop)
		line(dst, lines[lines.size() - 1], lines[0], color, thickness);
}


void DrawLines(cv::Mat &dst, const cv::Point lines[4], const cv::Scalar &color, const int thickness, 
	const bool isLoop)
{
	for (u_int i = 0; i < 3; ++i)
		line(dst, lines[i], lines[i + 1], color, thickness);

	if (isLoop)
		line(dst, lines[3], lines[0], color, thickness);
}




// hhttp://www.gisdeveloper.co.kr/15
// AP1 - AP2 를 지나는 직선
// AP3 - AP4 를 지나는 직선
// 두 직선의 교점을 찾아 IP에 저장한다.
// 교점이 없다면 false를 리턴한다.
bool GetIntersectPoint(const Point2f& AP1, const Point2f& AP2,
	const Point2f& BP1, const Point2f& BP2, Point2f* IP)
{
	float t;
	float s;
	float under = (BP2.y - BP1.y)*(AP2.x - AP1.x) - (BP2.x - BP1.x)*(AP2.y - AP1.y);
	if (under == 0) return false;

	float _t = (BP2.x - BP1.x)*(AP1.y - BP1.y) - (BP2.y - BP1.y)*(AP1.x - BP1.x);
	float _s = (AP2.x - AP1.x)*(AP1.y - BP1.y) - (AP2.y - AP1.y)*(AP1.x - BP1.x);

	t = _t / under;
	s = _s / under;

	if (t<0.0 || t>1.0 || s<0.0 || s>1.0) return false;
	if (_t == 0 && _s == 0) return false;

	IP->x = AP1.x + t * (float)(AP2.x - AP1.x);
	IP->y = AP1.y + t * (float)(AP2.y - AP1.y);

	return true;
}


bool GetIntersectPoint(const cv::Point& AP1, const cv::Point& AP2,
	const cv::Point& BP1, const cv::Point& BP2, cv::Point2f* IP)
{
	return GetIntersectPoint(cv::Point2f((float)AP1.x, (float)AP1.y),
		cv::Point2f((float)AP2.x, (float)AP2.y),
		cv::Point2f((float)BP1.x, (float)BP1.y),
		cv::Point2f((float)BP2.x, (float)BP2.y), 
		IP);
}


// Skew 변환 행렬을 계산해서 리턴한다.
// scale 값 만큼 contours를 늘려 잡는다.
Mat SkewTransform(const vector<Point> &contours, const int width, const int height, const float scale)
{
	Point arr[4];
	arr[0] = contours[0];
	arr[1] = contours[1];
	arr[2] = contours[2];
	arr[3] = contours[3];
	return SkewTransform(arr, width, height, scale);;
}


cv::Mat SkewTransform(const cv::Point contours[4], const int width, const int height, const float scale)
{
	//vector<Point2f> quad(4);
	Point2f quad[4];
	quad[0] = Point2f(0, 0);
	quad[1] = Point2f((float)width, (float)0);
	quad[2] = Point2f((float)width, (float)height);
	quad[3] = Point2f((float)0, (float)height);


	Point2f ip; // 중점
	if (!GetIntersectPoint(contours[0], contours[2], contours[3], contours[1], &ip))
		return Mat();

	const Point2f offset1 = ((Point2f)contours[0] - ip) * (scale - 1.f);
	const Point2f offset2 = ((Point2f)contours[1] - ip) * (scale - 1.f);
	const Point2f offset3 = ((Point2f)contours[2] - ip) * (scale - 1.f);
	const Point2f offset4 = ((Point2f)contours[3] - ip) * (scale - 1.f);

	//vector<Point2f> screen(4);
	Point2f screen[4];
	screen[0] = (Point2f)contours[0] + offset1;
	screen[1] = (Point2f)contours[1] + offset2;
	screen[2] = (Point2f)contours[2] + offset3;
	screen[3] = (Point2f)contours[3] + offset4;

	const Mat tm = getPerspectiveTransform(screen, quad);
	return tm;
}


// 격자무늬 선을 그린다.
void ShowBoxLines(Mat &dst, const int rows, const int cols, const bool isClear)
{
	const float w = (float)dst.cols;
	const float h = (float)dst.rows;
	const float cx = w / cols;
	const float cy = h / rows;

	if (isClear)
		dst.setTo(Scalar(255, 255, 255));

	for (float x = 0; x <= w; x += cx)
	{
		line(dst, Point((int)x, 0), Point((int)x, (int)h), Scalar(0, 0, 0));
	}

	for (float y = 0; y < h; y += cy)
	{
		line(dst, Point(0, (int)y), Point((int)w, (int)y), Scalar(0, 0, 0));
	}
}


Point2f PointNormalize(const cv::Point &pos)
{
	const float d = (float)cv::norm(pos);
	return Point2f((float)pos.x / d, (float)pos.y / d);
}


Gdiplus::Bitmap* IplImageToBitmap(IplImage *iplImage)
{
	// todo compile error
// 	Gdiplus::Bitmap *bitmap = ::new Gdiplus::Bitmap(640, 480, PixelFormat24bppRGB);
// 	IplImageToBitmap(iplImage, bitmap);
// 	return bitmap;
	return NULL;
}


void IplImageToBitmap(IplImage *iplImage, Gdiplus::Bitmap *out)
{
	// todo compile error
// 	Gdiplus::BitmapData bitmapData;
// 	bitmapData.Width = out->GetWidth();
// 	bitmapData.Height = out->GetHeight();
// 	bitmapData.Stride = 3 * bitmapData.Width;
// 	bitmapData.PixelFormat = out->GetPixelFormat();
// 	bitmapData.Scan0 = (VOID*)iplImage->imageData;
// 	bitmapData.Reserved = NULL;
// 
// 	Gdiplus::Status s = out->LockBits(
// 		&Gdiplus::Rect(0, 0, out->GetWidth(), out->GetHeight()),
// 		Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
// 		PixelFormat24bppRGB, &bitmapData);
// 	if (s == Gdiplus::Ok)
// 		out->UnlockBits(&bitmapData);
}


IplImage* BitmapToIplImage(Gdiplus::Bitmap *bitmap)
{
// 	IplImage* image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
// 	CopyMemory(image->imageData, bitmap->, size);
// 	cvFlip(m_iplImage);
	return NULL;
}


// lineContour로 묶여진 사각형을 정규화 한다.
cRectContour ContourNormalize(cLineContour contourLines[4])
{
	cRectContour contour;
	vector<cv::Point> centers(4);
	for (int i = 0; i < 4; ++i)
	{
		contourLines[i].Normalize();
		centers[i] = contourLines[i].Center();
	}

	// 정규화
	contour.Init(centers);

	cLineContour lines[4]; // 정규화된 사각형을 저장한다.
	for (int i = 0; i < 4; ++i)
	{
		cv::Point center = contourLines[i].Center();
		for (int k = 0; k < 4; ++k)
		{
			// 가장 근접한 line을 찾는다.
			cv::Point dist = (contour.m_contours[k] - center);
			const float len = (float)sqrt(dist.x*dist.x + dist.y*dist.y);
			if (len < 10)
			{
				lines[k] = contourLines[i];
				break;
			}
		}
	}

	// 0 -------- 1
	// |          |
	// |    +     |
	// |          |
	// 3 -------- 2
	//cRectContour retVal;
// 	retVal.m_contours[0] = lines[0].m_contours[0];
// 	retVal.m_contours[1] = lines[1].m_contours[2];
// 	retVal.m_contours[2] = lines[2].m_contours[2];
// 	retVal.m_contours[3] = lines[3].m_contours[0];
	Point posAr[4];
	posAr[0] = lines[0].m_contours[0];
	posAr[1] = lines[1].m_contours[2];
	posAr[2] = lines[2].m_contours[2];
	posAr[3] = lines[3].m_contours[0];

	cRectContour retVal;
	retVal.Init(posAr);
	return retVal;
}


// lineContour로 묶여진 사각형을 정규화 한다.
// [0] center pos
// [1] width/2
// [2] height/2
//          |
//          |
//          |
//   <------X-------->
//          |
//          |
//          |
cRectContour ContourNormalize2(cLineContour contourLines[3])
{
	vector<cv::Point> centers(3);
	for (int i = 0; i < 3; ++i)
	{
		contourLines[i].Normalize();
		centers[i] = contourLines[i].Center();
	}

	const int w1 = abs(contourLines[1].m_contours[0].x - centers[0].x);
	const int w2 = abs(contourLines[1].m_contours[2].x - centers[0].x);
	const int w = max(w1, w2);
	const int h = abs(centers[2].y - centers[0].y);

	// 0 -------- 1
	// |          |
	// |    +     |
	// |          |
	// 3 -------- 2
	cRectContour retVal;
	retVal.m_contours[0] = centers[0] + Point(-w, -h);
	retVal.m_contours[1] = centers[0] + Point(w, -h);
	retVal.m_contours[2] = centers[0] + Point(-w, h);
	retVal.m_contours[3] = centers[0] + Point(w, h);
	retVal.Normalize();

	return retVal;
}


cv::Point GetContourCenter(const vector<cv::Point> &contour)
{
	Point pos(0, 0);
	for each (auto &p in contour)
		pos += p;

	return Point(pos.x / (int)contour.size(), pos.y / (int)contour.size());
}
