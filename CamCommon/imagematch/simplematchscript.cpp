
#include "stdafx.h"
#include "simplematchscript.h"
#include "matchprocessor.h"


using namespace cv;
using namespace cvproc;
using namespace cvproc::imagematch;


cSimpleMatchScript::cSimpleMatchScript(cMatchScript2 *matchScript)
	: m_matchScript(matchScript)
	, m_state(WAIT)
	, m_curIdx(0)
{
	const int maxLen = sizeof(m_matchResult) / sizeof(cMatchResult);
	for (int i = 0; i < maxLen; ++i)
		m_matchResult[i].Clear();

	m_tess.Init("./", "eng", "dictionary.txt");
}

cSimpleMatchScript::~cSimpleMatchScript()
{
}



// aaa, bb, cc, "dd ee"  ff -> aaa, bb, cc, dd ee
int cSimpleMatchScript::attrs(const string &str, OUT string &out)
{
	int i = 0;

	out.reserve(64);

	bool isLoop = true;
	bool isComma = false;
	bool isString = false;
	bool isFirst = true;
	while (isLoop && str[i])
	{
		switch (str[i])
		{
		case '"':
			isFirst = false;
			isString = !isString;
			break;
		case ',': // comma
			if (isString)
			{
				out += ',';
			}
			else
			{
				isComma = true;
				out += ',';
			}
			break;

		case '\r':
		case '\n':
		case ' ': // space
			if (isFirst)
			{
				// nothing~
			}
			else if (isString)
			{
				out += ',';
			}
			else
			{
				if (!isComma)
					isLoop = false;
			}
			break;
		default:
			isFirst = false;
			isComma = false;
			out += str[i];
			break;
		}
		++i;
	}

	return i;
}


// attr - list ->  { id=value  }
void cSimpleMatchScript::attr_list(const string &str)
{
	int offset = 0;
	while (1)
	{
		const int pos = str.find("=", offset);
		if (pos == string::npos)
			break;
		
		string id = str.substr(offset, pos - offset);
		trim(id);
		if (id.empty())
			break;

		offset = pos + 1; // next '='

		// aaa, bb, cc  dd -> aaa, bb, cc 
		string data;
		offset += attrs(&str[offset], data);

		m_commands.push_back(pair<string, string>(id, data));
	}
}


// parse grammar
// var = parameter1, parameter2, parameter3
//
// command
//		- roi=x,y,width,height
//     - bgr=num1,num2,num3
//         - Mat &= Scalar(num1,num2,num3)
//     - scale=num
//         - Mat *= num
//     - gray=0/1
//         - gray convert
//     - hsv=num1,num2,num3,num4,num5,num6
//         - hsv converting, inRange( Scalar(num1,num2,num3), Scalar(num4,num5,num6) )
// - templatematch = filename
//		- @label_select
// - featurematch = filename
//		- @label_select
// - imagematch = labelname
//		- @tree_label
// - img=@capture_select
void cSimpleMatchScript::Parse(const string &script)
{
	m_commands.clear();
	attr_list(script);
}


