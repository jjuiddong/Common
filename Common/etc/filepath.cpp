
#include "stdafx.h"
#include "FilePath.h"
#include <shlwapi.h> // 이 헤더 파일에 FilePath에 관련된 함수들이 많다. 잘 이용해보자.
#include <io.h>
#include <Shellapi.h>
#pragma comment(lib, "shlwapi")
#include <sys/stat.h>
#include <shlobj.h>


namespace common {

	bool CompareExtendName(const char *srcFileName, int srcStringMaxLength, const char *compareExtendName);
	bool CompareExtendName(const wchar_t *srcFileName, int srcStringMaxLength, const wchar_t *compareExtendName);

}


//------------------------------------------------------------------------
// fileName에서 파일이름과 확장자를 제외한 나머지 경로를 리턴한다.
// 마지막에 '\' 문자는 없다.
//------------------------------------------------------------------------
string common::GetFilePathExceptFileName(const string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	PathRemoveFileSpecA( srcFileName );
	return srcFileName;
}


/**
 @brief  fileName에서 확장자를 리턴한다.
 */
string common::GetFileExt(const string &fileName)
{
	char *ext = PathFindExtensionA(fileName.c_str());
	return ext;
}

string common::RemoveFileExt(const string &fileName)
{
	char tmp[MAX_PATH] = { NULL, };
	strcpy_s(tmp, fileName.c_str());
	PathRemoveExtensionA(tmp);
	return tmp;
}


// 디렉토리 경로를 포함한 총 파일 이름을 리턴한다. (상대경로일 때 이용).
string common::GetFullFileName(const string &fileName)
{
	char dstFileName[ MAX_PATH] = {NULL, };

	if (IsRelativePath(fileName))
	{
		char curDir[ MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, curDir);
		const string path = string(curDir) + "/" + fileName;
		GetFullPathNameA(path.c_str(), MAX_PATH, dstFileName, NULL);
	}
	else
	{
		GetFullPathNameA(fileName.c_str(), MAX_PATH, dstFileName, NULL);
		return dstFileName;
	}

	return dstFileName;
}

// dir1/dir2// ==> dir1/dir2/
string common::CheckDirectoryPath(const string &fileName)
{
	if (fileName.empty())
	{
		return fileName;
	}
	else if ((int)fileName.size() >= 2)
	{
		const char c1 = fileName[fileName.size() - 1];
		const char c2 = fileName[fileName.size() - 2];
		if (((c1 == '/') || (c1 == '\\')) &&
			((c2 == '/') || (c2 == '\\')))
		{
			string temp = fileName;
			temp.pop_back();
			return CheckDirectoryPath(temp);
		}
	}

	return fileName;
}


/**
 @brief fileName의 디렉토리 경로를 제외한 파일이름과 확장자를 리턴한다.
 */
string common::GetFileName(const string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	char *name = PathFindFileNameA(srcFileName);
	return name;
}


/**
 @brief fileName의 디렉토리 경로, 확장자를 제외한 파일이름을 리턴한다.
 */
string common::GetFileNameExceptExt(const string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	char *name = PathFindFileNameA(srcFileName);
	PathRemoveExtensionA(name);
	return name;
}


// pathFrom경로에서 pathTo 경로의 파일을 접근하기 위한 상대경로를 리턴한다.
// ex) 
// pathFrom : c:/project/media,  pathTo : c:/project/media/terrain/file.txt
// result = ./terrain/file.txt
string common::RelativePathTo(const string &pathFrom, const string &pathTo)
{
	char szOut[ MAX_PATH];

	const BOOL result = PathRelativePathToA(szOut,
		pathFrom.c_str(), FILE_ATTRIBUTE_DIRECTORY,
		pathTo.c_str(), FILE_ATTRIBUTE_NORMAL);

	return szOut;
}

common::StrPath common::RelativePathTo(const StrPath &pathFrom, const StrPath &pathTo)
{
	StrPath out;
	const BOOL result = PathRelativePathToA(out.m_str,
		pathFrom.c_str(), FILE_ATTRIBUTE_DIRECTORY,
		pathTo.c_str(), FILE_ATTRIBUTE_NORMAL);
	return out;
}


