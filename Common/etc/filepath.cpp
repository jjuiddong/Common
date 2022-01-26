
#include "stdafx.h"
#include "FilePath.h"
#include <shlwapi.h> // filesystem library
#include <io.h>
#include <Shellapi.h>
#pragma comment(lib, "shlwapi")
#include <sys/stat.h>
#include <shlobj.h>


namespace 
{
	//------------------------------------------------------------------------
	// srcFileName의 확장자와 compareExtendName 이름이 같다면 true를 리턴한다.
	// compareExtendName : .bmp, .jpg, .fbx
	//------------------------------------------------------------------------
	bool CompareExtendName(const char* srcFileName, const int srcStringMaxLength
		, const char* compareExtendName)
	{
		const int len = (int)strnlen_s(srcFileName, srcStringMaxLength);
		if (len <= 0)
			return FALSE;

		const int TEMPSIZE = 16;
		int count = 0;
		char temp[TEMPSIZE];
		for (int i = 0; i < len && i < (ARRAYSIZE(temp) - 1); ++i)
		{
			const char c = srcFileName[len - i - 1];
			temp[count++] = c;
			if ('.' == c)
				break;
		}
		temp[count] = NULL;

		char extendName[TEMPSIZE];
		for (int i = 0; i < count; ++i)
			extendName[i] = temp[count - i - 1];
		extendName[count] = NULL;

		return !strcmp(extendName, compareExtendName);
	}


	// compare extend filename
	// compareExtendName : .bmp, .jpg, .fbx
	bool CompareExtendName(const wchar_t* srcFileName, const int srcStringMaxLength
		, const wchar_t* compareExtendName)
	{
		const int len = (int)wcslen(srcFileName);
		if (len <= 0)
			return FALSE;

		const int TEMPSIZE = 16;
		int count = 0;
		wchar_t temp[TEMPSIZE];
		for (int i = 0; i < len && i < (ARRAYSIZE(temp) - 1); ++i)
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

}


common::StrPath common::RelativePathTo(const StrPath &pathFrom, const StrPath &pathTo)
{
	StrPath out;
	const BOOL result = PathRelativePathToA(out.m_str,
		pathFrom.c_str(), FILE_ATTRIBUTE_DIRECTORY,
		pathTo.c_str(), FILE_ATTRIBUTE_NORMAL);
	return out;
}

// 상대 경로이면 true를 리턴한다.
bool common::IsRelativePath(const StrPath &path)
{
	return PathIsRelativeA(path.c_str()) ? true : false;
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


bool common::IsFileExist(const StrPath &fileName) {
	return _access_s(fileName.c_str(), 0) == 0;
}

bool common::IsDirectoryExist(const StrPath &fileName) {
	return IsDirectoryExist(fileName.c_str());
}

