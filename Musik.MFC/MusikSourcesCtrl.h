///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "CPropTree/PropTree.h"

#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CPropTreeItem*> CMusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CMusikLibrary;

///////////////////////////////////////////////////

class CMusikSourcesCtrl : public CPropTree
{
public:

	// construct / destruct
	CMusikSourcesCtrl( CMusikLibrary* m_Library );
	virtual ~CMusikSourcesCtrl();

protected:

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// create default headers
	void CreateHeaders();
	CPropTreeItem* m_LibrariesRoot;
	CPropTreeItem* m_StdPlaylistRoot;
	CPropTreeItem* m_DynPlaylistRoot;

	// loading the items
	void LoadLibraries();
	void LoadStdPlaylists();
	void LoadDynPlaylists();

	// macros
	DECLARE_DYNAMIC(CMusikSourcesCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// stuff relating to the core
	CMusikLibrary* m_Library;

	// items that will show up in the list
	CMusikSourcesItemPtrArray m_Libraries;
	CMusikSourcesItemPtrArray m_StdPlaylists;
	CMusikSourcesItemPtrArray m_DynPlaylists;
};

///////////////////////////////////////////////////