wstring common::RelativePathToW(const wstring &pathFrom, const wstring &pathTo)
{
	wchar_t szOut[MAX_PATH];

	const BOOL result = PathRelativePathToW(szOut,
		pathFrom.c_str(), FILE_ATTRIBUTE_DIRECTORY,
		pathTo.c_str(), FILE_ATTRIBUTE_NORMAL);

	return szOut;
}


// 상대 경로이면 true를 리턴한다.
bool common::IsRelativePath(const string &path)
{
	return PathIsRelativeA(path.c_str())? true : false;
}
bool common::IsRelativePath(const StrPath &path)
{
	return PathIsRelativeA(path.c_str()) ? true : false;
}



// ./dir1/dir2/file.ext  ==>  dir1/dir2/file.ext
string common::DeleteCurrentPath(const string &fileName)
{
	const int pos = fileName.find(".\\");
	if (pos == 0)
	{
		return DeleteCurrentPath(fileName.substr(2));
	}

	return fileName;
}


__int64  common::FileSize(const string &fileName)
{
	struct __stat64 buf;
	if (_stat64(fileName.c_str(), &buf) != 0)
		return -1; // error, could use errno to find out more
	return buf.st_size;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
//		- ex) ".media/data/"
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			- ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles( const list<string> &findExt, const string &searchPath
	, OUT list<string> &out)
{
	string modifySearchPath;
	if (!searchPath.empty() &&
		(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath.empty() ? "" : searchPath + "\\";
	}

	WIN32_FIND_DATAA fd;
	const string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				CollectFiles( findExt, modifySearchPath + string(fd.cFileName) + "\\", out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fileName);
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (CompareExtendName(fileName.c_str() , (int)fileName.length(), it->c_str()))
					{
						out.push_back(modifySearchPath + fileName );
						break;
					}
					++it;
				}
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);
	
	return true;
}


// same CollectFiles() function
// findExt: .bmp, .jpg, .png
// return  Relative Path
bool common::CollectFiles2(const list<string> &findExt, const string &searchPath
	, const string &relativePath, OUT list<string> &out)
{
	string modifySearchPath;
	if (!searchPath.empty() &&
		(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath.empty()? "" : searchPath + "/";
	}

	WIN32_FIND_DATAA fd;
	const string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				CollectFiles2(findExt, modifySearchPath + string(fd.cFileName) + "/"
					, relativePath, out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				out.push_back(  DeleteCurrentPath(RelativePathTo(relativePath
					, modifySearchPath + fileName)) );
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (CompareExtendName(fileName.c_str(), (int)fileName.length(), it->c_str()))
					{
						out.push_back(DeleteCurrentPath(RelativePathTo(relativePath
							, modifySearchPath + fileName)));
						break;
					}
					++it;
				}
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//	ignoreDirs 에 포함된 폴더는 검색에서 제외된다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles3(const list<string> &findExt, const string &searchPath
	, const list<string> &ignoreDirs, OUT list<string> &out)
{
	string modifySearchPath;
	if (!searchPath.empty() &&
		(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath.empty() ? "" : searchPath + "/";
	}

	WIN32_FIND_DATAA fd;
	const string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin()
					, ignoreDirs.end(), fd.cFileName))
				{
					CollectFiles3(findExt
						, modifySearchPath + string(fd.cFileName) + "/"
						, ignoreDirs, out);
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fileName);
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (CompareExtendName(fileName.c_str(), (int)fileName.length(), it->c_str()))
					{
						out.push_back(modifySearchPath + fileName);
						break;
					}
					++it;
				}
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


bool common::CollectFiles4(const list<string> &findExt, const string &searchPath
	, const string &relativePath, const list<string> &ignoreDirs
	, OUT list<string> &out)
{
	string modifySearchPath;
	if (!searchPath.empty() &&
		(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath.empty() ? "" : searchPath + "\\";
	}

	WIN32_FIND_DATAA fd;
	const string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin()
					, ignoreDirs.end(), fd.cFileName))
				{
					CollectFiles4(findExt, modifySearchPath + string(fd.cFileName) + "/"
						, relativePath, ignoreDirs, out);
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				out.push_back(DeleteCurrentPath(RelativePathTo(relativePath
					, modifySearchPath + fileName)));
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (CompareExtendName(fileName.c_str(), (int)fileName.length(), it->c_str()))
					{
						out.push_back(
							DeleteCurrentPath(
								RelativePathTo(relativePath, modifySearchPath + fileName)));
						break;
					}
					++it;
				}
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


