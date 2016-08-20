//
// 2016-07-13, jjuiddong
// 이미지 매칭을 전담하는 싱글톤 클래스
// 쓰레드로 동작하며, MatchScript Parse 트리를 순회하며,
// 이미지 매칭을 시도한다.
// 
#pragma once

#include "MatchResult.h"


namespace cvproc {
	namespace imagematch {

		class cMatchProcessor : public common::cSingleton<cMatchProcessor>
		{
		public:
			cMatchProcessor();
			virtual ~cMatchProcessor();

			bool Match( INOUT cMatchResult &matchResult );


			struct sExecuteTreeArg 
			{
				bool isUsed;
				bool isEnd;
				sParseTree *parent; // 매칭할 부모 노드
				sParseTree *node; // 매칭할 노드, 템플릿 이미지 경로를 저장하고 있다.
				cv::Mat *out; // 결과 정보를 저장해서 리턴
				int result;
				cMatchResult *matchResult;
			};
			int executeTreeEx(INOUT sExecuteTreeArg &arg);

			void FinishMatchThread();
			void SetInputImage(const cv::Mat &img, const string &name = "@input");
			void RemoveInputImage(const string &name, const int imageId);
			void RemoveScalarImage(const int imageId);
			void RemoveHsvImage(const int imageId);
			cMatchResult* AllocMatchResult();
			void FreeMatchResult(cMatchResult *p);
			void ClearMemPool();
			void Clear();


		public:
			cv::Mat& loadImage(const string &fileName);
			cv::Mat& loadScalarImage(const string &fileName, const int imageId, const cv::Scalar &scalar, const float scale);
			//cv::Mat& loadHsvImage(const string &fileName, const int imageId, const cv::Scalar &hsv1, const cv::Scalar &hsv2);
			cv::Mat& loadCvtImageAcc(const string &fileName, const int imageId, sParseTree *node);
			void loadDescriptor(const string &fileName, OUT vector<cv::KeyPoint> **keyPoints, OUT cv::Mat **descriptor);
			void loadDescriptor(const string &keyName, const cv::Mat &img, OUT vector<cv::KeyPoint> **keyPoints, OUT cv::Mat **descriptor, const bool isSearch = true);
			int executeOcr(INOUT sExecuteTreeArg &arg);
			tess::cTessWrapper* GetTesseract();


		public:
			cMatchResult *m_lastMatchResult;
			map<string, cv::Mat* > m_imgTable;
			map<string, vector<cv::KeyPoint>* > m_keyPointsTable; // use feature detection
			map<string, cv::Mat* > m_descriptorTable; // use feature detection
			map<__int64, cv::Mat* > m_scalarImgTable;	// source image & scalar  table
			//map<__int64, cv::Mat* > m_hsvImgTable;	// hsv convert image table
			cv::Ptr<cv::xfeatures2d::SURF> m_detector;
			bool m_isGray = true;
 			bool m_isLog; // default = true, log message print to log.txt
			bool m_isLog2; // default = true, ocr log

			struct sCvtKey
			{
				__int64 key1;
				__int64 key2;
				__int64 key3;
				bool operator<(const sCvtKey&rhs) const;
				bool operator==(const sCvtKey&rhs) const;
			};
			map<sCvtKey, cv::Mat* > m_cvtImgTable;	// hsv, hls convert image table

			// match result pool
			struct sMRInfo
			{
				bool used;
				cMatchResult *p;
			};
			vector<sMRInfo> m_matchResults;

			// tesseract pool
			vector<tess::cTessWrapper*> m_tess;
			int m_tessIdx;
			CriticalSection m_tessCS;

			CriticalSection m_loadImgCS;
			CriticalSection m_loadDescriptCS;
			CriticalSection m_loadScalarImgCS;
			CriticalSection m_loadHsvImgCS;
			int m_inputImageId; // input image counting
			bool m_isMatchThreadLoop;
			bool m_isThreadTerminate; // false 일 때만, 스레드가 동작한다. 스레드를 중간에 멈추는 용도로 씀.
			HANDLE m_threadHandle;
			static cv::Mat m_nullMat; // empty Mat (only use null Mat return)
		};

} }