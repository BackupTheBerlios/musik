/*
 *  ActivityBox.cpp
 *
 *  The infamous "Activity Box" control
 *	These controls are visible at the top of the main dialog.
 *	They contain lists of artists / albums / genres / etc.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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

#define MUSIK_ACT_TEXT 9998

//--- for CMusikSongArray ---//
#include "MusikLibrary.h"

//--- forward declarations ---//
enum EMUSIK_ACTIVITY_TYPE;
enum EMUSIK_LIB_TYPE;
class CActivityBox;
class MusikActivityRenameThread;

class CActivityListBox : public wxListCtrl
{
public:
	CActivityListBox( CActivityBox *parent, wxWindowID id );
	~CActivityListBox();

	//--- gets ---//
	wxString				GetFirstSel	();
	void					GetSelected	( wxArrayString & aReturn );
	const wxArrayString&	GetList		() const { return m_Items; }

	//--- sets ---//
	void SetList		(const  wxArrayString & );
	void SetSel			( const  wxArrayString & aList );
	void SetSel			( const wxString & sel, bool bDeselectAllFirst = true);
	void SetRelated		( int n ){ m_Related = n; }

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

	inline bool		HasShowAllRow	() const; 
	long			GetRowCount		() const { return HasShowAllRow()?( long )m_Items.GetCount()+ 1: ( long )m_Items.GetCount(); }
	inline wxString GetRowText		( long row ) const ;

	//--- stripe colors ---//
	wxListItemAttr m_LightAttr;
	wxListItemAttr m_DarkAttr;
	wxListItemAttr m_ActiveAttr;
	wxListItemAttr m_AllReset;
	
	int m_Related;

	wxArrayString m_Items;
	CActivityBox *m_pParent;
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
	CActivityBox( wxWindow *parent, wxWindowID id, EMUSIK_ACTIVITY_TYPE nType );
	~CActivityBox();
	
	//--- thread event handlers ---//
	void StartRenameThread		( int mode, const wxArrayString & sel, wxString newvalue );
	void OnRenameThreadStart	( wxCommandEvent& WXUNUSED(event) );
	void OnRenameThreadEnd		( wxCommandEvent& WXUNUSED(event) );
	void OnRenameThreadProg		( wxCommandEvent& WXUNUSED(event) );

	//--- sets ---//
	void SetCaption				( wxString sCaption )										{ pHeader->SetCaption( sCaption );						}
	void SetActivityType		( EMUSIK_ACTIVITY_TYPE  nType )								{ m_ActivityType = nType;								}
	void DeselectAll			()															{ pListBox->DeselectAll();								}
	void SetSel					( const wxString & sel, bool bDeselectAllFirst = true )		{ pListBox->SetSel( sel , bDeselectAllFirst );			}
	void SetSel					( const  wxArrayString & aList )							{ pListBox->SetSel( aList );							}	
	void SetDropTarget			(wxDropTarget* target)										{ pListBox->SetDropTarget( target );					}	
	
	//--- gets ---//
	int			GetListId				()													{ return pListBox->GetId();								}
	wxString	GetFirstSel				()													{ return pListBox->GetFirstSel( ); 						}
	void		GetSelected				( wxArrayString & aReturn )							{ pListBox->GetSelected( aReturn );return; 				}
	int			GetSelectedItemCount	()													{ return pListBox->GetSelectedItemCount();				}
	void		GetRelatedList			( CActivityBox *pDst, wxArrayString & aReturn );
	wxString	GetActivityTypeStr		();
	void		GetSelectedSongs		( CMusikSongArray& array );

	CActivityListBox*		GetListBox		()												{ return pListBox;										}
	EMUSIK_ACTIVITY_TYPE	GetActivityType	()												{ return m_ActivityType;								}

	//--- tag editing ---//
	void EditBegin			();
	void EditCommit			();
	void EditCancel			();
	void EditDone			();
	bool EditVisible		(){ return m_EditVisible ; }

	//--- drag 'n drop ---//
	void		DNDBegin			();
	wxString	DNDGetList			();

	//---misc ---//
	bool IsSelected			( int n )							{ return pListBox->IsSelected( n );		}
	void Update				( bool selectnone = true )			{ pListBox->Update( selectnone );		}	
	void RescaleColumns		()									{ pListBox->RescaleColumns();			}
	void ResetCaption		();
	void ResetContents		();
	void SetContents		( const wxArrayString &list ); 
	void GetFullList		( wxArrayString & aReturn );
	void SetPlaylist		();
	void EnableProgress		( bool enable = true );
	void SetRelated			( int n ){ pListBox->SetRelated( n ); }

	//--- custom, thread event handlers ---//
	void SetProgress			( int n )				{ m_Progress = n; }
	void SetProgressType		( int n )				{ m_ProgressType = n;		}
	void SetActiveThread		( wxThread* thread )	{ m_ActiveThread = thread;	}	

	int GetProgress()			{ return m_Progress;		}
	int GetProgressType()		{ return m_ProgressType;	}
	wxThread* GetActiveThread()	{ return m_ActiveThread;	}

	DECLARE_EVENT_TABLE()

private:

	EMUSIK_LIB_TYPE ACTIVITY_TYPE2LIB_TYPE ( EMUSIK_ACTIVITY_TYPE lbtype );
	EMUSIK_ACTIVITY_TYPE   m_ActivityType;
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
