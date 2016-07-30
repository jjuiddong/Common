//
// 2016-04-17, jjuiddong
// 형식을 가진 출력 버퍼를 생성한다.
//
#pragma once


namespace motion
{

	class cFormatParser
	{
	public:
		cFormatParser();
		virtual ~cFormatParser();

		bool Read(const string &fileName);
		bool Parse(const string &str);
		int Execute(BYTE *buffer, const int bufferSize);


	public:
		// BNF
		// statement -> arg_list
		// arg_list -> { arg }
		// arg -> expr [, expr]
		// expr -> string | id | fmt | "string"

		enum EXPR_TYPE { NONE ,STRING, ID, FMT, };

		struct sExpr
		{
			EXPR_TYPE type;
			string str;			
		};

		struct sArg
		{
			sExpr *expr1;
			sExpr *expr2;
		};

		struct sArgList
		{
			sArg *arg;
			sArgList *next;
		};

		struct sStatement
		{
			sArgList *arglist;
		};


	protected:
		sStatement* statement(string &src);
		sArgList* arglist(string &src);
		sArg* arg(string &src);
		sExpr* expr(string &src);

		void rm_statement(sStatement *p);
		void rm_arglist(sArgList *p);
		void rm_arg(sArg *p);
		void rm_expr(sExpr *p);

		script::FIELD_TYPE::TYPE GetFmtType(const string &fmt);


	public:
		sStatement *m_stmt;
	};

}
