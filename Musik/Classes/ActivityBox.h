/*
 *  ActivityBox.cpp
 *
 *  The infamous "Activity Box" control
 *	These controls are visible at the top of the main dialog.
 *	They contain lists of artists / albums / genres / etc.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef ACTIVITY_BOX
#define ACTIVITY_BOX

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "wx/listctrl.h"
#include "wx/thread.h"
#include "wx/dnd.h"

#include "ListHeader.h"

enum EACTIVITY_TYPE
{
	ACTIVITY_NONE = 0,
	ACTIVITY_ARTISTS,
	ACTIVITY_ALBUMS,
	ACTIVITY_GENRES,
	ACTIVITY_YEARS,
	ACTIVITY_DECADES
};

#define MUSIK_ACT_TEXT 9998

//--- forward declaration for threads ---//
class CActivityBox;
class MusikActivityRenameThread;

class CActivityListBox : public wxListCtrl
{
public:
	CActivityListBox( wxPanel *parent, wxArrayString *items, wxWindowID id );
	~CActivityListBox();

	//--- gets ---//
	wxString		GetFirstSel	();
	wxArrayString	GetSelected	();

	//--- sets ---//
	void			SetList		( wxArrayString * );
	wxArrayString*	GetList		(){ return pItems; }
	void			SetSel		( wxArrayString * );
	void			SetSel		( wxString sel );
	void			SetRelated	( int n ){ m_Related = n; }

	//--- others ---//
	void DeselectAll	();
	bool IsSelected		( int n );
	void Update			( bool selectnone );
	void RescaleColumns	();

private:
	//--- virtual functions ---//
    virtual	wxString		OnGetItemText	(long item, long column) const;
    virtual	wxListItemAttr*	OnGetItemAttr	(long item) const;
	virtual	int				OnGetItemImage	(long item) const { return 0; }

	//--- stripe colors ---//
	wxListItemAttr m_LightAttr;
	wxListItemAttr m_DarkAttr;
	wxListItemAttr m_ActiveAttr;
	wxListItemAttr m_AllReset;
	
	int m_Related;

	wxArrayString *pItems;
};

class CActivityBoxEvt : public wxEvtHandler
{
public:
	CActivityBoxEvt( CActivityBox *parent, CActivityListBox *box ){ pParent = parent; pListBox = box; };
	~CActivityBoxEvt(){};

	void TranslateKeys	( wxKeyEvent& event	);

	DECLARE_EVENT_TABLE()
private:
	CActivityBox	 *pParent;
	CActivityListBox *pListBox;
};

class CActivityEditEvt : public wxEvtHandler
{
public:
	CActivityEditEvt( CActivityBox *parent, wxTextCtrl *textctrl ){ pParent = parent; pTextCtrl = textctrl; };
	~CActivityEditEvt(){};
	void TranslateKeys( wxKeyEvent& event );
	void EditCommit();

	DECLARE_EVENT_TABLE()
private:
	CActivityBox	*pParent;
	wxTextCtrl		*pTextCtrl;
};


class CActivityBox : public wxPanel
{
public:
	CActivityBox( wxWindow *parent, wxArrayString *items, wxWindowID id, int nType );
	~CActivityBox();
	
	//--- thread event handlers ---//
	void StartRenameThread		( int mode, wxArrayString sel, wxString newvalue );
	void OnRenameThreadStart	( wxCommandEvent& WXUNUSED(event) );
	void OnRenameThreadEnd		( wxCommandEvent& WXUNUSED(event) );
	void OnRenameThreadProg		( wxCommandEvent& WXUNUSED(event) );

	//--- gets / sets ---//
	void			SetCaption				( wxString sCaption )				{ pHeader->SetCaption( sCaption );						}
	int				GetActivityType			()									{ return m_ActivityType;								}
	wxString		GetActivityTypeStr		();
	void			SetActivityType			( int nType )						{ m_ActivityType = nType;								}
	void			DeselectAll				()									{ pListBox->DeselectAll();								}
	void			SetSel					( wxString sel )					{ pListBox->SetSel( sel );								}
	void			SetSel					( wxArrayString *aList )			{ pListBox->SetSel( aList );							}	
	bool			IsSelected				( int n )							{ return pListBox->IsSelected( n );						}
	wxString		GetFirstSel				()									{ return pListBox->GetFirstSel();						}
	wxArrayString	GetSelected				()									{ return pListBox->GetSelected();						}
	int				GetSelectedItemCount	()									{ return pListBox->GetSelectedItemCount();				}
	wxArrayString	GetRelatedList			( CActivityBox *pDst );
	void			SetDropTarget			(wxDropTarget* target)				{ pListBox->SetDropTarget( target );					}

	//--- right click editing ---//
	void EditBegin			();
	void EditCommit			();
	void EditCancel			();
	void EditDone			();
	bool EditVisible		(){ return m_EditVisible ; }

	//--- drag 'n drop ---//
	void		DNDBegin			();
	wxString	DNDGetList			();

	//---misc ---//
	void			Update				( bool selectnone = true )			{ pListBox->Update( selectnone );		}	
	void			RescaleColumns		()									{ pListBox->RescaleColumns();			}
	void			ResetCaption		();
	void			ResetContents		();
	void			SetContents			( wxArrayString *list );
	wxArrayString	GetFullList			();
	void			SetPlaylist			();
	void			EnableProgress		( bool enable = true );
	void			SetRelated			( int n ){ pListBox->SetRelated( n ); }
	int				GetListId			(){ return pListBox->GetId(); }
	CActivityListBox*	GetListBox		(){ return pListBox; }

	//--- custom, thread event handlers ---//
	void SetProgress			( int n )				{ m_Progress = n; }
	void SetProgressType		( int n )				{ m_ProgressType = n;		}
	void SetActiveThread		( wxThread* thread )	{ m_ActiveThread = thread;	}	

	int GetProgress()			{ return m_Progress;		}
	int GetProgressType()		{ return m_ProgressType;	}
	wxThread* GetActiveThread()	{ return m_ActiveThread;	}

	DECLARE_EVENT_TABLE()
private:

	int  m_ActivityType;
	bool m_EditVisible;

	wxBoxSizer			*pSizer;
	wxTextCtrl			*pEdit;
	wxGauge				*pProgress;
	CListHeader			*pHeader;
	CActivityListBox	*pListBox;
	CActivityBoxEvt		*pActivityBoxEvt;
	CActivityEditEvt	*pActivityEditEvt;

	//--- thread stuff ---//
	MusikActivityRenameThread	*pRenameThread;
	int m_Progress;
	int m_ProgressType;
	wxThread* m_ActiveThread;
};

class ActivityDropTarget : public wxTextDropTarget
{
public:
	ActivityDropTarget( CActivityBox *pSList )	{ pActivityCtrl = pSList; pList = pSList->GetListBox(); }

	virtual bool			OnDropText(wxCoord x, wxCoord y, const wxString& text);
	virtual wxDragResult	OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
	void					HighlightSel( wxPoint );

private:
	CActivityBox *pActivityCtrl;
	CActivityListBox *pList;
	int nLastHit;
};

#endif
