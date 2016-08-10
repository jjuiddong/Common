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
			char line[256]; // tree parsing string from script file
			char name[128]; // tree name
			vector<string> *table; // string table pointer, reference to symbol table
			int roi[4]; // x,y,w,h
			int depth; // only use parsing
			bool isRelation; // parent relation coordinate roi
			float threshold; // match max threshold, {threshold_0.1 ~ threshold_0.9 ~~ etc}
			int matchType; // 0:templateMatch, 1:featureMatch, 2:ocr match {templatematch or featurematch or tesseract ocr}
			int scalar[3]; // BGR
			int hsv[6]; // hsv
			float scale; // BGR x Scale
			int lineNum; // for error message
			int result;	// for match traverse, only use excute tree, -1:not visit, 0:fail, 1:success, 2:finale success
			bool isRun; // only use excute tree
			int processCnt; // match count
			double max; // templatematch max value, only use execute tree

			//------------------------------------------------------------------
			// graph script
			bool noUpperTraverse;
			bool noProc;
			bool isAuto;
			bool isSideMenu;
			bool isEnterChild;
			bool isNoMenu;
			//bool isUpDnSubmenu;
			bool isSideSubmenu;
			bool isSceneIdInherit;
			bool isSceneIdChildInherit;
			float delay;
			char tag[64];
			int sceneId;
			int key; // keyboard
			//------------------------------------------------------------------

			cv::Point matchLoc; // match location, only use execution
			sParseTree *next;
			sParseTree *child;

			bool IsEmptyRoi() const {
				return !roi[0] && !roi[1] && !roi[2] && !roi[3];
			}

			bool IsEmptyHsv() const {
				return !hsv[0] && !hsv[1] && !hsv[2] &&
					!hsv[3] && !hsv[4] && !hsv[5];
			}

			bool IsEmptyBgr() const {
				return !scalar[0] && !scalar[1] && !scalar[2];
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
