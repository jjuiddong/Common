//
// 2016-05-27, jjuiddong
// ImageMatch 스크립트 전역 선언 모음.
//
#pragma once


namespace cvproc {
	namespace imagematch {


		struct sParseTree
		{
			int type; // 0=tree, 1=exec, 2=include
			int id; // tree타입일 때만 사용, 0부터 시작하는 중복되지 않는 id, matchScript2에서 생성
			map<string, string> attrs;
 			int depth; // only use parsing
			int lineNum; // for error message
			sParseTree *next;
			sParseTree *child;

			// match processing
			int result;	// for match traverse, only use excute tree, -1:not visit, 0:fail, 1:success, 2:final success
			bool isRun; // only use excute tree
			int processCnt; // match count
			double max; // templatematch max value, only use execute tree
			cv::Point matchLoc; // match location, only use execution

			sParseTree() {
				type = 0;
				id = 0;
				depth = 0;
				result = 0;
				isRun = false;
				processCnt = 0;
				max = 0;
				next = NULL;
				child = NULL;
			}

			bool IsEmptyRoi() const {
				return attrs.end() == attrs.find("roi");
			}

			bool IsEmptyHsv(const int idx) const {
				switch (idx)
				{
				case 0:
					return attrs.end() == attrs.find("hsv0");
				case 1:
					return attrs.end() == attrs.find("hsv1");
				case 2:
					return attrs.end() == attrs.find("hsv2");
				default:
					break;
				}
				return false;
			}

			bool IsEmptyHls(const int idx) const {
				switch (idx)
				{
				case 0:
					return attrs.end() == attrs.find("hls0");
				case 1:
					return attrs.end() == attrs.find("hls1");
				case 2:
					return attrs.end() == attrs.find("hls2");
				default:
					break;
				}
				return false;
			}

			bool IsEmptyCvt() const {
				for (int i = 0; i < 3; ++i)
					if (!IsEmptyHsv(i))
						return false;

				for (int i = 0; i < 3; ++i)
					if (!IsEmptyHls(i))
						return false;

				return true;
			}

			bool IsEmptyBgr() const {
				return attrs.end() == attrs.find("scalar");
			}

			sParseTree* clone(const bool recursive=true) const
			{
				sParseTree*node = new sParseTree;
				node->type = this->type;
				node->id = this->id;
				node->attrs = this->attrs;

				if (recursive)
				{
					node->child = (this->child)? this->child->clone(recursive) : NULL;
					node->next = (this->next)? this->next->clone(recursive) : NULL;
				}
				else
				{
					node->child = this->child;
					node->next = this->next;
				}
				return node;
			}

		};



		struct sMatchInfo
		{
			string label;
			double max;
			int depth;
			float threshold;
			int elapseTime;
		};


		static const char *g_numStr = "1234567890.";
		static const int g_numLen = 11;
		static const char *g_strStr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@$_/.";
		static const int g_strLen = 57;
		static const char *g_strStr2 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@$_/.1234567890";
		static const int g_strLen2 = 67;
		static const char *g_strOp = "+-*/=:()";
		static const int g_opLen = 7;
	


		// 동적으로 생성된 스크립트 스트링 테이블을 제거한다.
		void ReleaseImageMatch();

	} }
