#pragma once


#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
#endif

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cvaux.h>
#include <opencv2/calib3d.hpp>
//#include <opencv2/xfeatures2d.hpp>


#include "../Common/common.h"
using namespace common;

#include "../Network/network.h"

#include "../Tess/tess.h"


using std::pair;


struct sSharedData
{
	int count;
	cv::Point2f pts[3];
};




#include "linecontour.h"
#include "camera.h"
#include "config.h"
#include "RecognitionConfig.h"
#include "recognitionEdgeconfig.h"
#include "detectpoint.h"
#include "detectpoint.h"
#include "rectcontour.h"
#include "rectcontour2.h"
#include "squarecontour.h"
#include "roi.h"
#include "screen.h"
#include "sharedmemcommunication.h"
#include "wndmsgcommunication.h"
#include "udpcommunication.h"
#include "dxcapture.h"
#include "movie.h"
#include "pidcontrol.h"
#include "skewdetect.h"
#include "streamingsender.h"
#include "streamingreceiver.h"
#include "streamingviewer.h"
#include "searchpoint.h"
#include "thresholdfinder.h"
#include "LEDCalibrater.h"
#include "LEDBlinkCapture.h"
#include "videowriter.h"
#include "qrcode.h"
#include "arqrcode.h"
#include "fps.h"
#include "opencvgdiplus.h"
#include "screencapture.h"
#include "frapscapture.h"
#include "templatematch.h"
#include "featurematch.h"
#include "convertimage.h"
#include "deskew.h"


#include "imagematch/imagematch.h"
//#include "imagematch/imagematchparser.h"
#include "imagematch/matchparser.h"
//#include "imagematch/matchscript.h"
#include "imagematch/matchscript2.h"
#include "imagematch/graphscript.h"
#include "imagematch/matchthread.h"
#include "imagematch/matchprocessor.h"
#include "imagematch/flowcontrol.h"
#include "imagematch/matchmanager.h"
#include "imagematch/simplematchscript.h"
#include "imagematch/MatchResult.h"

#include "util.h"


 #ifdef _DEBUG
	#pragma comment(lib, "opencv_core310d.lib")
	#pragma comment(lib, "opencv_imgcodecs310d.lib")
	#pragma comment(lib, "opencv_features2d310d.lib")
	#pragma comment(lib, "opencv_videoio310d.lib")
	#pragma comment(lib, "opencv_highgui310d.lib")
	#pragma comment(lib, "opencv_imgproc310d.lib")
 	#pragma comment(lib, "opencv_flann310d.lib")
 	#pragma comment(lib, "opencv_xfeatures2d310d.lib")
	#pragma comment(lib, "opencv_calib3d310d.lib")
#else
	#pragma comment(lib, "opencv_core310.lib")
	#pragma comment(lib, "opencv_imgcodecs310.lib")
	#pragma comment(lib, "opencv_features2d310.lib")
	#pragma comment(lib, "opencv_videoio310.lib")
	#pragma comment(lib, "opencv_highgui310.lib")
	#pragma comment(lib, "opencv_imgproc310.lib")
	#pragma comment(lib, "opencv_flann310.lib")
	#pragma comment(lib, "opencv_xfeatures2d310.lib")
	#pragma comment(lib, "opencv_calib3d310.lib")
#endif
