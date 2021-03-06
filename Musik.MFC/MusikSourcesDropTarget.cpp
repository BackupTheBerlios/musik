///////////////////////////////////////////////////

#include "stdafx.h"
#include "MusikSourcesDropTarget.h"

#include "MusikSourcesCtrl.h"

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

CMusikSourcesDropTarget::CMusikSourcesDropTarget( CMusikSourcesCtrl* pList, UINT uSourceID )
{
	m_pList = pList;
	m_piDropHelper = NULL;
	m_bUseDnDHelper = false;
	m_uSourceID = uSourceID;

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

CMusikSourcesDropTarget::~CMusikSourcesDropTarget()
{
    if ( m_piDropHelper != NULL )
        m_piDropHelper->Release();
}

///////////////////////////////////////////////////

DROPEFFECT CMusikSourcesDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
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

DROPEFFECT CMusikSourcesDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
    if ( pDataObject->GetGlobalData ( m_uSourceID ) == NULL )
	{
		// Look for CF_HDROP data in the data object, and accept the drop if
		// it's there.
		if ( pDataObject->GetGlobalData ( CF_HDROP ) != NULL )
			dwEffect = DROPEFFECT_COPY;
	}

    // Call the DnD helper.
	if ( m_bUseDnDHelper )
		m_piDropHelper->DragOver ( &point, dwEffect );

    return dwEffect;
}

///////////////////////////////////////////////////

BOOL CMusikSourcesDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	BOOL bRet;

    // Read the CF_HDROP data and put the files in the main window's list.
	bRet = ReadHdropData ( pDataObject );

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
    
    return bRet;
}

///////////////////////////////////////////////////

void CMusikSourcesDropTarget::OnDragLeave ( CWnd* pWnd )
{
	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->DragLeave();
	}
}

///////////////////////////////////////////////////

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.

BOOL CMusikSourcesDropTarget::ReadHdropData ( COleDataObject* pDataObject )
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
