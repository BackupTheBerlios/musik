///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikPlaylistDropTarget
//
// Filename(s): 
//
//   musikPlaylistDropTarget.h, musikPlaylistDropTarget.cpp
//
// Information:
//
//   A drop target for the playlist to allow files from anywhere,
//   including windows explorer, to be dropped onto a CmusikPlaylist.
// 
// Usage: 
//
//   Construct in a CmusikPlaylist, then use 
//   CmusikPlaylistDropTarget::Register() to invoke it, 
//   CmusikPlaylistDropTarget::Revoke() to remove it.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlaylistDropTarget.h"

#include "musikPlaylistCtrl.h"

///////////////////////////////////////////////////

// uncomment the following if the latest
// platform SDK is currently installed...

struct IDropTargetHelper : public IUnknown
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // IDropTargetHelper
    STDMETHOD (DragEnter)(THIS_ HWND hwndTarget, IDataObject* pDataObject,
                          POINT* ppt, DWORD dwEffect) PURE;
    STDMETHOD (DragLeave)(THIS) PURE;
    STDMETHOD (DragOver)(THIS_ POINT* ppt, DWORD dwEffect) PURE;
    STDMETHOD (Drop)(THIS_ IDataObject* pDataObject, POINT* ppt,
                     DWORD dwEffect) PURE;
    STDMETHOD (Show)(THIS_ BOOL fShow) PURE;
};

// {4657278A-411B-11d2-839A-00C04FD918D0}
extern "C" const GUID __declspec(selectany) CLSID_DragDropHelper = 
    { 0x4657278a, 0x411b, 0x11d2, { 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0 }};

// {4657278B-411B-11d2-839A-00C04FD918D0}
extern "C" const GUID __declspec(selectany) IID_IDropTargetHelper = 
    { 0x4657278b, 0x411b, 0x11d2, { 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0 }};

///////////////////////////////////////////////////

CmusikPlaylistDropTarget::CmusikPlaylistDropTarget( CmusikPlaylistCtrl* pList, UINT uSourceID, UINT uSelectionID_L, UINT uSelectionID_R )
{
	m_pList = pList;
	m_piDropHelper = NULL;
	m_bUseDnDHelper = false;
	m_uSourceID = uSourceID;
	m_uSelectionID_L = uSelectionID_L;
	m_uSelectionID_R = uSelectionID_R;

    // Create an instance of the shell DnD helper object.
    if ( SUCCEEDED( CoCreateInstance ( 
		CLSID_DragDropHelper, 
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper,
		(void**) &m_piDropHelper ) ) )
        {
			m_bUseDnDHelper = true;
        }
}

///////////////////////////////////////////////////

CmusikPlaylistDropTarget::~CmusikPlaylistDropTarget()
{
    if ( m_piDropHelper != NULL )
        m_piDropHelper->Release();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
	if ( pDataObject->GetGlobalData( m_uSelectionID_L ) || pDataObject->GetGlobalData( m_uSelectionID_R ) )
		return DROPEFFECT_NONE;

	if ( pDataObject->GetGlobalData ( m_uSourceID ) == NULL )
	{
		// Look for CF_HDROP data in the data object, and accept the drop if
		// it's there.
		if ( pDataObject->GetGlobalData ( CF_HDROP ) != NULL )
			dwEffect = DROPEFFECT_COPY;
	}

    // Call the DnD helper.
    if ( m_bUseDnDHelper )
	{
        // The DnD helper needs an IDataObject interface, so get one from
        // the COleDataObject.  Note that the FALSE param means that
        // GetIDataObject will not AddRef() the returned interface, so 
        // we do not Release() it.

        IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
        m_piDropHelper->DragEnter ( pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect );                                    
	}

    return dwEffect;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we want to
	// rearrange items. Set the flag in the playlist control.
	if ( pDataObject->GetGlobalData( m_uSelectionID_L ) || pDataObject->GetGlobalData( m_uSelectionID_R ) )
		return DROPEFFECT_NONE;

    if ( pDataObject->GetGlobalData ( m_uSourceID ) != NULL )
		m_pList->m_DropArrange = true;
	else
		m_pList->m_DropArrange = false;

	// Look for CF_HDROP data in the data object, and accept the drop if
	// it's there.
	if ( pDataObject->GetGlobalData ( CF_HDROP ) != NULL )
		dwEffect = DROPEFFECT_COPY;

    // Call the DnD helper.
	if ( m_bUseDnDHelper )
		m_piDropHelper->DragOver ( &point, dwEffect );

    return dwEffect;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	BOOL bRet;

    // Call the DnD helper.
	if ( m_bUseDnDHelper )
	{
		// The DnD helper needs an IDataObject interface, so get one from
		// the COleDataObject.  Note that the FALSE param means that
		// GetIDataObject will not AddRef() the returned interface, so 
		// we do not Release() it.
		IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
		m_piDropHelper->Drop ( piDataObj, &point, dropEffect );
	}

    // Read the CF_HDROP data call list's OnDrop()
	bRet = ReadHdropData ( pDataObject );

    return bRet;
}

///////////////////////////////////////////////////

void CmusikPlaylistDropTarget::OnDragLeave ( CWnd* pWnd )
{
	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->DragLeave();
	}
}

///////////////////////////////////////////////////

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.

BOOL CmusikPlaylistDropTarget::ReadHdropData ( COleDataObject* pDataObject )
{
	HGLOBAL     hg;
	HDROP       hdrop;

	// Get the HDROP data from the data object.
    hg = pDataObject->GetGlobalData ( CF_HDROP );
	if ( hg == NULL )
	{
		return FALSE;
	}

	hdrop = (HDROP) GlobalLock ( hg );
	if ( hdrop == NULL )
	{
		GlobalUnlock ( hg );
		return FALSE;
	}

	m_pList->OnDropFiles( hdrop );

    GlobalUnlock ( hg );

    return TRUE;
}

///////////////////////////////////////////////////
