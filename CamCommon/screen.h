//
// 화면에 표시되는 매인 스크린 정보를 관리한다.
// 화면 해상도, 카메라에서 인식한 스크린 크기 등의 정보를 관리한다.
// 
#pragma once


class cCapture;
class cScreen
{
public:
	cScreen();
	virtual ~cScreen();

	void Init(const cv::Rect &screenResolution, const vector<cv::Point> &screenContour);
	void InitResolution(const cv::Rect &screenResolution);

	void DetectScreen(cCamera &camera, const float minScreenArea);
	void SetScreenContour(const cv::Point contours[4]);

	void ShowChessBoard(const int rows, const int cols);
	cv::Point2f ShowPingpong(const cv::Point2f &ballPos, const cv::Point2f &velocity, const cv::Size2f &ballSize);
	void ShowBoxLines(const int rows, const int cols, const bool isClear=false);
	void ShowPoints(const vector<cv::Point2f> &points, const cv::Size2f &size=cv::Size2f(20,20), 
		const cv::Scalar &color = cv::Scalar(0, 0, 0),
		const bool isClear = false, const cv::Scalar &bgColor=cv::Scalar(255,255,255));

	cv::Mat SkewTransform(const int width, const int height, const float scale=1.f);
	void CloseScreen();

	int GetWidth();
	int GetHeight();
	int GetRecogWidth();
	int GetRecogHeight();
	pair<float, float> GetResolutionRecognitionRate();

	const cv::Rect& GetScreenResolution() const;
	const vector<cv::Point>& GetScreenContour() const;
	cv::Mat& GetScreen();


protected:
	bool m_show;
	cv::Rect m_screenResolution; // {0,0,w,h}
	vector<cv::Point> m_screenContour;	// 인식된 스크린의 크기 (인덱스 순서)
	// 0 ---------- 1
	// |            |
	// |            |
	// |            |
	// 2 ---------- 3
	//

	cv::Mat m_screen;
	int m_oldT;
	cv::Point2f m_ballPos;
	cv::Point2f m_ballVelocity;
};


inline const cv::Rect& cScreen::GetScreenResolution() const { return m_screenResolution; }
inline const vector<cv::Point>& cScreen::GetScreenContour() const { return m_screenContour; }
inline cv::Mat& cScreen::GetScreen() { return m_screen;  }
