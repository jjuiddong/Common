#pragma once

namespace common
{

	string GetFilePathExceptFileName(const string &fileName);
	string GetFileExt(const string &fileName);
	string GetFullFileName(const string &fileName);
	string GetFileName(const string &fileName);
	string GetFileNameExceptExt(const string &fileName);
	string RelativePathTo(const string &pathFrom, const string &pathTo);
	StrPath RelativePathTo(const StrPath &pathFrom, const StrPath &pathTo);
	wstring RelativePathToW(const wstring &pathFrom, const wstring &pathTo);
	bool IsRelativePath(const string &path);
	bool IsRelativePath(const StrPath &path);
	string DeleteCurrentPath(const string &fileName);
	__int64 FileSize(const string &fileName);
	string RemoveFileExt(const string &fileName);
	string CheckDirectoryPath(const string &fileName);


	// Find and Collect File
	bool CollectFiles( const list<string> &findExt, const string &searchPath, OUT list<string> &out);
	bool CollectFiles2(const list<string> &findExt, const string &searchPath
		, const string &relativePath, OUT list<string> &out);
	bool CollectFiles3(const list<string> &findExt, const string &searchPath, 
		const list<string> &ignoreDirs, OUT list<string> &out);

	bool CollectFilesOrdered(const list<string> &findExt, const string &searchPath
		, OUT list<string> &out, const int flags=0);
	bool FindFile( const StrPath &findName, const StrPath &searchPath, StrPath &out, const int depth=-1);
	bool FindFile2(const StrPath &findName, const StrPath &searchPath
		, const list<string> &ignoreDirs, StrPath &out, const int depth = -1);

	bool CollectFiles(const vector<WStr32> &findExt, const wchar_t *searchPath, OUT vector<WStrPath> &out);
	bool CollectFiles3(const vector<WStr32> &findExt, const wchar_t *searchPath
		, const vector<WStr64> &ignoreDirs, OUT vector<WStrPath> &out);

	bool IsFileExist(const char *fileName);
	bool IsFileExist(const wchar_t *fileName);
	bool IsFileExist(const StrPath &fileName);

	struct sFolderNode
	{
		map<string, sFolderNode*> children;
		list< std::pair<string, sFolderNode*> > childrenFiles; // for Display Order of Folder and Files
		int data; // special data
	};
	sFolderNode* CreateFolderNode(const list<string> &fileList);
	void DeleteFolderNode(sFolderNode *node);

	int FileOperationFunc(unsigned int func, const string &to, const string &from);

}
