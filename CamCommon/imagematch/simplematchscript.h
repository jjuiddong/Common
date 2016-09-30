//
// 2016-07-11, jjuiddong
// 이미지 매칭 스크립트
// 심플 버전
//
// - templatematch = filename
// - featurematch = filename
// - imagematch = filename
//
//	symbol
//		- @label_select : argument file 1
//     - @tree_label : argument file 2
//
#pragma once



namespace cvproc {
	namespace imagematch {

		class cSimpleMatchScript
		{
		public:
			cSimpleMatchScript(cMatchScript2 *matchScript);
			virtual ~cSimpleMatchScript();

			string Match(INOUT cv::Mat &src, OUT cv::Mat &dst, const string &script, 
				const string &label_select="", const string &capture_select="", const string &tree_label="");

			int IsMatchComplete();
			bool UpdateMatchResult(OUT cv::Mat &dst);
			void TerminiateMatch();
			cMatchResult& GetCurrentMatchResult();


		protected:
			void Parse(const string &script);
			void NextIndex();
			int attrs(const string &str, OUT string &out);
			void attr_list(const string &str);


		public:
			enum STATE { WAIT, BEGIN_MATCH};
			STATE m_state;
			cMatchScript2 *m_matchScript;
			vector<std::pair<string, string>> m_commands;
			cMatchResult m_matchResult[32];
			cv::Mat m_src;
			cv::Mat m_tessImg;
			int m_curIdx;
			int m_beginMatchTime;
			tess::cTessWrapper m_tess;
			CriticalSection m_processCS;
		};

} }