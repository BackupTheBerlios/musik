///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikListCtrl 
//
// Filename(s): 
//
//   CMusikListCtrl.h,
//   CMusikListCtrl.cpp
//
// Information:
//
//   A CListCtrl instance that does things that Musik needs,
//   such as drag and drop and hiding scrollbars.
//
// Example: 
//
//   CMusikSourcesCtrl, CMusikPlaylistCtrl
//
// Usage: 
//
//   See examples.
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//
//   Casey Langen   : Lead Developer, Project Manager
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
// 
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Hide Scrollbar code taken from: 
//   Lars [Large] Werner
//   http://www.codeproject.com/listctrl/listctrlsbhide.asp
//
// Drag and Drop accept files code taken from:
//   Stuart Carter
//   http://www.codeproject.com/listctrl/filedroplistctrl.asp
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#define LCSB_CLIENTDATA 1
#define LCSB_NCOVERRIDE 2

#define SB_HORZ             0
#define SB_VERT             1
#define SB_BOTH             3

///////////////////////////////////////////////////

typedef HRESULT (CALLBACK FAR * LPFN_DROP_FILES_CALLBACK)(CListCtrl*, const CString&, const UINT&);

///////////////////////////////////////////////////

class CMusikListCtrl : public CListCtrl
{
public:
	CMusikListCtrl();
	virtual ~CMusikListCtrl();

	// enumerations
	enum DLDropFlags
	{
		DL_ACCEPT_FILES =       0x01,	// Allow files to be dropped
		DL_ACCEPT_FOLDERS =		0x02,	// Allow folders to be droppped
		DL_ALLOW_DUPLICATES =	0x04,	// Allow a pathname to be dropped even if its already in the list (ignored if you specify a callback function)
		DL_FILTER_EXTENSION =	0x10,	// Only accept files with the specified extension. Specify in csFileExt
		DL_USE_CALLBACK =		0x20,	// Receive a callback for each item dropped, specified in pfnCallback (you have responsibility for inserting items into the list)

		DL_FOLDER_TYPE =		0x40,	// Returned to the callback function - indicating the type of path dropped
		DL_FILE_TYPE =			0x80
	};

	// structs
	struct DROPLISTMODE 
	{ 
		UINT iMask;								// Specifies what type of items to accept - a combination of the above flags
		CString csFileExt;						// The file extension on which to filter. Use the format ".extension". Ignored unless DL_FILTER_EXTENSION is specified
		LPFN_DROP_FILES_CALLBACK pfnCallback;	// Address of your callback function. Ignored unless DL_USE_CALLBACK is specified
	}; 

	// drag and drop mode
	BOOL SetDropMode(const CMusikListCtrl::DROPLISTMODE& dropMode);
	DROPLISTMODE GetDropMode() const { return m_dropMode; };

	// hide scrollbars
	void HideScrollBars(int Type, int Which=SB_BOTH);

protected:

	// message maps
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnDropFiles(HDROP dropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// drag and drop
	virtual int InsertPathname(const CString& csFilename);

	// macros
	DECLARE_MESSAGE_MAP()

private:

	// drag and drop
	DROPLISTMODE m_dropMode;
	BOOL m_bMustUninitOLE;

	CString ExpandShortcut(CString& csFilename) const;
	BOOL ValidatePathname(const CString& csPathname, UINT& iPathType) const;

	// Prevent default compiler generation of these copy constructors (no bitwise copy)
	CMusikListCtrl& operator=(const CMusikListCtrl& x);
	CMusikListCtrl(const CMusikListCtrl& x);

	// scrollbars
	BOOL NCOverride;
	int Who;
};

///////////////////////////////////////////////////