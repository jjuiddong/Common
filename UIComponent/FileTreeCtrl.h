//
// 2016-07-31, jjuiddong
//
// - folder sorting
//		- folder는 항상 제일위에 있게함.
//
#pragma once


// CFileTreeCtrl

class CFileTreeCtrl : public CTreeCtrl
{
public:
	CFileTreeCtrl();
	virtual ~CFileTreeCtrl();

	void Update(const list<string> &fileList);
	void Update(const string &directoryPath, const list<string> &extList, const int flags=0);
	void Update(const string &directoryPath, const list<string> &extList, const string &serchStr );


	string GetSelectFilePath(HTREEITEM item);
	int GetFileCount() const;
	void ExpandAll();


public:
	struct sTreeNode 
	{
		map<string, sTreeNode*> children;
		list< std::pair<string, sTreeNode*> > childrenFiles; // for Display Order of Folder and Files
		int data; // special data
	};

	sTreeNode* CreateTreeNode(const list<string> &fileList);
	sTreeNode* CreateFolderNode(const list<string> &fileList);
	void DeleteTreeNode(sTreeNode *node);
	void GenerateTreeItem(HTREEITEM parent, sTreeNode*node);

	DECLARE_MESSAGE_MAP()

	static CImageList *m_imageList;
	static int m_imageListRefCnt;
	int m_fileCount;
};


inline int CFileTreeCtrl::GetFileCount() const { return m_fileCount; }
