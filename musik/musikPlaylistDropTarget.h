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

#pragma once

///////////////////////////////////////////////////

struct IDropTargetHelper;  
class CmusikPlaylistCtrl;

///////////////////////////////////////////////////

class CmusikPlaylistDropTarget : public COleDropTarget  
{
public:
	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
				
	void OnDragLeave ( CWnd* pWnd );

    CmusikPlaylistDropTarget ( CmusikPlaylistCtrl* pList, UINT uSourceID, UINT uSelectionID_L, UINT uSelectionID_R );
	virtual ~CmusikPlaylistDropTarget();

protected:
    CmusikPlaylistCtrl*	m_pList;
    IDropTargetHelper*	m_piDropHelper;
    bool				m_bUseDnDHelper;
	UINT				m_uSourceID;
	UINT				m_uSelectionID_L;
	UINT				m_uSelectionID_R;

    BOOL ReadHdropData ( COleDataObject* pDataObject );
};

///////////////////////////////////////////////////
