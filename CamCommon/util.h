
#pragma once



double angle(cv::Point pt1, const cv::Point &pt2, const cv::Point &pt0);
void setLabel(cv::Mat& im, const std::string &label, const std::vector<cv::Point>& contour, const cv::Scalar &color = cv::Scalar(0, 0, 0));
void horizLines(cv::Mat &image, const int w, const int h, const int div);
void verticalLines(cv::Mat &image, const int w, const int h, const int div);
void chessboard(cv::Mat &image, const int w, const int h, const int rows, const int cols);
void gridboard(cv::Mat &image, const int w, const int h, const int rows, const int cols);
void ShowBoxLines(cv::Mat &dst, const int rows, const int cols, const bool isClear = false);
cv::Mat SkewTransform(const vector<cv::Point> &contours, const int width, const int height, const float scale);
cv::Mat SkewTransform(const cv::Point contours[4], const int width, const int height, const float scale);

bool OrderedContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);
bool OrderedContours(const cv::Point src[4], OUT cv::Point dst[4], OUT int *chIndices = NULL);
bool OrderedContours2(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);
void OrderedContours2(const cv::Point src[4], OUT cv::Point dst[4], OUT int *chIndices=NULL);
void OrderedLineContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);
void DrawLines(cv::Mat &dst, const vector<cv::Point> &lines, const cv::Scalar &color = cv::Scalar(0, 0, 0),
	const int thickness = 1, const bool isLoop = true);
void DrawLines(cv::Mat &dst, const cv::Point lines[4], const cv::Scalar &color = cv::Scalar(0, 0, 0),
	const int thickness = 1, const bool isLoop = true);


namespace Gdiplus { class Bitmap; }
Gdiplus::Bitmap* IplImageToBitmap(IplImage *iplImage);
void IplImageToBitmap(IplImage *iplImage, Gdiplus::Bitmap *out);

IplImage* BitmapToIplImage(Gdiplus::Bitmap *bitmap);



cv::Point2f PointNormalize(const cv::Point &pos);

cRectContour ContourNormalize(cLineContour contourLines[4]);
cRectContour ContourNormalize2(cLineContour contourLines[3]);
cv::Point GetContourCenter(const vector<cv::Point> &contour);

bool GetIntersectPoint(const cv::Point2f& AP1, const cv::Point2f& AP2, const cv::Point2f& BP1, const cv::Point2f& BP2, cv::Point2f* IP);
bool GetIntersectPoint(const cv::Point& AP1, const cv::Point& AP2, const cv::Point& BP1, const cv::Point& BP2, cv::Point2f* IP);