// 파일명과 날짜 정보를 저장해 리턴한다.
// findExt: .bmp, .jpg, .png
bool CollectFilesRaw(const list<string> &findExt, const string &searchPath, 
	OUT list<std::pair<FILETIME, string>> &out)
{
	WIN32_FIND_DATAA fd;
	const string searchDir = searchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				CollectFilesRaw(findExt, searchPath + string(fd.cFileName) + "/", out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				//out.push_back(searchPath + fileName);
				out.push_back(std::pair<FILETIME, string>(fd.ftLastWriteTime, searchPath + fileName));
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (common::CompareExtendName(fileName.c_str(), (int)fileName.length(), it->c_str()))
					{
						out.push_back(std::pair<FILETIME, string>(fd.ftLastWriteTime, searchPath + fileName));
						break;
					}
					++it;
				}
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);
	return true;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
// flag : 0 = 최근 수정된 날짜를 기준으로 정렬한다.
//-----------------------------------------------------------------------------//
bool common::CollectFilesOrdered(const list<string> &findExt, const string &searchPath
	, OUT list<string> &out, const int flags) // flags=0
{
	using std::pair;

	list< pair<FILETIME, string>> files;
	CollectFilesRaw(findExt, searchPath, files);

	// 최근 수정된 날짜 순서대로 정렬.
	// 파일과 폴더를 비교할 때는, 폴더가 항상 먼저 나오게 한다.
	if (flags == 0)
	{
		files.sort(  
			[](const pair<FILETIME, string> &a, const pair<FILETIME, string> &b)
			{ 
				return CompareFileTime(&a.first, &b.first) > 0; 
			} 
		);
	}

	for each (auto &it in files)
		out.push_back(it.second);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles(const vector<WStr32> &findExt, const wchar_t *searchPath
	, OUT vector<WStrPath> &out)
{
	WStrPath modifySearchPath;
	//if (!searchPath.empty() &&
	//	(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..",fd.cFileName))
			{
				const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
				CollectFiles(findExt, newPath.c_str(), out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles(const list<WStr32> &findExt, const wchar_t *searchPath
	, OUT list<WStrPath> &out)
{
	WStrPath modifySearchPath;
	//if (!searchPath.empty() &&
	//	(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..", fd.cFileName))
			{
				const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
				CollectFiles(findExt, newPath.c_str(), out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//	ignoreDirs 에 포함된 폴더는 검색에서 제외된다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles3(const vector<WStr32> &findExt, const wchar_t *searchPath
	, const vector<WStr64> &ignoreDirs, OUT vector<WStrPath> &out)
{
	WStrPath modifySearchPath;
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..", fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin(), ignoreDirs.end(), fd.cFileName))
				{
					const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
					CollectFiles(findExt, newPath.c_str(), out);
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath폴더에 findExt 확장자 리스트에 포함된 파일을 out에 저장한다.
//	ignoreDirs 에 포함된 폴더는 검색에서 제외된다.
//
// searchPath: 탐색하고자 하는 디렉토리 경로
//		- 마지막에 / 넣어야한다.
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
//			ex) .bmp, .jpg, .png
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles3(const list<WStr32> &findExt, const wchar_t *searchPath
	, const list<WStr64> &ignoreDirs, OUT list<WStrPath> &out)
{
	WStrPath modifySearchPath;
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..", fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin(), ignoreDirs.end(), fd.cFileName))
				{
					const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
					CollectFiles(findExt, newPath.c_str(), out);
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//------------------------------------------------------------------------
// srcFileName의 확장자와 compareExtendName 이름이 같다면 true를 리턴한다.
// compareExtendName : .bmp, .jpg, .fbx
//------------------------------------------------------------------------
bool common::CompareExtendName(const char *srcFileName, const int srcStringMaxLength
	, const char *compareExtendName)
{
	const int len = (int)strnlen_s(srcFileName, srcStringMaxLength);
	if (len <= 0)
		return FALSE;

	const int TEMPSIZE = 16;
	int count = 0;
	char temp[TEMPSIZE];
	for (int i=0; i < len && i < (ARRAYSIZE(temp)-1); ++i)
	{
		const char c = srcFileName[ len-i-1];
		temp[count++] = c;
		if ('.' == c)
			break;
	}
	temp[ count] = NULL;

	char extendName[TEMPSIZE];
	for (int i=0; i < count; ++i)
		extendName[ i] = temp[ count-i-1];
	extendName[ count] = NULL;

	return !strcmp(extendName, compareExtendName);
}


// compare extend filename
// compareExtendName : .bmp, .jpg, .fbx
bool common::CompareExtendName(const wchar_t *srcFileName, const int srcStringMaxLength
	, const wchar_t *compareExtendName)
{
	const int len = (int)wcslen(srcFileName);
	if (len <= 0)
		return FALSE;

	const int TEMPSIZE = 16;
	int count = 0;
	wchar_t temp[TEMPSIZE];
	for (int i = 0; i < len && i < (ARRAYSIZE(temp)-1); ++i)
	{
		const wchar_t c = srcFileName[len - i - 1];
		temp[count++] = c;
		if ('.' == c)
			break;
	}

	temp[count] = NULL;

	wchar_t extendName[TEMPSIZE];
	for (int i = 0; i < count; ++i)
		extendName[i] = temp[count - i - 1];
	extendName[count] = NULL;

	return !wcscmp(extendName, compareExtendName);
}


// searchPath 디렉토리 안에서 findName 의 파일이름을 가진 파일이 있다면 해당 경로를
// out 에 저장하고 true 를 리턴한다.
// depth 크기만큼 하위 디렉토리를 검색한다.  -1이면 끝까지 검색한다.
bool common::FindFile( const StrPath &findName, const StrPath &searchPath, StrPath &out
	, const int depth //= -1
)
{
	if (depth == 0)
		return false;

	StrPath lowerCaseFindFileName = findName;
	lowerCaseFindFileName.lowerCase();

	WIN32_FIND_DATAA fd;
	const StrPath searchDir = searchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if (strcmp(".", fd.cFileName) && strcmp("..", fd.cFileName))
			{
				if (FindFile( findName, searchPath + fd.cFileName + "/", out, depth-1 ))
					break;
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			StrPath fileName = fd.cFileName;
			// 속도가 느려져서 주석처리함, GetFileName()호출은 외부에서 할 것
			//if (lowerCase(fileName) == lowerCase(GetFileName(findName)))
			if (fileName.lowerCase() == lowerCaseFindFileName)
			{
				out = searchPath + findName.GetFileName();
				break;
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return !out.empty();
}


// searchPath 디렉토리 안에서 findName 의 파일이름을 가진 파일이 있다면 해당 경로를
// out 에 저장하고 true 를 리턴한다.
// depth 크기만큼 하위 디렉토리를 검색한다.  -1이면 끝까지 검색한다.
// ignoreDirs 에 포함된 폴더명은 검색에서 제외된다.
bool common::FindFile2(const StrPath &findName, const StrPath &searchPath
	, const list<string> &ignoreDirs, StrPath &out
	, const int depth //= -1
)
{
	if (depth == 0)
		return false;

	StrPath lowerCaseFindFileName = findName;
	lowerCaseFindFileName.lowerCase();

	WIN32_FIND_DATAA fd;
	const StrPath searchDir = searchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(".", fd.cFileName) && strcmp("..", fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin(), ignoreDirs.end(), fd.cFileName))
				{
					if (FindFile(findName, searchPath + fd.cFileName + "/", out, depth - 1))
						break;
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			StrPath fileName = fd.cFileName;
			// 속도가 느려져서 주석처리함, GetFileName()호출은 외부에서 할 것
			//if (lowerCase(fileName) == lowerCase(GetFileName(findName)))
			if (fileName.lowerCase() == lowerCaseFindFileName)
			{
				out = searchPath + findName.GetFileName();
				break;
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return !out.empty();
}


// collect sub directory name
// searchPath: search directory name
// maxLoop : avoid too much process time
// return : process loop
int common::CollectFolder(const char *searchPath, OUT vector<string> &out
	, const int maxLoop //= 10000
)
{
	int loop = maxLoop; // count down, when search sub folder

	string modifySearchPath;
	const int searchLen = strlen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += "\\";
	}

	WIN32_FIND_DATAA fd;
	string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (loop > 0)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				const string newPath = modifySearchPath + fd.cFileName;
				out.push_back(newPath);

				loop = CollectFolder(newPath.c_str(), out, loop-1);
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return loop;
}


// collect sub directory name
// searchPath: search directory name
// level: search sub directory count, -1: infinity
// return: 1
int common::CollectFolder2(const char *searchPath, OUT vector<string> &out
	, const int level //= -1
)
{
	if (level == 0)
		return 1; // finish.
	string modifySearchPath;
	const int searchLen = strlen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += "\\";
	}

	WIN32_FIND_DATAA fd;
	string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				const string newPath = modifySearchPath + fd.cFileName;
				out.push_back(newPath);

				CollectFolder(newPath.c_str(), out, (level < 0)? -1 : level-1);
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return 1;
}


bool common::IsFileExist(const char *fileName) {
	return _access_s(fileName, 0) == 0;
}
bool common::IsFileExist(const wchar_t *fileName) {
	return _waccess_s(fileName, 0) == 0;
}
bool common::IsFileExist(const StrPath &fileName) {
	return _access_s(fileName.c_str(), 0) == 0;
}

bool common::IsDirectoryExist(const char *fileName) {
	DWORD attribs = ::GetFileAttributesA(fileName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool common::IsDirectoryExist(const wchar_t *fileName) {
	DWORD attribs = ::GetFileAttributesW(fileName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool common::IsDirectoryExist(const StrPath &fileName) {
	return IsDirectoryExist(fileName.c_str());
}


// Create Folder Tree,  from fileList
// fileList : fileName list
// Must Delete return value by DeleteFolderNode()
//
// root - child1
//					- child1-1
//			- child2
//					- child2-1
//
template <class Seq>
common::sFolderNode* CreateFolderNode_(const Seq &fileList
	, const bool isContainFile = false
)
{
	using namespace common;
	sFolderNode *rootNode = new sFolderNode;

	for each (auto &str in fileList)
	{
		// check directory or file
		const bool isDir = IsDirectoryExist(str.c_str());

		vector<string> strs;
		common::tokenizer(str, "/", ".", strs);

		sFolderNode *node = rootNode;
		for (uint i = 0; i < strs.size(); ++i)
		{
			if (i == (strs.size() - 1))
			{
				if (!isDir && isContainFile) // contain file?
					node->files.push_back(strs[i]);
				continue;// end
			}

			const string name = strs[i];
			auto it = node->children.find(name);
			if (node->children.end() == it)
			{
				sFolderNode *t = new sFolderNode;
				node->children[name] = t;
				node = t;
			}
			else
			{
				node = it->second;
			}
		}
	}

	return rootNode;
}
common::sFolderNode* common::CreateFolderNode(const list<string> &fileList
	, const bool isContainFile //= false
) { 
	return ::CreateFolderNode_(fileList, isContainFile); 
}
common::sFolderNode* common::CreateFolderNode(const vector<string> &fileList
	, const bool isContainFile //= false
) { 
	return ::CreateFolderNode_(fileList, isContainFile);
}


void common::DeleteFolderNode(sFolderNode *node)
{
	RET(!node);
	for each (auto &child in node->children)
		DeleteFolderNode(child.second);
	delete node;
}


// https://msdn.microsoft.com/ko-kr/library/windows/desktop/bb759795(v=vs.85).aspx
// File Copy, Delete, Rename, Move Operation
// func : FO_COPY, FO_DELETE, FO_MOVE, FO_RENAME
int common::FileOperationFunc(unsigned int func, const string &to, const string &from)
{
	char dst[MAX_PATH];
	strcpy_s(dst, to.c_str());
	dst[to.size() + 1] = NULL; // double NULL

	char src[MAX_PATH];
	strcpy_s(src, from.c_str());
	src[from.size() + 1] = NULL; // double NULL

	SHFILEOPSTRUCTA s;
	ZeroMemory(&s, sizeof(s));
	s.hwnd = NULL;
	s.wFunc = func;
	s.fFlags = FOF_NO_UI;
	s.pTo = dst;
	s.pFrom = src;
	s.lpszProgressTitle = NULL;
	return SHFileOperationA(&s);
}


static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		std::string tmp = (const char *)lpData;
		//std::cout << "path: " << tmp << std::endl;
		SendMessage(hwnd, BFFM_SETSELECTIONA, TRUE, lpData);
	}
	return 0;
}


// open select folder dialog
string common::BrowseFolder(const HWND hWnd, const string &titleStr, const string &saved_path)
{
	char path[MAX_PATH];
	const char *path_param = saved_path.c_str();

	BROWSEINFOA bi = { 0 };
	bi.hwndOwner = hWnd;
	bi.lpszTitle = titleStr.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)path_param;

	LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDListA(pidl, path);

		//free memory used
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		return path;
	}

	return "";
}


// open file selection dialog
// filters : filter spec array
//			- first : filter name 
//				- ex) L"Path File (*.txt; *.pmap)"
//			- second : filter spec
//				- ex) L"*.txt;*.pmap"
//
//
// ex) StrPath path = common::OpenFileDialog(m_owner->getSystemHandle()
//					, { {L"Path File (*.txt; *.pmap)", L"*.txt;*.pmap"}
//					 , {L"All File (*.*)", L"*.*"} });
//
//
string common::OpenFileDialog(const HWND hWnd
	, const vector<std::pair<wstring, wstring>> &filters)
{
	IFileOpenDialog *pFileOpen;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		vector<COMDLG_FILTERSPEC> filterSpecs;
		for (auto &filter : filters)
		{
			COMDLG_FILTERSPEC spec;
			spec.pszName = filter.first.c_str();
			spec.pszSpec = filter.second.c_str();
			filterSpecs.push_back(spec);
		}

		pFileOpen->SetFileTypes(filterSpecs.size(), &filterSpecs[0]);

		hr = pFileOpen->Show(hWnd);
		if (SUCCEEDED(hr))
		{
			IShellItem *pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					WStrPath path = pszFilePath;
					CoTaskMemFree(pszFilePath);
					return path.str().c_str();
				}
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}
	return "";
}


// open file save dialog
// filters : filter spec array
//			- first : filter name 
//				- ex) L"Path File (*.txt; *.pmap)"
//			- second : filter spec
//				- ex) L"*.txt;*.pmap"
//
//
// ex) StrPath path = common::SaveFileDialog(m_owner->getSystemHandle()
//					, { {L"Path File (*.txt; *.pmap)", L"*.txt;*.pmap"}
//					 , {L"All File (*.*)", L"*.*"} });
//
//
string common::SaveFileDialog(const HWND hWnd
	, const vector<std::pair<wstring, wstring>> &filters)
{
	IFileSaveDialog *pFileSave;
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
		IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

	if (SUCCEEDED(hr))
	{
		vector<COMDLG_FILTERSPEC> filterSpecs;
		for (auto &filter : filters)
		{
			COMDLG_FILTERSPEC spec;
			spec.pszName = filter.first.c_str();
			spec.pszSpec = filter.second.c_str();
			filterSpecs.push_back(spec);
		}
		pFileSave->SetFileTypes(filterSpecs.size(), &filterSpecs[0]);

		hr = pFileSave->Show(hWnd);
		if (SUCCEEDED(hr))
		{
			IShellItem *pItem;
			hr = pFileSave->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					WStrPath path = pszFilePath;
					CoTaskMemFree(pszFilePath);
					return path.str().c_str();
				}
				pItem->Release();
			}
		}
		pFileSave->Release();
	}

	return "";
}
