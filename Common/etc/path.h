//
// 2017-03-26, jjuiddong
// file path class
//
#pragma once


namespace common
{

	class cFilePath
	{
	public:
		cFilePath();
		cFilePath(const string &filePath);
		virtual ~cFilePath();

		void SetFilePath(const string &filePath);
		string GetFileName() const;
		string GetFileNameExceptExt() const;
		string GetFullFileName() const;
		string GetFileExt() const;
		string GetFilePathExceptFileName() const;
		string RelativeTo(const string &pathTo);
		bool IsRelativePath() const;
		__int64 GetFileSize() const;
		const char* c_str() const;
		void CheckPath();

		explicit operator string ();
		explicit operator const char *();
		cFilePath& operator=(const string &rhs);
		cFilePath& operator=(const cFilePath &rhs);


	public:
		string m_filePath;
	};

}
