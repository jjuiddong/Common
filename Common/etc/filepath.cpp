
#include "stdafx.h"
#include "FilePath.h"
#include <shlwapi.h> // 이 헤더 파일에 FilePath에 관련된 함수들이 많다. 잘 이용해보자.
#include <io.h>
#include <Shellapi.h>
#pragma comment(lib, "shlwapi")
//#include <sys/types.h>  
#include <sys/stat.h>


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
// findExt: 찾고자 하는 확장자, 2개이상 설정할수있게 하기위해서 리스트 자료형태가 되었다.
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles( const list<string> &findExt, const string &searchPath, OUT list<string> &out)
// isRelativePath=false
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
				CollectFiles( findExt, modifySearchPath + string(fd.cFileName) + "/", out);
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
// return  Relative Path
bool common::CollectFiles2(const list<string> &findExt, const string &searchPath, const string &relativePath, OUT list<string> &out)
{
	string modifySearchPath;
	if (!searchPath.empty() &&
		(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath.empty()? "" : searchPath + "\\";
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
				CollectFiles2(findExt, modifySearchPath + string(fd.cFileName) + "/", relativePath, out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				out.push_back(  DeleteCurrentPath(RelativePathTo(relativePath, modifySearchPath + fileName)) );
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (CompareExtendName(fileName.c_str(), (int)fileName.length(), it->c_str()))
					{
						out.push_back(DeleteCurrentPath(RelativePathTo(relativePath, modifySearchPath + fileName)));
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
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
// flag : 0 = 최근 수정된 날짜를 기준으로 정렬한다.
//-----------------------------------------------------------------------------//
bool common::CollectFilesOrdered(const list<string> &findExt, const string &searchPath, OUT list<string> &out, 
	const int flags) // flags=0
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
// out: 일치하는 확장자를 가진 파일이름을 저장한다.
//-----------------------------------------------------------------------------//
bool common::CollectFiles(const vector<WStr32> &findExt, const wchar_t *searchPath, OUT vector<WStrPath> &out)
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


//------------------------------------------------------------------------
// srcFileName의 확장자와 compareExtendName 이름이 같다면 true를 리턴한다.
// 확장자는 srcFileName 끝에서 '.'이 나올 때까지 이다.
//------------------------------------------------------------------------
bool common::CompareExtendName(const char *srcFileName, const int srcStringMaxLength, const char *compareExtendName)
{
	const int len = (int)strnlen_s(srcFileName, srcStringMaxLength);
	if (len <= 0)
		return FALSE;

	int count = 0;
	char temp[5];
	for (int i=0; i < len && i < 4; ++i)
	{
		const char c = srcFileName[ len-i-1];
		if ('.' == c)
		{
			break;
		}
		else
		{
			temp[ count++] = c;
		}
	}
	temp[ count] = NULL;

	char extendName[5];
	for (int i=0; i < count; ++i)
		extendName[ i] = temp[ count-i-1];
	extendName[ count] = NULL;

	if (!strcmp(extendName, compareExtendName))
	{
		return true;
	}

	return false;
}


bool common::CompareExtendName(const wchar_t *srcFileName, const int srcStringMaxLength, const wchar_t *compareExtendName)
{
	const int len = (int)wcslen(srcFileName);
	if (len <= 0)
		return FALSE;

	int count = 0;
	wchar_t temp[5];
	for (int i = 0; i < len && i < 4; ++i)
	{
		const wchar_t c = srcFileName[len - i - 1];
		if ('.' == c)
		{
			break;
		}
		else
		{
			temp[count++] = c;
		}
	}
	temp[count] = NULL;

	wchar_t extendName[5];
	for (int i = 0; i < count; ++i)
		extendName[i] = temp[count - i - 1];
	extendName[count] = NULL;

	if (!wcscmp(extendName, compareExtendName))
	{
		return true;
	}

	return false;
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


bool common::IsFileExist(const char *fileName)
{
	return _access_s(fileName, 0) == 0;
}


bool common::IsFileExist(const wchar_t *fileName)
{
	return _waccess_s(fileName, 0) == 0;
}


bool common::IsFileExist(const StrPath &fileName)
{
	return _access_s(fileName.c_str(), 0) == 0;
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
common::sFolderNode* common::CreateFolderNode(const list<string> &fileList)
{
	sFolderNode *rootNode = new sFolderNode;

	for each (auto &str in fileList)
	{
		vector<string> strs;
		common::tokenizer(str, "/", ".", strs);

		sFolderNode *node = rootNode;
		for (u_int i = 0; i < strs.size(); ++i)
		{
			if (i == (strs.size() - 1)) // Last String Is FileName, then Ignored
				continue;

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
