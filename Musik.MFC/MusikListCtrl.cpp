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

#include "stdafx.h"
#include "MusikListCtrl.h"

#include <sys/stat.h>
#include <afxdisp.h>	// OLE stuff
#include <shlwapi.h>	// Shell functions (PathFindExtension() in this case)
#include <afxpriv.h>	// ANSI to/from Unicode conversion macros

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

CMusikListCtrl::CMusikListCtrl()
{
	NCOverride = FALSE;	//False as default...
	Who = SB_BOTH;		//Default remove both...

	//
	// Default drop mode
	//
	m_dropMode.iMask = DL_ACCEPT_FILES | DL_ACCEPT_FOLDERS;
	m_dropMode.pfnCallback = NULL;

	//
	// Initialize OLE libraries
	//
	m_bMustUninitOLE = FALSE;
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	if (!pState->m_bNeedTerm) // TRUE if OleUninitialize needs to be called
	{
		HRESULT hr = ::OleInitialize(NULL);
		if (FAILED(hr))
			// Or something of your choosing...
			AfxMessageBox(_T("OLE initialization failed. This means drag and drop will not work.\n\nMake sure that the OLE libraries are the correct version."));
		else
			m_bMustUninitOLE = TRUE;
	}
}

CMusikListCtrl::~CMusikListCtrl()
{
	if(m_bMustUninitOLE)
	{
		::OleUninitialize();
	}
}

BEGIN_MESSAGE_MAP(CMusikListCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CMusikListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//
	// Register ourselves as a drop target for files
	//
	DragAcceptFiles(TRUE);
	
	return 0;
}

BOOL CMusikListCtrl::SetDropMode(const CMusikListCtrl::DROPLISTMODE& dropMode)
{
	//
	// If they want to use a callback, ensure they also
	// specified the address of a function...
	//
	if(dropMode.iMask & DL_USE_CALLBACK && dropMode.pfnCallback == NULL)
	{
		// Must specify a function if using DL_USE_CALLBACK flag
		ASSERT(FALSE);
		return FALSE;
	}

	m_dropMode = dropMode;
	return TRUE;
}

void CMusikListCtrl::OnDropFiles(HDROP dropInfo)
{
	//
	// Get the number of pathnames (files or folders) dropped
	//
	UINT nNumFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);

	//
	// Iterate through the pathnames and process each one
	//
	TCHAR szFilename[MAX_PATH + 1];
	CString csPathname;
	CString csExpandedFilename;

	for (UINT nFile = 0 ; nFile < nNumFilesDropped; nFile++)
	{
		//
		// Get the pathname
		//
		DragQueryFile(dropInfo, nFile, szFilename, MAX_PATH + 1);

		//
		// It might be shortcut, so try and expand it
		//
		csPathname = szFilename;
		csExpandedFilename = ExpandShortcut(csPathname);
		if(!csExpandedFilename.IsEmpty())
		{
			csPathname = csExpandedFilename;
		}


		//
		// Now see if its something we allow to be dropped
		//
		UINT iPathType = 0;
		if(ValidatePathname(csPathname, iPathType))
		{
			//
			// By default, we insert the filename into the list
			// ourselves, but if our parent wants to do something flashy
			// with it (maybe get the filesize and insert that as an extra
			// column), they will have installed a callback for each
			// droppped item
			//
			if(m_dropMode.iMask & DL_USE_CALLBACK)
			{
				//
				// Let them know about this list control and the item
				// droppped onto it
				//
				if(m_dropMode.pfnCallback)
					m_dropMode.pfnCallback(this, csPathname, iPathType);
			}
			else
			{
				InsertPathname(csPathname);
			}
		}
	}


	//
	// Free the dropped-file info that was allocated by windows
	//
	DragFinish(dropInfo);
}

CString CMusikListCtrl::ExpandShortcut(CString& csFilename) const
{
	USES_CONVERSION;		// For T2COLE() below
	CString csExpandedFile;

	//
    // Make sure we have a path
	//
	if(csFilename.IsEmpty())
	{
		ASSERT(FALSE);
		return csExpandedFile;
	}

	//
    // Get a pointer to the IShellLink interface
	//
    HRESULT hr;
    IShellLink* pIShellLink;

    hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
							IID_IShellLink, (LPVOID*) &pIShellLink);

    if (SUCCEEDED(hr))
    {

		//
        // Get a pointer to the persist file interface
		//
		IPersistFile* pIPersistFile;
        hr = pIShellLink->QueryInterface(IID_IPersistFile, (LPVOID*) &pIPersistFile);

        if (SUCCEEDED(hr))
        {
			//
            // Load the shortcut and resolve the path
			//
            // IPersistFile::Load() expects a UNICODE string
			// so we're using the T2COLE macro for the conversion
			//
			// For more info, check out MFC Technical note TN059
			// (these macros are also supported in ATL and are
			// so much better than the ::MultiByteToWideChar() family)
			//
            hr = pIPersistFile->Load(T2COLE(csFilename), STGM_READ);
			
			if (SUCCEEDED(hr))
			{
				WIN32_FIND_DATA wfd;
				hr = pIShellLink->GetPath(csExpandedFile.GetBuffer(MAX_PATH),
										  MAX_PATH,
										  &wfd,
										  SLGP_UNCPRIORITY);

				csExpandedFile.ReleaseBuffer(-1);
            }
            pIPersistFile->Release();
        }
        pIShellLink->Release();
    }

    return csExpandedFile;
}

