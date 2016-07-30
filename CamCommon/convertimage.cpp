
#include "stdafx.h"
#include "convertimage.h"

using namespace cvproc;
using namespace cv;


cConvertImage::cConvertImage()
{
}

cConvertImage::~cConvertImage()
{
}


// option
//		- roi=x,y,width,height
//     - bgr=num1,num2,num3
//         - Mat &= Scalar(num1,num2,num3)
//     - scale=num
//         - Mat *= num
//     - gray=0/1
//         - gray convert
//     - hsv=num1,num2,num3,num4,num5,num6
//         - hsv converting, inRange( Scalar(num1,num2,num3), Scalar(num4,num5,num6) )
void cConvertImage::Convert(const cv::Mat &src, OUT cv::Mat &dst, const string &option)
{
	RET(!src.data);

	vector<string> argv;
	common::tokenizer(option, " ", "", argv);
	
	dst = src.clone();

	const u_int argc = argv.size();
	for (u_int i = 0; i < argc; ++i)
	{
		trim(argv[i]);

		cv::Rect roi = { 0,0,0,0 };
		sscanf(argv[i].c_str(), "roi=%d,%d,%d,%d", &roi.x, &roi.y, &roi.width, &roi.height);
		if (roi.area() > 0)
		{
			dst = dst(roi);
			continue;
		}

		int bgr[3] = { 0,0,0 };
		sscanf(argv[i].c_str(), "bgr=%d,%d,%d", bgr, bgr + 1, bgr + 2);
		if ((bgr[0] != 0) || (bgr[1] != 0) || (bgr[2] != 0))
		{
			dst &= Scalar(bgr[0], bgr[1], bgr[2]);
			continue;
		}

		float scale = 0;
		sscanf(argv[i].c_str(), "scale=%f", &scale);
		if (scale != 0)
		{
			dst *= scale;
			continue;
		}

		int gray = 0;
		sscanf(argv[i].c_str(), "gray=%d", &gray);
		if (gray)
		{
			if (dst.channels() >= 3)
				cvtColor(dst, dst, CV_BGR2GRAY);
			continue;
		}

		int  thresh = 0;
		sscanf(argv[i].c_str(), "invert=%d", &thresh);
		if (thresh > 0)
		{
			if (dst.channels() >= 3)
				cvtColor(dst, dst, CV_BGR2GRAY);
			threshold(dst, dst, thresh, 255, CV_THRESH_BINARY_INV);
			continue;
		}

		int hsv[6] = { 0,0,0, 0,0,0 }; // inrage
		sscanf(argv[i].c_str(), "hsv=%d,%d,%d,%d,%d,%d", hsv, hsv + 1, hsv + 2, hsv + 3, hsv + 4, hsv + 5);
		if ((hsv[0] != 0) || (hsv[1] != 0) || (hsv[2] != 0) || (hsv[3] != 0) || (hsv[4] != 0) || (hsv[5] != 0))
		{
			cvtColor(dst, dst, CV_BGR2HSV);
			inRange(dst, cv::Scalar(hsv[0], hsv[1], hsv[2]), cv::Scalar(hsv[3], hsv[4], hsv[5]), dst);
			cvtColor(dst, dst, CV_GRAY2BGR);
			GaussianBlur(dst, dst, cv::Size(9, 9), 2, 2);
			continue;
		}
	}
}