string cSimpleMatchScript::Match(INOUT cv::Mat &src, OUT cv::Mat &dst, const string &script
	,const string &label_select, const string &capture_select, const string &tree_label
	,const bool showMsg)
	// label_select="", capture_select="", tree_label="", showMsg=true
{
	AutoCSLock cs(m_processCS);

	m_src = src.clone(); // 결과 정보를 출력할 때, 쓰임
	dst = src.clone();
	m_tessImg = Mat();

	vector<Mat> accMat;
	accMat.reserve(8);

	Parse(script);

	const int t1 = timeGetTime();

	string errMsg;
	char value[128];
	cDeSkew deSkew;
	for each (auto &item in m_commands)
	{
		const string cmd = item.first;
		const string param = item.second;

		//----------------------------------------------------------------------
		// roi=x,y,width,height
		if (cmd == "roi")
		{
			cv::Rect roi = { 0,0,0,0 };
			sscanf(param.c_str(), "%d,%d,%d,%d", &roi.x, &roi.y, &roi.width, &roi.height);
			if (roi.area() > 0)
			{
				if (dst.data)
					dst = dst(roi);
				continue;
			}
		}

		//----------------------------------------------------------------------
		// rect=x,y,width,height
		if (cmd == "rect")
		{
			cv::Rect rect = { 0,0,0,0 };
			sscanf(param.c_str(), "%d,%d,%d,%d", &rect.x, &rect.y, &rect.width, &rect.height);
			if (rect.area() > 0)
			{
				cRectContour r(rect);
				r.Draw(dst, Scalar(0, 0, 255), 2);
				continue;
			}
		}

		//----------------------------------------------------------------------
		//     - bgr=num1,num2,num3
		//         - Mat &= Scalar(num1,num2,num3)
		if (cmd == "bgr")
		{
			int bgr[3] = { 0,0,0 };
			sscanf(param.c_str(), "%d,%d,%d", bgr, bgr + 1, bgr + 2);
			if ((bgr[0] != 0) || (bgr[1] != 0) || (bgr[2] != 0))
			{
				if (dst.data)
					dst &= Scalar(bgr[0], bgr[1], bgr[2]);
				continue;
			}
		}


		//----------------------------------------------------------------------
		//     - scale=num
		//         - Mat *= num
		if (cmd == "scale")
		{
			float scale = 0;
			sscanf(param.c_str(), "%f", &scale);
			if (scale != 0)
			{
				if (dst.data)
					dst *= scale;
				continue;
			}
		}

		//----------------------------------------------------------------------
		//     - gray=0/1
		//         - gray convert
		if (cmd == "gray")
		{
			int gray = 0;
			sscanf(param.c_str(), "%d", &gray);
			if (gray)
			{
				if (dst.data && (dst.channels() >= 3))
					cvtColor(dst, dst, CV_BGR2GRAY);
				continue;
			}
		}

		//----------------------------------------------------------------------
		if (cmd == "threshold")
		{
			int  thresh1 = 0;
			sscanf(param.c_str(), "%d", &thresh1);
			if (thresh1 > 0)
			{
				if (dst.data && (dst.channels() >= 3))
					cvtColor(dst, dst, CV_BGR2GRAY);
				threshold(dst, dst, thresh1, 255, CV_THRESH_BINARY);
				continue;
			}
		}

		//----------------------------------------------------------------------
		if (cmd == "adapthreshold")
		{
			double thresh_c = 0;
			int block_size = 0;
			sscanf(param.c_str(), "%d,%lf", &block_size, &thresh_c);
			if (thresh_c > 0)
			{
				if (dst.data && (dst.channels() >= 3))
					cvtColor(dst, dst, CV_BGR2GRAY);
				adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, block_size, thresh_c);
				continue;
			}
		}

		//----------------------------------------------------------------------
		if (cmd == "invert")
		{
			int  thresh = 0;
			sscanf(param.c_str(), "%d", &thresh);
			if (thresh > 0)
			{
				if (dst.data && (dst.channels() >= 3))
					cvtColor(dst, dst, CV_BGR2GRAY);
				threshold(dst, dst, thresh, 255, CV_THRESH_BINARY_INV);
				continue;
			}
		}


		//----------------------------------------------------------------------
		//     - hsv=num1,num2,num3,num4,num5,num6
		//         - hsv converting, inRange( Scalar(num1,num2,num3), Scalar(num4,num5,num6) )
		if (cmd == "hsv")
		{
			int hsv[6] = { 0,0,0, 0,0,0 }; // inrage
			sscanf(param.c_str(), "%d,%d,%d,%d,%d,%d", hsv, hsv + 1, hsv + 2, hsv + 3, hsv + 4, hsv + 5);
			if ((hsv[0] != 0) || (hsv[1] != 0) || (hsv[2] != 0) || (hsv[3] != 0) || (hsv[4] != 0) || (hsv[5] != 0))
			{
				if (dst.data)
				{
					cvtColor(dst, dst, CV_BGR2HSV);
					inRange(dst, cv::Scalar(hsv[0], hsv[1], hsv[2]), cv::Scalar(hsv[3], hsv[4], hsv[5]), dst);
					cvtColor(dst, dst, CV_GRAY2BGR);
					accMat.push_back(dst.clone());
				}
				continue;
			}
		}

		//----------------------------------------------------------------------
		//     - hls=num1,num2,num3,num4,num5,num6
		//         - hsv converting, inRange( Scalar(num1,num2,num3), Scalar(num4,num5,num6) )
		if (cmd == "hls")
		{
			int hsl[6] = { 0,0,0, 0,0,0 }; // inrage
			sscanf(param.c_str(), "%d,%d,%d,%d,%d,%d", hsl, hsl + 1, hsl + 2, hsl + 3, hsl + 4, hsl + 5);
			if ((hsl[0] != 0) || (hsl[1] != 0) || (hsl[2] != 0) || (hsl[3] != 0) || (hsl[4] != 0) || (hsl[5] != 0))
			{
				if (dst.data)
				{
					cvtColor(dst, dst, CV_BGR2HLS);
					inRange(dst, cv::Scalar(hsl[0], hsl[1], hsl[2]), cv::Scalar(hsl[3], hsl[4], hsl[5]), dst);
					cvtColor(dst, dst, CV_GRAY2BGR);
					accMat.push_back(dst.clone());
				}
				continue;
			}
		}

		if (cmd == "acc")
		{
			if (!accMat.empty())
			{
				Mat tmp(accMat.front().rows, accMat.front().cols, accMat.front().flags);
				for each (auto &m in accMat)
					tmp += m;
				dst = tmp;
			}
			continue;
		}


		//----------------------------------------------------------------------
		// img=@capture_select
		if (cmd == "img")
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				dst = imread((string("@capture_select") == value) ? capture_select.c_str() : value);
				if (!dst.data)
				{
					errMsg = "Not Found Capture Image \n";
					return errMsg; // 실패시 종료.
				}

				m_src = dst.clone();
				src = dst.clone();
			}
		}


		//----------------------------------------------------------------------
		// Re Load
		if (cmd == "reload")
		{
			dst = src.clone();
		}


		//----------------------------------------------------------------------
		// hough
		if (cmd == "hough")
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				std::vector<cv::Vec2f> lines;
				cv::HoughLines(dst, lines, 1, 0.1f, 80);
				for each (auto line in lines)
				{
					float rho = line[0];   // 첫 번째 요소는 rho 거리
					float theta = line[1]; // 두 번째 요소는 델타 각도

					const double PI = MATH_PI;
					if (theta < PI / 4.f || theta > 3.f*PI / 4.f) { // 수직 행
						cv::Point pt1((int)(rho / cos(theta)), 0); // 첫 행에서 해당 선의 교차점   
						cv::Point pt2((int)((rho - dst.rows*sin(theta)) / cos(theta)), dst.rows);
						// 마지막 행에서 해당 선의 교차점
						cv::line(dst, pt1, pt2, cv::Scalar(255), 1); // 하얀 선으로 그리기
					}
					else { // 수평 행
						cv::Point pt1(0, (int)(rho / sin(theta))); // 첫 번째 열에서 해당 선의 교차점  
						cv::Point pt2(dst.cols, (int)((rho - dst.cols*cos(theta)) / sin(theta)));
						// 마지막 열에서 해당 선의 교차점
						cv::line(dst, pt1, pt2, cv::Scalar(255), 1); // 하얀 선으로 그리기
					}
				}
			}
		}


		//----------------------------------------------------------------------
		// canny=threshold
		if (cmd == "canny")
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				const int threshold1 = atoi(value);
				cv::Canny(dst, dst, 0, threshold1, 5);
			}
		}


		//----------------------------------------------------------------------
		// findcontours=arcAlpha
		if (cmd == "findcontours")
		{
			float arcAlpha = 0;
			sscanf(param.c_str(), "%f", &arcAlpha);
			if (arcAlpha != 0)
			{
				cRectContour rect;
				vector<vector<cv::Point>> contours;
				cv::findContours(dst, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

				// 좌우로 가장긴 다각형을 찾는다.
				int maxLenIdx = -1;
				int maxLength = 0;
				std::vector<cv::Point> maxLine;
				std::vector<cv::Point> approx;
				for (u_int i = 0; i < contours.size(); i++)
				{
					cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*arcAlpha, true);
					if (approx.empty())
						continue;

					int minX = 10000;
					int maxX = 0;
					for each (auto pos in approx)
					{
						if (minX > pos.x)
							minX = pos.x;
						if (maxX < pos.x)
							maxX = pos.x;
					}

					const int len = abs(maxX - minX);
					if (maxLength < len)
					{
						maxLength = len;
						maxLenIdx = i;
					}
				}

				if (maxLenIdx != -1)
				{
					cv::approxPolyDP(cv::Mat(contours[maxLenIdx]), approx, cv::arcLength(cv::Mat(contours[maxLenIdx]), true)*arcAlpha, true);

					double maxV = 0;
					int idx = 0;
					for (u_int i=0; i < approx.size()-1; ++i)
					{
						const double n = cv::norm(approx[i] - approx[i + 1]);
						if (maxV < n)
						{
							maxV = n;
							idx = i;
						}
					}

					cv::line(dst, approx[idx], approx[idx + 1], Scalar(255), 3);

					for (u_int i = 0; i < approx.size() - 1; ++i)
						cv::line(dst, approx[i], approx[i + 1], Scalar(255), 1);
					cv::line(dst, approx[approx.size() - 1], approx[0], Scalar(255), 1);
				}
			}
		}


		//----------------------------------------------------------------------
		// deskew=arcAlpha
		if (cmd == "deskew")
		{
			float arcAlpha2 = 0;
			int deSkewDebug = 0;
			sscanf(param.c_str(), "%f,%d", &arcAlpha2, &deSkewDebug);
			if (arcAlpha2 != 0)
			{
				deSkew.DeSkew(dst, arcAlpha2, deSkewDebug, true);
			}
		}


		//----------------------------------------------------------------------
		// TemplateMatch
		if (cmd == "templatematch")
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				Mat label = imread((string("@label_select") == value)? label_select.c_str() : value, 
					IMREAD_GRAYSCALE);
				if (!label.data)
				{
					errMsg = "Not Found Label Image \n";
					break;
				}

				cvproc::cTemplateMatch match(0.7f, true, false);
				match.Match(dst, label, "Template Match", &dst);
				break; // 명령어를 실행한 후, 종료한다.
			}
		}

		//----------------------------------------------------------------------
		// FeatureMatch
		if (cmd == "featurematch")
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				Mat label = imread((string("@label_select") == value) ? label_select.c_str() : value,
					IMREAD_GRAYSCALE);
				if (!label.data)
				{
					errMsg = "Not Found Label Image \n";
					break;
				}

				cvproc::cFeatureMatch match(true, false);
				match.Match(dst, label, "Feature Match", &dst);
				break; // 명령어를 실행한 후, 종료한다.
			}
		}

		//----------------------------------------------------------------------
		// ImageMatch
		if ((cmd == "imagematch") || (cmd == "imagematch2"))
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				const string treeLabelName = (string("@tree_label") == value) ? tree_label : value;

				//m_src = src.clone(); // 결과 정보를 출력할 때, 쓰임
				m_matchResult[m_curIdx].Init(m_matchScript, dst, treeLabelName, 0,
					(sParseTree*)m_matchScript->FindTreeLabel(treeLabelName), 
					true, false);

				m_matchResult[m_curIdx].m_traverseType = (cmd == "imagematch")? 0 : 1;

				// 쓰레드로 동작한다.
				m_state = BEGIN_MATCH;
				cMatchProcessor::Get()->Match(m_matchResult[m_curIdx]);

				break; // 명령어를 실행한 후, 종료한다.
			}
		}

		//----------------------------------------------------------------------
		//     - dilate
		if (cmd == "dilate")
		{
			dilate(dst, dst, Mat());
			continue;
		}

		//----------------------------------------------------------------------
		//     - erode
		if (cmd == "erode")
		{
			erode(dst, dst, Mat());
			continue;
		}


		//----------------------------------------------------------------------
		// tess=dictionary file name
		if (cmd == "tess")
		{
			ZeroMemory(value, sizeof(value));
			sscanf(param.c_str(), "%s", value);
			if (value[0] != NULL)
			{
				// deskew 된 영역의 이미지로 문자 인식을 한다.
				string srcStr;
				string fastStr;
				string result;
				int t0, t01, t1, t2;
				float maxFitness;
				if (deSkew.m_tessImg.data)
				{
					m_tessImg = deSkew.m_tessImg;
					deSkew.m_tessImg = Mat();				
					t0 = timeGetTime();
					srcStr = m_tess.Recognize(m_tessImg);
					t01 = timeGetTime();
					result = m_tess.Dictionary2(value, srcStr, fastStr, maxFitness, t1, t2);
				}
				else
				{
					t0 = timeGetTime();
					srcStr = m_tess.Recognize(dst);
					t01 = timeGetTime();
					result = m_tess.Dictionary2(value, srcStr, fastStr, maxFitness, t1, t2);
				}

				if (dst.rows < 1080)
				{
					Mat tmp(Size(dst.cols, 1080), dst.flags);
					tmp.setTo(Scalar(255, 255, 255));
					dst.copyTo(tmp(Rect(0,0,dst.cols, dst.rows)));
					dst = tmp;
				}

				Scalar color(0, 0, 255);
				if (dst.channels() == 1)
					cvtColor(dst, dst, CV_GRAY2BGR);

				putText(dst, common::format("tess source = %s", srcStr.c_str()).c_str(),
					Point(0, 90), 1, 2.f, color, 2);

				putText(dst, common::format("tess FastSearch = %s", fastStr.c_str()).c_str(),
					Point(0, 120), 1, 2.f, color, 2);

				putText(dst, common::format("tess result = %s", result.c_str()).c_str(), 
					Point(0, 150), 1, 2.f, color, 2);

				putText(dst, common::format("tess recognition time = %d, t1 = %d, t2 = %d", t01-t0, t1, t2).c_str(),
					Point(0, 180), 1, 2.f, color, 2);
			}

		}

	}

	const int t2 = timeGetTime();
	if (dst.data && showMsg)
		putText(dst, common::format("time=%d", t2-t1).c_str(), Point(0, 60), 1, 2.f, Scalar(255, 255, 255), 2);

	return errMsg;
}


