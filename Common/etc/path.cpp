
#include "stdafx.h"
#include "path.h"

using namespace common;

cFilePath::cFilePath()
{
}

cFilePath::cFilePath(const char *filePath)
{
	SetFilePath(string(filePath));
}

cFilePath::cFilePath(const string &filePath)
{
	SetFilePath(filePath);
}

cFilePath::~cFilePath()
{
}


void cFilePath::SetFilePath(const string &filePath)
{
	m_filePath = filePath;
}


string cFilePath::GetFileName() const
{
	return common::GetFileName(m_filePath);
}


string cFilePath::GetFileNameExceptExt() const
{
	return common::GetFileNameExceptExt(m_filePath);
}


string cFilePath::GetFullFileName() const
{
	return common::GetFullFileName(m_filePath);
}


// contain dot
// .x
// .dae
string cFilePath::GetFileExt() const
{
	return common::GetFileExt(m_filePath);
}


string cFilePath::GetFilePathExceptFileName() const
{
	return common::GetFilePathExceptFileName(m_filePath);
}


string cFilePath::RelativeTo(const string &pathTo)
{
	return common::RelativePathTo(m_filePath, pathTo);
}


bool cFilePath::IsRelativePath() const
{
	return common::IsRelativePath(m_filePath);
}


__int64 cFilePath::GetFileSize() const
{
	return common::FileSize(m_filePath);
}


void cFilePath::CheckPath()
{
	common::CheckDirectoryPath(m_filePath);
}


const char* cFilePath::c_str() const
{
	return m_filePath.c_str();
}


cFilePath::operator string ()
{
	return m_filePath;
}


cFilePath::operator const char *()
{
	return m_filePath.c_str();
}


bool cFilePath::empty() const
{
	return m_filePath.empty();
}


cFilePath& cFilePath::operator=(const char *rhs)
{
	m_filePath = rhs;
	return *this;
}


cFilePath& cFilePath::operator=(const string &rhs)
{
	m_filePath = rhs;
	return *this;
}


cFilePath& cFilePath::operator=(const cFilePath &rhs)
{
	if (this != &rhs)
	{
		m_filePath = rhs.m_filePath;
	}
	return *this;
}
