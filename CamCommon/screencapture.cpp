
#include "stdafx.h"
#include "screencapture.h"

using namespace cvproc;
using namespace cv;


cScreenCapture::cScreenCapture() 
	: m_prtScrSleepTime(100)
	, m_log(false)
{
}
cScreenCapture::~cScreenCapture()
{
}


// 초기화, 쓰레드로 실행할지 여부를 결정.
bool cScreenCapture::Init(const bool isThread)
{
	// 아직 구현안됨.
	return true;
}


// 스크린을 캡쳐 하고, 영상을 리턴한다. 
// m_prtScrSleepTime 밀리세컨드만큼 딜레이된다. defualt=100 milliseconds
cv::Mat cScreenCapture::ScreenCapture(const bool isAlt)
{
	PressScreenShot(isAlt);
	Mat img = GetClipboardImage();
	return img;
}


// PrintScreen 키를 누른다.
void cScreenCapture::PressScreenShot(const bool isAlt)
{
	if (isAlt)
	{
		INPUT input;
		WORD vkey = VK_LMENU; // see link below
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		input.ki.wVk = vkey;
		input.ki.dwFlags = KEYEVENTF_SCANCODE;
		SendInput(1, &input, sizeof(INPUT));
	}

	INPUT input;
	WORD vkey = VK_SNAPSHOT; // see link below
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(m_prtScrSleepTime);

	input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	if (isAlt)
	{
		INPUT input;
		WORD vkey = VK_LMENU; // see link below
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		input.ki.wVk = vkey;
		input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		SendInput(1, &input, sizeof(INPUT));
	}

	Sleep(m_prtScrSleepTime);
}


// 클립보드 내용 가져와서 리턴
cv:: Mat cScreenCapture::GetClipboardImage()
{
	::OpenClipboard(NULL);

// 	std::cout << "Format Bitmap: " << IsClipboardFormatAvailable(CF_BITMAP) << "\n";
// 	std::cout << "Format DIB: " << IsClipboardFormatAvailable(CF_DIB) << "\n";
// 	std::cout << "Format DIBv5: " << IsClipboardFormatAvailable(CF_DIBV5) << "\n";

	if (!IsClipboardFormatAvailable(CF_BITMAP) &&
		!IsClipboardFormatAvailable(CF_DIB) &&
		!IsClipboardFormatAvailable(CF_DIBV5))
		return Mat();

	HANDLE hClipboard = GetClipboardData(CF_DIB);
	if (!hClipboard)
		hClipboard = GetClipboardData(CF_DIBV5);
	if (!hClipboard)
		hClipboard = GetClipboardData(CF_BITMAP);

	Mat reval;
	if (hClipboard != NULL && hClipboard != INVALID_HANDLE_VALUE)
	{
		void* dib = GlobalLock(hClipboard);
		if (!dib)
			return Mat();

		DIB *info = reinterpret_cast<DIB*>(dib);
		BMP bmp = { 0 };
		bmp.header.type = 0x4D42;
		bmp.header.offset = 54;
		bmp.header.bfSize = info->biSizeImage + bmp.header.offset;
		bmp.dib = *info;

		if (m_log)
		{
			stringstream ss;
			ss << "Type: " << std::hex << bmp.header.type << std::dec << "\n";
 			ss << "bfSize: " << bmp.header.bfSize << "\n";
 			ss << "Reserved: " << bmp.header.reserved << "\n";
			ss << "Offset: " << bmp.header.offset << "\n";
			ss << "biSize: " << bmp.dib.biSize << "\n";
			ss << "Width: " << bmp.dib.biWidth << "\n";
			ss << "Height: " << bmp.dib.biHeight << "\n";
			ss << "Planes: " << bmp.dib.biPlanes << "\n";
			ss << "Bits: " << bmp.dib.biBitCount << "\n";
			ss << "Compression: " << bmp.dib.biCompression << "\n";
			ss << "Size: " << bmp.dib.biSizeImage << "\n";
			ss << "X-res: " << bmp.dib.biXPelsPerMeter << "\n";
			ss << "Y-res: " << bmp.dib.biYPelsPerMeter << "\n";
			ss << "ClrUsed: " << bmp.dib.biClrUsed << "\n";
			ss << "ClrImportant: " << bmp.dib.biClrImportant << "\n";
			dbg::Log("%s", ss.str());
		}

		reval = Mat(bmp.dib.biHeight, bmp.dib.biWidth, CV_8UC4);
		CopyMemory(reval.data, (info + 1), bmp.dib.biSizeImage);
		flip(reval, reval, 0);

		GlobalUnlock(dib);
	}

	CloseClipboard();
	return reval;
}

