//
// 환경파일 읽고, 저장하는 기능.
//
// 2019-03-05, jjuiddong
//	- check UTF-8 with BOM
//		- https://stackoverflow.com/questions/6769311/how-windows-notepad-interpret-characters
//		- Notepad normally uses ANSI encoding, so if it reads the file as UTF-8 
//		  then it has to guess the encoding based on the data in the file.
//		  If you save a file as UTF-8, Notepad will put the BOM(byte order mark) 
//		  EF BB BF at the beginning of the file.
//
// 2019-03-30
//	- makefile syntax parsing
//		- ifeq, ifneq, else, endif
//		- http://korea.gnu.org/manual/4check/make-3.77/ko/make_7.html
//
//	- syntax
//		- ifeq (val, val)
//		- ifneq (val, val)
//		- else ifeq(val, val)
//		- endif
//
//	- sample
//		mode = 1
//		ifeq($(mode), 1)
//			test = 0
//			ifeq($(mode), 2)
//				ifeq($(mode), 3)
//					test = 10
//				endif
//			endif
//		else ifeq($(mode), 4)
//			test = 1
//		else ifeq($(mode), 5)
//			test = 2
//		else
//			test = 3
//		endif
//
#pragma once

namespace common
{

	class cConfig
	{
	public:
		cConfig(const string &fileName = "");
		virtual ~cConfig();

		bool Read(const string &fileName);
		bool Write(const string &fileName);
		void ParseStr(const string &str);

		bool GetBool(const string &key, const bool defaultValue=false);
		float GetFloat(const string &key, const float defaultValue = 0);
		double GetDouble(const string &key, const double defaultValue = 0);
		int GetInt(const string &key, const int defaultValue = 0);
		string GetString(const string &key, const string &defaultValue = "");
		Vector3 GetVector3(const string &key, const Vector3 &defaultValue = Vector3(0, 0, 0));
		bool CheckKey(const string &key);

		void SetValue(const StrId &key, const char *value);
		void SetValue(const StrId &key, const bool value);
		void SetValue(const StrId &key, const int value);
		void SetValue(const StrId &key, const float value);
		void SetValue(const StrId &key, const double value);


	protected:
		struct sReservedKeyword {
			enum TYPE { NONE, IFEQ, IFNEQ, ELSE, ENDIF, EOF0 };
			TYPE type;
			StrId name;
		};

		bool Parse(const string &fileName);
		bool CheckUTF8withBOM(std::ifstream &ifs);
		bool GetToken(std::istream &stream, OUT Str512 &out
			, const bool isTokenizingSpecialChar = false
			, const bool isScanSpacing = false
			, OUT int *eofState = NULL);
		sReservedKeyword::TYPE GetToken2(std::istream &stream, OUT Str512 &out
			, const bool isTokenizingSpecialChar = false);
		sReservedKeyword::TYPE GetReservedKeyword(const char *str);
		void UnGetToken(std::istream &stream, const Str512 &tok);
		bool IsMakeFileSyntax(const string &fileName);
		const string& GetSymbolData(const string &key, const int cnt = 100);
		virtual void InitDefault() {}
		virtual void UpdateParseData() {}

		// syntax, BNF
		// program -> expr-list
		// expr-list -> { expr }
		// expr -> if_expr | ifelse_expr | assign_expr
		// if_expr -> [ifeq | ifneq] ( var, var ) expr-list endif
		// ifelse_expr -> [ifeq | ifneq] ( var, var ) expr-list 
		//				    else [if_expr | ifelse_expr | expr-list endif]
		// assign_expr -> id = var
		// var -> variable | number | string
		// variable -> $(id)  | id
		struct sExpr;
		struct sIfExpr {
			sReservedKeyword::TYPE type; // ifeq, ifneq
			string arg1;
			string arg2;
			sExpr *exprList;
			sIfExpr *elseIfExpr;
			sExpr *elseExprList;
		};
		struct sAssignExpr {
			string id;
			string var;
		};		
		struct sExpr {
			sIfExpr *ifExpr;
			sAssignExpr *assignExpr;
			sExpr *next;
		};
		struct sProgram {
			sExpr *exprList;
		};
		
		sProgram* program(std::istream &stream);
		sExpr* expr_list(std::istream &stream);
		sExpr* expr(std::istream &stream);
		sAssignExpr* expr_assign(std::istream &stream, const Str512 &tok);
		sIfExpr* expr_if(std::istream &stream, const sReservedKeyword::TYPE type);
		string variable(std::istream &stream);

		void Execute(sProgram *p);
		void ExecuteExpr(sExpr *p);
		void ExecuteIfExpr(sIfExpr *p);
		void Remove(sProgram *p);
		void Remove(sExpr *p);
		void Remove(sIfExpr *p);


	public:
		map<string, string> m_options;
		vector<sReservedKeyword> m_keywords;
		sProgram *m_program;
		Str512 m_tok;
	};

}
