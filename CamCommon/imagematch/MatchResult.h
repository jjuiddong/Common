//
// 2016-07-15, jjuiddong
// ImageMatch 결과를 저장한다.
//
#pragma once


namespace cvproc {
	namespace imagematch {
		
		class cMatchScript2;

		class cMatchResult
		{
		public:
			cMatchResult();
			virtual ~cMatchResult();

			void Init(cMatchScript2 *p, const cv::Mat &input, const string &inputName, 
				const int inputImageId, sParseTree *labelTree, const bool isRegisterInput, 
				const bool isBlockMode);
			
			void MatchComplete();

			void TerminateMatch();

			void Clear();


		public:
			// 입력
			int m_traverseType;	 // 매칭 순회하는 방식 설정, {0 = 매칭되면 종료, 1=모드 매칭한후, 가장 접합률이 높은 노드 설정}
			cMatchScript2 *m_script;
			cv::Mat m_input;
			cv::Mat m_srcImage; // 원본 입력 이미지, m_input 은 매칭될 때, 흑백영상으로 바뀐다.
			string m_inputName;
			int m_inputImageId;
			bool m_registerInput;
			bool m_blockMode;
			bool m_removeInput;
			sParseTree *m_nodeLabel;
			bool m_isTerminate; // 매칭을 종료할 때 true 가 되어야 한다.

			// 출력
			string m_resultStr;
			bool m_isRun;	// 매칭을 시도했다면 true 가된다.
			bool m_isEnd;
			int m_result;
			int m_matchCount; // debug
			int m_beginTime;

			struct sNodeResult
			{
				bool nodesRun; // 쓰레드로 동작하기 때문에, 동기화를 위해, 임시 버퍼로 쓰레드 상태를 판단한다. key = node->id
				cv::Rect matchRect; // 매칭된 위치를 저장한다. TemplateMatch
				cv::Point matchRect2[4]; // 매칭된 위치를 저장한다. FeatureMatch
				double max; // 템플릿 매치 최대 값을 저장한다.
				char str[128]; // OCR 인식 문자열
				int result; // 매칭 결과
			};

			sNodeResult m_data[1024];
		};

} }