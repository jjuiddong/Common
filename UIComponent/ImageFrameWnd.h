//
// 2016-07-03, jjuiddong
// 이미지 출력 전용 윈도우
//
#pragma once


class CImageFrameWnd : public CMiniFrameWnd
{
protected:
	class cImageView : public CView
	{
	public:
		cImageView() : m_image(NULL) {}
		virtual ~cImageView() {
			SAFE_DELETE(m_image);
		}

		bool Init(CWnd *parentWnd, const CRect &rect)
		{
			if (!Create(NULL, L"ImageWnd", WS_VISIBLE | WS_CHILDWINDOW, rect, parentWnd, 100))
				return false;
			return true;
		}

		bool ShowImage(const string &fileName)
		{
			SAFE_DELETE(m_image);
			m_image = Gdiplus::Image::FromFile(str2wstr(fileName).c_str());
 			InvalidateRect(0);
			return true;
		}

		bool ShowImage(const cv::Mat &img)
		{
			SAFE_DELETE(m_image);
			if (img.data)
				m_image = CGdiPlus::CopyMatToBmp(img);
			//InvalidateRect(0);
			CDC *pDC = GetDC();
			OnDraw(pDC);
			ReleaseDC(pDC);
			return true;
		}

		virtual void OnDraw(CDC* pDC)
		{
			Gdiplus::Graphics g(*pDC);
			if (m_image)
			{
				CRect cr;
				GetClientRect(cr);
				// 윈도우 크기가 이미지 보다 크다면, 이미지 크기대로 출력한다.
				// 이미지 크기가 윈도우 보다 크다면, 윈도우 크기대로 출력한다.
				const int w = MIN((int)m_image->GetWidth(), cr.Width());
				const int h = MIN((int)m_image->GetHeight(), cr.Height());
				g.DrawImage(m_image, 0, 0, w, h);
			}
		}

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			if (nChar == VK_ESCAPE)
			{
				CImageFrameWnd *parent = (CImageFrameWnd*)GetParent();
				parent->ShowWindow(SW_HIDE);
			}
		}

		BOOL OnEraseBkgnd(CDC* pDC)
		{
			return TRUE;
		}


		string m_fileName;
		Gdiplus::Image *m_image;
		DECLARE_MESSAGE_MAP()
	};



public:
	CImageFrameWnd();
	virtual ~CImageFrameWnd();

	bool Init(const string &windowName, const string &fileName, const CRect &initialRect, CWnd *parentWnd, const bool isAutoSize=false);
	bool Init(const string &windowName, const cv::Mat &img, const CRect &initialRect, CWnd *parentWnd, const bool isAutoSize = false);
	bool ShowImage(const string &fileName);
	bool ShowImage(const cv::Mat &img);
	void SetAutoSize(const bool isAutoSize);


protected:
	void Initialize(const string &windowName, const CRect &initialRect, CWnd *parentWnd, const bool isAutoSize);
	void ReCalcWindowSize();


protected:
	string m_windowName;
	bool m_isAutoSize;
	cImageView *m_imageView;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
