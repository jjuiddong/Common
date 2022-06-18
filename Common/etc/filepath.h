#pragma once

namespace common
{

	StrPath RelativePathTo(const StrPath &pathFrom, const StrPath &pathTo);
	bool IsRelativePath(const StrPath &path);

	// Find and Collect File
	// char argument (ASCII)
	bool FindFile( const StrPath &findName, const StrPath &searchPath, StrPath &out, const int depth=-1);
	bool FindFile2(const StrPath &findName, const StrPath &searchPath
		, const list<string> &ignoreDirs, StrPath &out, const int depth = -1);
	bool FindFile3(const StrPath& findName, const StrPath& searchPath, StrPath& out, const int depth = -1);

	// wchar argument (UNICODE)
	bool CollectFiles(const vector<WStr32> &findExt, const wchar_t *searchPath, OUT vector<WStrPath> &out);
	bool CollectFiles(const list<WStr32> &findExt, const wchar_t *searchPath, OUT list<WStrPath> &out);
	bool CollectFiles3(const vector<WStr32> &findExt, const wchar_t *searchPath
		, const vector<WStr64> &ignoreDirs, OUT vector<WStrPath> &out);
	bool CollectFiles3(const list<WStr32> &findExt, const wchar_t *searchPath
		, const list<WStr64> &ignoreDirs, OUT list<WStrPath> &out);

	bool IsFileExist(const StrPath &fileName);
	bool IsDirectoryExist(const StrPath &fileName);

}
