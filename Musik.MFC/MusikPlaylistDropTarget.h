///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

struct IDropTargetHelper;  
class CMusikPlaylistCtrl;

///////////////////////////////////////////////////

class CMusikPlaylistDropTarget : public COleDropTarget  
{
public:
	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
				
	void OnDragLeave ( CWnd* pWnd );

    CMusikPlaylistDropTarget ( CMusikPlaylistCtrl* pList, UINT uSourceID );
	virtual ~CMusikPlaylistDropTarget();

protected:
    CMusikPlaylistCtrl*	m_pList;
    IDropTargetHelper*	m_piDropHelper;
    bool				m_bUseDnDHelper;
	UINT				m_uSourceID;

    BOOL ReadHdropData ( COleDataObject* pDataObject );
};

///////////////////////////////////////////////////
