
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
string cSimpleMatchScript::Match(INOUT cv::Mat &src, OUT cv::Mat &dst, const string &script
	,const string &label_select, const string &capture_select, const string &tree_label) // label_select="", tree_label=""
{
	//RETV(!src.data, "data is empty");

	dst = src.clone();
	m_tessImg = Mat();

	// 공백, 개행문자 기준으로 분리
	vector<string> argv0;
	common::tokenizer(script, " ", "", argv0);
	vector<string> argv;
	for each (auto arg in argv0)
		common::tokenizer(arg, "\r\n", "", argv);

	const int t1 = timeGetTime();

	Point skewPoint1, skewPoint2; // skew 명령어로 선택된 skew 위치 
	string errMsg;
	char value[128];
	char value2[128];
	const u_int argc = argv.size();
	for (u_int i = 0; i < argc; ++i)
	{
		trim(argv[i]);

		//----------------------------------------------------------------------
		// roi=x,y,width,height
		cv::Rect roi = { 0,0,0,0 };
		sscanf(argv[i].c_str(), "roi=%d,%d,%d,%d", &roi.x, &roi.y, &roi.width, &roi.height);
		if (roi.area() > 0)
		{
			if (dst.data)
				dst = dst(roi);
			continue;
		}


		//----------------------------------------------------------------------
		//     - bgr=num1,num2,num3
		//         - Mat &= Scalar(num1,num2,num3)
		int bgr[3] = { 0,0,0 };
		sscanf(argv[i].c_str(), "bgr=%d,%d,%d", bgr, bgr + 1, bgr + 2);
		if ((bgr[0] != 0) || (bgr[1] != 0) || (bgr[2] != 0))
		{
			if (dst.data)
				dst &= Scalar(bgr[0], bgr[1], bgr[2]);
			continue;
		}


		//----------------------------------------------------------------------
		//     - scale=num
		//         - Mat *= num
		float scale = 0;
		sscanf(argv[i].c_str(), "scale=%f", &scale);
		if (scale != 0)
		{
			if (dst.data)
				dst *= scale;
			continue;
		}


		//----------------------------------------------------------------------
		//     - gray=0/1
		//         - gray convert
		int gray = 0;
		sscanf(argv[i].c_str(), "gray=%d", &gray);
		if (gray)
		{
			if (dst.data && (dst.channels() >= 3))
				cvtColor(dst, dst, CV_BGR2GRAY);
			continue;
		}


		//----------------------------------------------------------------------
		int  thresh = 0;
		sscanf(argv[i].c_str(), "invert=%d", &thresh);
		if (thresh > 0)
		{
			if (dst.data && (dst.channels() >= 3))
				cvtColor(dst, dst, CV_BGR2GRAY);
			threshold(dst, dst, thresh, 255, CV_THRESH_BINARY_INV);
			continue;
		}


		//----------------------------------------------------------------------
		//     - hsv=num1,num2,num3,num4,num5,num6
		//         - hsv converting, inRange( Scalar(num1,num2,num3), Scalar(num4,num5,num6) )
		int hsv[6] = { 0,0,0, 0,0,0 }; // inrage
		sscanf(argv[i].c_str(), "hsv=%d,%d,%d,%d,%d,%d", hsv, hsv + 1, hsv + 2, hsv + 3, hsv + 4, hsv + 5);
		if ((hsv[0] != 0) || (hsv[1] != 0) || (hsv[2] != 0) || (hsv[3] != 0) || (hsv[4] != 0) || (hsv[5] != 0))
		{
			if (dst.data)
			{
				cvtColor(dst, dst, CV_BGR2HSV);
				inRange(dst, cv::Scalar(hsv[0], hsv[1], hsv[2]), cv::Scalar(hsv[3], hsv[4], hsv[5]), dst);
				cvtColor(dst, dst, CV_GRAY2BGR);
				//GaussianBlur(dst, dst, cv::Size(9, 9), 2, 2);
			}
			continue;
		}


		//----------------------------------------------------------------------
		// img=@capture_select
		ZeroMemory(value, sizeof(value));
		sscanf(argv[i].c_str(), "img=%s", value);
		if (value[0] != NULL)
		{
			src = imread((string("@capture_select") == value) ? capture_select.c_str() : value);
			if (!src.data)
			{
				errMsg = "Not Found Capture Image \n";
				return errMsg; // 실패시 종료.
			}
			dst = src.clone();
		}


		//----------------------------------------------------------------------
		// hough
		ZeroMemory(value, sizeof(value));
		sscanf(argv[i].c_str(), "hough=%s", value);
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


		//----------------------------------------------------------------------
		// canny=threshold2
		ZeroMemory(value, sizeof(value));
		sscanf(argv[i].c_str(), "canny=%s", value);
		if (value[0] != NULL)
		{
			const int threshold1 = atoi(value);
			cv::Canny(dst, dst, 0, threshold1, 5);
		}


		//----------------------------------------------------------------------
		// findcontours=arcAlpha
		float arcAlpha = 0;
		sscanf(argv[i].c_str(), "findcontours=%f", &arcAlpha);
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


		//----------------------------------------------------------------------
		// skew=arcAlpha
		float arcAlpha2 = 0;
		int skewDebug = 0;
		sscanf(argv[i].c_str(), "skew=%f,%d", &arcAlpha2, &skewDebug);
		if (arcAlpha2 != 0)
		{
			Mat tmp = dst.clone();

			if (dst.data && (dst.channels() >= 3))
				cvtColor(dst, dst, CV_BGR2GRAY);
			threshold(dst, dst, 20, 255, CV_THRESH_BINARY_INV);
			cv::Canny(dst, dst, 0, 100, 5);

			vector<vector<cv::Point>> contours;
			cv::findContours(dst, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

			// 좌우로 가장긴 다각형을 찾는다.
			int maxLenIdx = -1;
			int maxLength = 0;
			std::vector<cv::Point> maxLine;
			std::vector<cv::Point> approx;
			for (u_int i = 0; i < contours.size(); i++)
			{
				cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*arcAlpha2, true);
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

			if (maxLenIdx == -1)
				continue; // error occur

			// 좌우로 가장 긴 다각형의 기울어진 각도를 계산해서, 원본 이미지를 수평이 되게 회전시킨다.
			cv::approxPolyDP(cv::Mat(contours[maxLenIdx]), approx, cv::arcLength(cv::Mat(contours[maxLenIdx]), true)*arcAlpha2, true);
			
			double maxV = 0, maxV2 = 0;
			int idx1 = 0, idx2 = 0;
			// 가장 긴 라인을 구한다. 마지막 circular line 은 제외
			for (u_int i = 0; i < approx.size()-1; ++i)
			{
				const int i2 = (i + 1) % approx.size(); // circular rect
				const double n = cv::norm(approx[i] - approx[i2]);
				if (maxV < n)
				{
					maxV = n;
					idx1 = i;
				}
			}

			// 두 번째로 긴 라인을 구한다. 가장 긴 라인과 수평이어야 한다.
			
			// 먼저 각 라인의 벡터를 구한다.
			vector<Vector3> lineDirs(approx.size());
			for (u_int i = 0; i < approx.size(); ++i)
			{
				const int i2 = (i + 1) % approx.size(); // circular rect
				const Vector3 p1 = Vector3((float)approx[i].x, (float)approx[i].y, 0);
				const Vector3 p2 = Vector3((float)approx[i2].x, (float)approx[i2].y, 0);
				Vector3 v = p2 - p1;
				v.Normalize();
				lineDirs[i] = v;
			}

			typedef pair<double, int> LineType; // angle, index
			vector< LineType > lines;
			lines.reserve(approx.size());

			// 서로 붙어있는 라인일 경우, 무시한다. (y  값으로 판단한다.)
			const int minimumLength = 20;
			for (u_int i = 0; i < approx.size(); ++i)
			{
				if (i == idx1)
					continue; // max 값 제외

				const int i2 = (i + 1) % approx.size(); // circular rect
				const double len = cv::norm(approx[i] - approx[i2]);
				const float angle = abs(lineDirs[idx1].DotProduct(lineDirs[i]));
				if (angle > 0.8f)
					lines.push_back(LineType(angle, i));
			}

			// 가장 긴 라인 추가
			lines.push_back(LineType(1, idx1));

			// 각 라인끼리, 수평이면서, 거리도 긴 쌍을 구한다.
			struct sType {
				float angle;
				int idx1;
				int idx2;
			};

			vector<sType> linePairs;
			linePairs.reserve(lines.size() * lines.size());

			for (u_int i = 0; i < lines.size()-1; ++i)
			{
				for (u_int k = i+1; k < lines.size(); ++k)
				{
					// y 차이가 일정크기 이상이어야 한다.
					const int i1 = lines[i].second;
					const int i2 = lines[k].second;
					const int i12 = (i1 + 1) % approx.size(); // circular rect
					const int i22 = (i2 + 1) % approx.size(); // circular rect
					const int left1 = (approx[i1].x < approx[i12].x) ? i1 : i12;
					const int left2 = (approx[i2].x < approx[i22].x) ? i2 : i22;
 					if (abs(approx[left1].y - approx[left2].y) < minimumLength)
 						continue;

					sType type;
					type.angle = abs(lineDirs[ lines[i].second ].DotProduct(lineDirs[lines[k].second]));
					type.idx1 = lines[i].second;
					type.idx2 = lines[k].second;
					linePairs.push_back(type);
				}
			}

			// 길면서, 서로 수평인 두 라인을 구한다.
			std::sort(linePairs.begin(), linePairs.end(),
				[](const sType &a, sType &b)
			{
				return a.angle > b.angle;
			} );

			if (!linePairs.empty())
			{
				idx1 = linePairs.front().idx1;
				idx2 = linePairs.front().idx2;
			}
 
 			const int i1 = idx1;
 			const int i12 = (idx1 + 1) % approx.size();
			const int i2 = idx2;
			const int i22 = (idx2 + 1) % approx.size();

			// debug display
			cvtColor(dst, dst, CV_GRAY2BGR);
			for (u_int i = 0; i < approx.size() - 1; ++i)
 				cv::line(dst, approx[i], approx[i + 1], Scalar(255,255,255), 2);
 			cv::line(dst, approx[approx.size() - 1], approx[0], Scalar(255,255,255), 2);
			cv::line(dst, approx[i1], approx[i12], Scalar(255, 0, 0), 3);
			cv::line(dst, approx[i2], approx[i22], Scalar(0, 0, 255), 3);

			Vector3 p1 = (approx[i1].x < approx[i12].x) ? Vector3((float)approx[i1].x, (float)approx[i1].y, 0) : Vector3((float)approx[i12].x, (float)approx[i12].y, 0);
			Vector3 p2 = (approx[i1].x > approx[i12].x) ? Vector3((float)approx[i1].x, (float)approx[i1].y, 0) : Vector3((float)approx[i12].x, (float)approx[i12].y, 0);
			Vector3 p3 = (approx[i2].x < approx[i22].x) ? Vector3((float)approx[i2].x, (float)approx[i2].y, 0) : Vector3((float)approx[i22].x, (float)approx[i22].y, 0); 
			Vector3 v = p2 - p1;
			v.Normalize();
			double angle = RAD2ANGLE(acos(v.DotProduct(Vector3(1, 0, 0))));
			if (v.y < 0)
				angle = -angle;

			skewPoint1 = Point((int)p1.x, (int)p1.y);
			skewPoint2 = Point((int)p3.x, (int)p3.y);
			const Mat affine_matrix = getRotationMatrix2D(Point((int)p1.x, (int)p1.y), angle, 1);

			if (skewDebug == 0)
				warpAffine(tmp, dst, affine_matrix, dst.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar::all(255));
		}



		//----------------------------------------------------------------------
		// TemplateMatch
		ZeroMemory(value, sizeof(value));
		sscanf(argv[i].c_str(), "templatematch=%s", value);
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

		//----------------------------------------------------------------------
		// FeatureMatch
		ZeroMemory(value, sizeof(value));
		sscanf(argv[i].c_str(), "featurematch=%s", value);
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

		//----------------------------------------------------------------------
		// ImageMatch
		ZeroMemory(value, sizeof(value));
		ZeroMemory(value2, sizeof(value2));
		sscanf(argv[i].c_str(), "imagematch=%s", value);
		sscanf(argv[i].c_str(), "imagematch2=%s", value2);
		if ((value[0] != NULL) || (value2[0] != NULL))
		{
			char *valueTmp = (value[0] != NULL) ? value : value2;
			const string treeLabelName = (string("@tree_label") == valueTmp) ? tree_label : valueTmp;

			m_src = src.clone(); // 결과 정보를 출력할 때, 쓰임
			m_matchResult[m_curIdx].Init(m_matchScript, &dst, treeLabelName, 0,
				(sParseTree*)m_matchScript->FindTreeLabel(treeLabelName), 
				true, false);

			m_matchResult[m_curIdx].m_traverseType = (value[0] != NULL) ? 0 : 1;

			// 쓰레드로 동작한다.
			m_state = BEGIN_MATCH;
			cMatchProcessor::Get()->Match(m_matchResult[m_curIdx]);

			break; // 명령어를 실행한 후, 종료한다.
		}


		//----------------------------------------------------------------------
		// tesseract
		ZeroMemory(value, sizeof(value));
		if (argv[i] == "tess")
		{
			// skew 된 영역의 이미지로 문자 인식을 한다.
			string srcStr;
			string result;
			if (skewPoint1 != Point(0, 0))
			{
				m_tessImg = dst( Rect(0, MIN(skewPoint1.y, skewPoint2.y)-5,
					dst.cols, (int)abs(skewPoint1.y-skewPoint2.y)+10) );
				srcStr = m_tess.Recognize(m_tessImg);
				result = m_tess.Dictionary(srcStr);
			}
			else
			{
				srcStr = m_tess.Recognize(dst);
				result = m_tess.Dictionary(srcStr);
			}

			putText(dst, common::format("tess source = %s", srcStr.c_str()).c_str(),
				Point(0, 90), 1, 2.f, Scalar(255, 255, 255), 2);

			putText(dst, common::format("tess result = %s", result.c_str()).c_str(), 
				Point(0, 120), 1, 2.f, Scalar(255, 255, 255), 2);
		}

	}

	const int t2 = timeGetTime();
	if (dst.data)
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
	dst = m_src.clone();

	// 결과 정보 출력
	putText(dst, m_matchResult[m_curIdx].m_resultStr.c_str(), Point(0, 30), 1, 1.3f, Scalar(0, 0, 255), 2);

	// 매칭된 이미지를 dst에 출력한다.
	for (int i = 0; i < 1024; ++i)
	{
		if (m_matchResult[m_curIdx].m_data[i].result > 0)
		{
			// Matching Rect 출력
			cRectContour rect;
			if (m_matchScript->m_nodeTable[i]->matchType == 0) // TemplateMatch
			{
				rect.Init(m_matchResult[m_curIdx].m_data[i].matchRect);
				rect.Draw(dst, Scalar(0, 0, 255), 2);
			}
			else // Feature Match
			{
				vector<cv::Point> pts(m_matchResult[m_curIdx].m_data[i].matchRect2, 
					m_matchResult[m_curIdx].m_data[i].matchRect2 + 4);
				rect.Init(pts);
				rect.Draw(dst, Scalar(0, 0, 255), 2);
			}

			// 매칭된 이미지 경로 출력
			putText(dst, m_matchScript->m_nodeTable[i]->name, rect.m_contours[0] + Point(0, 30), 1, 2.f, Scalar(0, 0, 255), 2);
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