// return value : -1 = 매칭이 되지 않은 상태
//							0 = 매칭 중
//						    1 = 매칭이 완료된 상태
int cSimpleMatchScript::IsMatchComplete()
{
	RETV(m_state == WAIT, -1);

	if (m_matchResult[m_curIdx].m_isEnd)
	{
		// 매칭이 완료된후, 초기화 된다.
		m_state = WAIT;
		return 1;
	}

	return 0;
}


cMatchResult& cSimpleMatchScript::GetCurrentMatchResult()
{
	return m_matchResult[m_curIdx];
}


// 결과정보를 갱신한다.
bool cSimpleMatchScript::UpdateMatchResult(OUT cv::Mat &dst)
{
	AutoCSLock cs(m_processCS);

	dst = m_src.clone();

	// 결과 정보 출력
	putText(dst, m_matchResult[m_curIdx].m_resultStr.c_str(), Point(0, 30), 1, 2, Scalar(0, 0, 255), 2);

	// 매칭된 이미지를 dst에 출력한다.
	for (int i = 0; i < 1024; ++i)
	{
		if (m_matchResult[m_curIdx].m_data[i].result > 0)
		{
			// Matching Rect 출력
			cRectContour rect;
			if ((m_matchScript->m_nodeTable[i]->attrs["type"] == "featurematch") // Feature Match, ocrmatch
				|| (m_matchScript->m_nodeTable[i]->attrs["type"] == "ocrmatch"))
			{
				vector<cv::Point> pts(m_matchResult[m_curIdx].m_data[i].matchRect2, 
					m_matchResult[m_curIdx].m_data[i].matchRect2 + 4);
				rect.Init(pts);
				rect.Draw(dst, Scalar(0, 0, 255), 2);
			}
			else // TemplateMatch
			{
				rect.Init(m_matchResult[m_curIdx].m_data[i].matchRect);
				rect.Draw(dst, Scalar(0, 0, 255), 2);
			}

			// 매칭된 이미지 경로 출력
			putText(dst, m_matchScript->m_nodeTable[i]->attrs["id"], rect.m_contours[0] + Point(0, 30), 1, 2.f, Scalar(0, 0, 255), 2);
			putText(dst, common::format("time=%d", timeGetTime() - m_matchResult[m_curIdx].m_beginTime).c_str(), Point(0, 60), 1, 2.f, Scalar(255, 255, 255), 2);
		}
	}

	NextIndex();

	return true;
}


// 매칭 강제 종료.
void cSimpleMatchScript::TerminiateMatch()
{
	m_matchResult[m_curIdx].TerminateMatch();
	NextIndex();
}


void cSimpleMatchScript::NextIndex()
{
	const int maxLen = sizeof(m_matchResult) / sizeof(cMatchResult);
	m_curIdx = (++m_curIdx) % maxLen;
}
