// FileTreeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FileTreeCtrl.h"
#include "UIUtility.h"


CImageList *CFileTreeCtrl::m_imageList = NULL;
int CFileTreeCtrl::m_imageListRefCnt = 0;

// CFileTreeCtrl
CFileTreeCtrl::CFileTreeCtrl()
{
	if (!m_imageList)
	{
		SHFILEINFO sfi = { 0 }; 
		m_imageList = new CImageList();
		m_imageList->Attach((HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX));
	}

	++m_imageListRefCnt;

}

CFileTreeCtrl::~CFileTreeCtrl()
{
	--m_imageListRefCnt;
	if (m_imageListRefCnt <= 0)
	{
		SAFE_DELETE(m_imageList);
		m_imageListRefCnt = 0;
	}
}


BEGIN_MESSAGE_MAP(CFileTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()



// CFileTreeCtrl 메시지 처리기입니다.

// fileList에 저장된 파일 경로 스트링으로 트리를 만든다.
void CFileTreeCtrl::Update(const list<string> &fileList)
{
	DeleteAllItems();	

	SetImageList(m_imageList, TVSIL_NORMAL);

	sTreeNode *rootNode = CreateTreeNode(fileList);
	GenerateTreeItem(NULL, rootNode);	
	Expand(GetRootItem(), TVE_EXPAND);

	DeleteTreeNode(rootNode);

	m_fileCount = (int)fileList.size();
}


// flag : 0 = 이름 순서대로 정렬
//			  1 = 최근 수정된 순서대로 정렬
void CFileTreeCtrl::Update(const string &directoryPath, const list<string> &extList, const int flags) // flags=0
{
	list<string> files;
	
	if (flags == 0)
		common::CollectFiles(extList, directoryPath, files);
	else if (flags == 1)
		common::CollectFilesOrdered(extList, directoryPath, files);

	Update(files);
}


// searchStr 문자열이 포함된 것만 화면에 출력한다.
void CFileTreeCtrl::Update(const string &directoryPath, const list<string> &extList, const string &serchStr )
{
	list<string> files;
	common::CollectFiles(extList, directoryPath, files);

	if (serchStr.empty())
	{
		Update(files);
	}
	else
	{
		list<string> fileList;
		for each(auto name in files)
		{
			if (string::npos != common::GetFileName(name).find(serchStr))
				fileList.push_back(name);
		}

		Update(fileList);
	}	
}


// sTreeNode 생성한다.
// 폴더는 파일보다 위에 있게 한다.
CFileTreeCtrl::sTreeNode* CFileTreeCtrl::CreateTreeNode(const list<string> &fileList)
{
	sTreeNode *rootNode = new sTreeNode;

	for each (auto &str in fileList)
	{
		vector<string> strs;
		common::tokenizer( str, "/", ".", strs);

		sTreeNode *node = rootNode;
		for (u_int i=0; i < strs.size(); ++i)
		{
			const string name = strs[i];
			auto it = node->children.find(name);
			if (node->children.end() ==  it)
			{
				sTreeNode *t = new sTreeNode;
				node->children[ name] = t;

				if (i == (strs.size()-1)) // Last String Is FileName
 				{
					node->childrenFiles.push_back(std::pair<string, sTreeNode*>(name, t));
 				}
				else
				{
 					// 폴더는 파일 리스트에서 상위로 이동한다.
					node->childrenFiles.push_front(std::pair<string, sTreeNode*>(name, t));
				}

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


// Only Generate Folder Node
CFileTreeCtrl::sTreeNode* CFileTreeCtrl::CreateFolderNode(const list<string> &fileList)
{
	sTreeNode *rootNode = new sTreeNode;

	for each (auto &str in fileList)
	{
		vector<string> strs;
		common::tokenizer(str, "/", ".", strs);

		sTreeNode *node = rootNode;
		for (u_int i = 0; i < strs.size(); ++i)
		{
			if (i == (strs.size() - 1)) // Last String Is FileName, then Ignored
				continue;

			const string name = strs[i];
			auto it = node->children.find(name);
			if (node->children.end() == it)
			{
				sTreeNode *t = new sTreeNode;
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


// treenode 제거.
void CFileTreeCtrl::DeleteTreeNode(sTreeNode *node)
{
	RET(!node);
	for each (auto &child in node->children)
		DeleteTreeNode(child.second);
	delete node;
}


// sTreeNode 정보로 TreeCtrl 노드를 생성한다.
void CFileTreeCtrl::GenerateTreeItem(HTREEITEM parent, sTreeNode*node)
{
	RET(!node);

	for each (auto &child in node->childrenFiles)
	{
		const int imageIdx = child.second->children.empty()? 0 : 1;
		HTREEITEM item = InsertItem(str2wstr(child.first).c_str(), imageIdx, imageIdx, parent);
		GenerateTreeItem(item, child.second);
	}
}


// 선택된 트리노드의 파일 경로를 리턴한다.
string CFileTreeCtrl::GetSelectFilePath(HTREEITEM item)
{
	wstring path;
	HTREEITEM curItem = item;
	while (curItem)
	{
		if (path.empty())
			path = (wstring)GetItemText(curItem);
		else
			path = (wstring)GetItemText(curItem) + L"/" + path;

		curItem = GetParentItem(curItem);
	}

	return wstr2str(path);
}


// 모든 트리 노드들을 펼친다.
void CFileTreeCtrl::ExpandAll()
{
	::ExpandAll(*this);
}
