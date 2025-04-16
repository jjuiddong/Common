#pragma once

namespace common
{

	string GetFilePathExceptFileName(const string& fileName);
	string GetFileExt(const string& fileName);
	string GetFullFileName(const string& fileName);
	string GetFileName(const string& fileName);
	string GetFileNameExceptExt(const string& fileName);
	string GetFileNameExceptExt2(const string& fileName);
	string RelativePathTo(const string& pathFrom, const string& pathTo);
	wstring RelativePathToW(const wstring& pathFrom, const wstring& pathTo);
	bool IsRelativePath(const string& path);
	string DeleteCurrentPath(const string& fileName);
	int64 FileSize(const string& fileName);
	uint64 FileSize2(const string& fileName);
	string RemoveFileExt(const string& fileName);
	string CheckDirectoryPath(const string& fileName);


	// Find and Collect File
	// char argument (ASCII)
	bool CollectFiles(const list<string>& findExt, const string& searchPath, OUT list<string>& out);
	bool CollectFiles2(const list<string>& findExt, const string& searchPath
		, const string& relativePath, OUT list<string>& out);
	bool CollectFiles3(const list<string>& findExt, const string& searchPath,
		const list<string>& ignoreDirs, OUT list<string>& out);
	bool CollectFiles4(const list<string>& findExt, const string& searchPath
		, const string& relativePath, const list<string>& ignoreDirs
		, OUT list<string>& out);

	bool CollectFilesOrdered(const list<string>& findExt, const string& searchPath
		, OUT list<string>& out, const int flags = 0);

	// wchar argument (UNICODE)
	int CollectFolder(const char* searchPath, OUT vector<string>& out
		, const int maxLoop = 10000);
	int CollectFolder2(const char* searchPath, OUT vector<string>& out
		, const int level = -1);

	bool IsFileExist(const char* fileName);
	bool IsFileExist(const wchar_t* fileName);
	bool IsDirectoryExist(const char* fileName);
	bool IsDirectoryExist(const wchar_t* fileName);

	struct sFolderNode
	{
		map<string, sFolderNode*> children;
		list<std::pair<string, sFolderNode*>> childrenFiles; // for Display Order of Folder and Files
		list<string> files;
		int data; // custom data
	};
	sFolderNode* CreateFolderNode(const list<string>& fileList
		, const bool isContainFile = false);
	sFolderNode* CreateFolderNode(const vector<string>& fileList
		, const bool isContainFile = false);
	void DeleteFolderNode(sFolderNode* node);

	int FileOperationFunc(unsigned int func, const string& to, const string& from);

	string BrowseFolder(const HWND hWnd, const string& titleStr, const string& saved_path);
	string OpenFileDialog(const HWND hWnd, const vector<std::pair<wstring, wstring>>& filters);
	string SaveFileDialog(const HWND hWnd, const vector<std::pair<wstring, wstring>>& filters);

}
