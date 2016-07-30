//
// 2016-05-23, jjuiddong
// 스크린 캡쳐 후, Clipboard 에서 이미지 정보를 가져온다.
//
#pragma once


namespace cvproc
{

	class cScreenCapture
	{
	public:
		cScreenCapture();
		virtual ~cScreenCapture();

		typedef struct
		{
			std::uint32_t biSize;
			std::int32_t  biWidth;
			std::int32_t  biHeight;
			std::uint16_t  biPlanes;
			std::uint16_t  biBitCount;
			std::uint32_t biCompression;
			std::uint32_t biSizeImage;
			std::int32_t  biXPelsPerMeter;
			std::int32_t  biYPelsPerMeter;
			std::uint32_t biClrUsed;
			std::uint32_t biClrImportant;
		} DIB;

		typedef struct
		{
			std::uint16_t type;
			std::uint32_t bfSize;
			std::uint32_t reserved;
			std::uint32_t offset;
		} HEADER;

		typedef struct
		{
			HEADER header;
			DIB dib;
		} BMP;

		bool Init(const bool isThread=false);
		cv::Mat ScreenCapture(const bool isAlt=false);


	protected:
		void PressScreenShot(const bool isAlt);
		cv::Mat GetClipboardImage();


	public:
		int m_prtScrSleepTime;
		bool m_log;
	};

}
