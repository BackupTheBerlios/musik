///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

struct IDropTargetHelper;  
class CmusikSourcesCtrl;

///////////////////////////////////////////////////

class CmusikSourcesDropTarget : public COleDropTarget  
{
public:
	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
				
	void OnDragLeave ( CWnd* pWnd );

    CmusikSourcesDropTarget ( CmusikSourcesCtrl* pList, UINT uSourceID, UINT uPlaylistID_R, UINT uSelectionID_R );
	virtual ~CmusikSourcesDropTarget();

protected:
    CmusikSourcesCtrl*	m_pList;
    IDropTargetHelper*	m_piDropHelper;
    bool				m_bUseDnDHelper;
	bool				m_RightButton;
	UINT				m_uSourceID;
	UINT				m_uPlaylistID_R;	// if the user holds the right mouse button from either the
	UINT				m_uSelectionID_R;	// selection boxes or the playlist, switch to the target...

    BOOL ReadHdropData ( COleDataObject* pDataObject );
};

///////////////////////////////////////////////////