BOOL CMusikListCtrl::ValidatePathname(const CString& csPathname, UINT& iPathType) const
{
	//
	// Get some info about that path so we can filter out dirs
	// and files if need be
	//
	BOOL bValid = FALSE;

	struct _stat buf;
	int result = _tstat( csPathname, &buf );
	if( result == 0 )
	{
		//
		// Do we have a directory? (if we want dirs)
		//
		if ((m_dropMode.iMask & DL_ACCEPT_FOLDERS) &&
			((buf.st_mode & _S_IFDIR) == _S_IFDIR)) 
	    {
			bValid = TRUE;
			iPathType = DL_FOLDER_TYPE;
		} 
	    else if ((m_dropMode.iMask & DL_ACCEPT_FILES) &&
				((buf.st_mode & _S_IFREG) == _S_IFREG)) 
	    {
			// 
			// We've got a file and files are allowed.
			//
			iPathType = DL_FILE_TYPE;

			//
			// Now if we are filtering out specific types,
			// check the file extension.
			//
			if(m_dropMode.iMask & DL_FILTER_EXTENSION)
			{
				LPTSTR pszFileExt = PathFindExtension(csPathname);
				if(CString(pszFileExt).CompareNoCase(m_dropMode.csFileExt) == 0)
				{
					bValid = TRUE;
				}
			}
			else
			{
				bValid = TRUE;
			}
		}
	}

	return bValid;
}

int CMusikListCtrl::InsertPathname(const CString& csFilename)
{
	if(!(m_dropMode.iMask & DL_ALLOW_DUPLICATES))
	{
		//
		// We don't allow duplicate pathnames, so
		// see if this one is already in the list.
		//
		LVFINDINFO lvInfo;
		lvInfo.flags = LVFI_STRING;
		lvInfo.psz = csFilename;

		if(FindItem(&lvInfo, -1) != -1)
			return -1;
	}

	return InsertItem(0, csFilename);
}


void CMusikListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default

	if(NCOverride==TRUE) //If the NCOverride is true we remove each time the CListCtrl calc its rect.
	{
		  if(Who==SB_HORZ) ModifyStyle(WS_HSCROLL ,0,0); //Just by modifing the style we remove the scrollbar
		  if(Who==SB_VERT) ModifyStyle(WS_VSCROLL,0,0);
		  if(Who==SB_BOTH) ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0);
	}

	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CMusikListCtrl::HideScrollBars(int Type, int Which)
{
	if(Type==LCSB_CLIENTDATA) //This is the clientrect function
	{
		RECT ierect;
		int cxvs, cyvs;
		GetClientRect(&ierect); //Get client width and height

		cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT
		cyvs = GetSystemMetrics (SM_CYVSCROLL); //Get the system metrics - HORZ
		
		if(Which==SB_HORZ) cxvs=0; //Set VERT to zero when choosen HORZ
		if(Which==SB_VERT) cyvs=0; //Set HORZ to zero when choosen VERT

		//Here we set the position of the window to the clientrect + the size of the scrollbars
		SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom+cyvs, SWP_NOMOVE | SWP_NOZORDER);

		//Her we modify the rect so the right part is subbed from the rect.
		if(Which==SB_BOTH||Which==SB_HORZ) ierect.bottom -= ierect.top;
		if(Which==SB_BOTH||Which==SB_VERT) ierect.right -= ierect.left;

		//Just to be safe that the left/top corner is 0...
		ierect.top = 0;
		ierect.left = 0;
		
		HRGN iehrgn = NULL; //This range is created base on which scrollbar that is going to be removed!

		//The -2 is probably a border of some kind that we also need to remove. I could not find any good
		//metrics that gave me an 2 as an answer. So insted we makes it static with -2.
		if(Which==SB_BOTH) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom-2);
		if(Which==SB_HORZ) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom-2);
		if(Which==SB_VERT) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom);
		
		//After the range has been made we add it...
		SetWindowRgn (iehrgn, TRUE);

		//Reset of NCOverride
		NCOverride=FALSE;
	}

	if(Type==LCSB_NCOVERRIDE) //This is the NcCalcSize override
	{
		NCOverride=TRUE; //Set to true, so we run the code on each OnNcCalcSize.
		Who=Which; //Selects which scrollbars to get hidden.
	}
}
