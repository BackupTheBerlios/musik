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

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "ActivityBox.h"

//--- globals ---//
#include "../MusikUtils.h"
#include "../MusikGlobals.h"

//--- frames ---//
#include "../Frames/MusikFrame.h"

//--- threads ---//
#include "../Threads/ActivityBoxThreads.h"

//-----------------------//
//--- CActivityBoxEvt ---//
//-----------------------//
BEGIN_EVENT_TABLE(CActivityBoxEvt, wxEvtHandler)
	EVT_CHAR				( CActivityBoxEvt::TranslateKeys	)
END_EVENT_TABLE()

void CActivityBoxEvt::TranslateKeys( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_F2 )
		pParent->EditBegin();
	event.Skip();
}

//------------------------//
//--- CActivityEditEvt ---//
//------------------------//
BEGIN_EVENT_TABLE(CActivityEditEvt, wxEvtHandler)
	EVT_CHAR	( CActivityEditEvt::TranslateKeys )
END_EVENT_TABLE()

void CActivityEditEvt::TranslateKeys( wxKeyEvent& event )
{
	int nKey = event.GetKeyCode();
	if ( nKey == WXK_ESCAPE )
		pParent->EditCancel();
	else if ( nKey == WXK_RETURN )
		pParent->EditCommit();
	event.Skip( TRUE );
}

//------------------------//
//--- CActivityListBox ---//
//------------------------//
CActivityListBox::CActivityListBox( wxPanel *parent, wxArrayString *items,  wxWindowID id )
	: wxListCtrl( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxLC_NO_HEADER | wxLC_REPORT | wxLC_VIRTUAL | wxNO_BORDER & ~wxHSCROLL & ~wxLC_SINGLE_SEL )
{
	m_Related = 0;
	pItems = items;
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );

	this->InsertColumn( 0, wxT(""), wxLIST_FORMAT_LEFT, 0 );
	this->InsertColumn( 1, wxT(""), wxLIST_FORMAT_LEFT, 0 );
}

void CActivityListBox::RescaleColumns()
{
	Freeze();

	int nWidth, nHeight;
	GetClientSize	( &nWidth, &nHeight );
	SetColumnWidth	( 0, 0 );
	
	#ifdef __WXMSW__
		SetColumnWidth	( 1, nWidth );
	#elif defined __WXGTK__
		SetColumnWidth( 1, nWidth - wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y) - GetColumnWidth( 0 ) - 1 );			
	#endif 

	Thaw();
	Refresh();
}

void CActivityListBox::SetList( wxArrayString* pList )
{
	*pItems = *pList;
}

void CActivityListBox::Update( bool selnone )
{
	//----------------------------------------------------------------------------------//
	//---        note that the activity box control is now virtual, so we don't      ---//
	//---    add items directly to it.  Instead, we have the object pItems a		 ---//
	//---  *wxArrayString, that the virtual listctrl then references via callbacks   ---//
	//----------------------------------------------------------------------------------//
	Freeze();

	//--- update colors from prefs ---//
	m_LightAttr		= wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), wxNullFont );
	m_DarkAttr		= wxListItemAttr( *wxBLACK, StringToColour( g_Prefs.sActStripeColour ), wxNullFont );
	m_ActiveAttr	= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ), wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ), wxNullFont );
	m_AllReset		= wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), g_fntListBold );

	SetItemCount( ( long )pItems->GetCount() );
	if ( selnone )
		wxListCtrlSelNone( this );
	Thaw();
	Refresh( FALSE );
	RescaleColumns();
}

//---------------------------------------------------//
//--- virtual functions. careful with these ones, ---// 
//---        boys. they drive everything          ---//
//---------------------------------------------------//
wxString CActivityListBox::OnGetItemText(long item, long column) const
{
	switch ( column )
	{
	case 0:
		break;

	case 1:
		return SanitizedString( pItems->Item( item ) );
		break;
	}
	return wxT("");
}

wxListItemAttr* CActivityListBox::OnGetItemAttr(long item) const
{
	if ( g_Prefs.nSelStyle == 2 && item < m_Related )
		return ( wxListItemAttr* )&m_AllReset;

	else if ( ( g_Prefs.nSelStyle == 0 || g_Prefs.nSelStyle == 1 ) && item == 0 )
		return ( wxListItemAttr* )&m_AllReset;

	if ( g_Prefs.nActStripes == 1 )
		return item % 2 ? (wxListItemAttr *)&m_DarkAttr : (wxListItemAttr *)&m_LightAttr;

	return ( wxListItemAttr* )&m_LightAttr;
}

CActivityListBox::~CActivityListBox()
{
}

bool CActivityListBox::IsSelected( int n )
{
	if ( GetItemState( n, wxLIST_STATE_SELECTED ) > 2 )
		return true;
	else
		return false;
}

void CActivityListBox::DeselectAll()
{
	wxListCtrlSelNone( this );
}

void CActivityListBox::SetSel( wxArrayString *aSel )
{
	DeselectAll();
	int nPos = -1;
	for ( size_t i = 0; i < aSel->GetCount(); i++ )
	{
		nPos = FindItem( -1, aSel->Item( i ) );
		if ( nPos > -1 )
			SetItemState( nPos, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	}
}

void CActivityListBox::SetSel( wxString sel )
{
	DeselectAll();
	for ( size_t i = 0; i < pItems->GetCount(); i++ )
	{
		if ( pItems->Item( i ) == sel )
			SetItemState( i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}
}

wxArrayString CActivityListBox::GetSelected()
{
	wxArrayString aReturn;
	int nIndex = -1;
	for( ;; )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex == -1 )
			break;
		aReturn.Add( pItems->Item( nIndex ) );
	}
	return aReturn;
}

wxString CActivityListBox::GetFirstSel()
{
	bool bUnknown = false;
	int nIndex = -1;
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex > -1 )
		{
			wxListItem item;
			item.SetId( nIndex );
			item.SetColumn( 1 );
			item.SetMask( wxLIST_MASK_TEXT );
			GetItem( item );
			if ( pItems->Item( nIndex ) != _("<unknown>") )
				return( pItems->Item( nIndex ) );
			else
				bUnknown = true;

		}
	}
	if ( bUnknown )
		return _("<unknown>");
	else
		return wxT("");
}


//--------------------//
//--- CActivityBox ---//
//--------------------//

BEGIN_EVENT_TABLE(CActivityBox, wxPanel)
	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU			( MUSIK_ACTIVITY_RENAME_THREAD_START,	CActivityBox::OnRenameThreadStart	)
	EVT_MENU			( MUSIK_ACTIVITY_RENAME_THREAD_END,		CActivityBox::OnRenameThreadEnd		)
	EVT_MENU			( MUSIK_ACTIVITY_RENAME_THREAD_PROG,	CActivityBox::OnRenameThreadProg	)
END_EVENT_TABLE()

//-----------------//
//--- dnd stuff ---//
//-----------------//
bool ActivityDropTarget::OnDropText( wxCoord x, wxCoord y, const wxString &text )
{
	wxString sFiles = text;
	long n;
	if ( sFiles != wxT( "" ) )
	{
		//--- separate values from type. "s\n" for Sources, "p\n" for playlist, "a\n" for activity box ---//
		wxString sType = sFiles.Left( 2 );
		sFiles = sFiles.Right( sFiles.Length() - 2 );

		//--- where did we land? ---//
		const wxPoint& pt = wxPoint( x, y );
		int nFlags;
		n = pList->HitTest( pt, nFlags );

		//--- hit nothing, do nothing ---//
		if ( n < 0 )
			return FALSE;

		//--- drag originated from playlist or activity area (others are ignored) ---//
		if ( sType == wxT( "p\n" ) )	
		{
			//--- drag over an existing item, convert id3 tags ---//
			wxArrayString files;
			wxString file = sFiles.BeforeFirst( wxT('\n') );
			while ( !file.IsEmpty() )
			{

				files.Add( file );

				sFiles = sFiles.AfterFirst( wxT('\n') );

				file = sFiles.BeforeFirst( wxT('\n') );

			}
			pActivityCtrl->StartRenameThread( 
				ACTIVITY_RENAME_SONGS,
				files, 
				SanitizedString( pList->GetList()->Item( n ) ) );
		}
	}

	return TRUE;
}

wxDragResult ActivityDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	// Disallow further drag and drop if a thread is happening.
	if ( g_MusikFrame->GetActiveThread() )
		return wxDragNone;

	const wxPoint& pt = wxPoint( x, y );
	HighlightSel( pt );
	return wxDragMove;
}

void ActivityDropTarget::HighlightSel( wxPoint pPos )
{
	g_DragInProg = true;
	int nFlags;
	long n = pList->HitTest( pPos, nFlags );
	if ( ( n > 0 ) && ( n != nLastHit ) )
	{
		wxListCtrlSelNone( pList );
		pList->SetItemState( n, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}
	nLastHit = n;
}

CActivityBox::CActivityBox( wxWindow *parent, wxArrayString *items, wxWindowID id, int nType )
	:  wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxSIMPLE_BORDER | wxCLIP_CHILDREN )
{
	//--- CListHeader and CActivityListBox ---//
	pHeader		= new CListHeader		( this, wxT("") );
	pListBox	= new CActivityListBox	( this, items, id );
	
	//--- drag and drop handler ---//
	pListBox->SetDropTarget( new ActivityDropTarget( this ) );

	//--- text control to change val ---//
	pEdit		= new wxTextCtrl( this, MUSIK_ACT_TEXT, wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ), wxSIMPLE_BORDER | wxTE_PROCESS_ENTER );

	//--- top sizer ---//
	pSizer = new wxBoxSizer( wxVERTICAL );
	pSizer->Add( pHeader, 0, wxEXPAND | wxBOTTOM, 0 );
	pSizer->Add( pListBox, 1, wxEXPAND, 0 );
	pSizer->Add( pEdit, 0, wxEXPAND, 0 );
	pSizer->Show( pEdit, FALSE );

	pEdit->Enable( FALSE );

	SetSizerAndFit( pSizer );
	Layout();

	//--- push event handlers ---//
	pActivityBoxEvt = new CActivityBoxEvt( this, pListBox );
	pListBox->PushEventHandler( pActivityBoxEvt );

	pActivityEditEvt = new CActivityEditEvt( this, pEdit );
	pEdit->PushEventHandler( pActivityEditEvt );

	m_EditVisible = false;
	m_ActivityType = nType;
}

CActivityBox::~CActivityBox()
{
	delete pHeader;
	delete pListBox;
	delete pEdit;
	delete pActivityBoxEvt;
	delete pActivityEditEvt;
}

wxString CActivityBox::GetActivityTypeStr()
{
	switch ( GetActivityType() )
	{
	case ACTIVITY_ARTISTS:
		return wxT( "artist" );
		break;
	case ACTIVITY_ALBUMS:
		return wxT( "album" );
		break;
	case ACTIVITY_GENRES:
		return wxT( "genre" );
		break;
	case ACTIVITY_YEARS:
		return wxT( "year" );
		break;
	}
	return wxT( "" );
}

wxArrayString CActivityBox::GetRelatedList( CActivityBox *pDst )
{
	int nDstType = pDst->GetActivityType();
	wxArrayString sel = GetSelected();
	if ( m_ActivityType == MUSIK_LBTYPE_ARTISTS )
	{
		if		( nDstType == MUSIK_LBTYPE_ALBUMS	)	return	g_Library.GetArtistAlbums	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_GENRES	)	return	g_Library.GetArtistGenres	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_YEARS	)	return	g_Library.GetArtistYears	( &sel );
	}

	else if ( m_ActivityType == MUSIK_LBTYPE_ALBUMS )
	{
		if		( nDstType == MUSIK_LBTYPE_ARTISTS	)	return	g_Library.GetAlbumArtists	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_GENRES	)	return	g_Library.GetAlbumGenres	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_YEARS	)	return	g_Library.GetAlbumYears		( &sel );
	}

	else if ( m_ActivityType == MUSIK_LBTYPE_GENRES )
	{
		if		( nDstType == MUSIK_LBTYPE_ARTISTS	)	return	g_Library.GetGenreArtists	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_ALBUMS	)	return	g_Library.GetGenreAlbums	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_YEARS	)	return	g_Library.GetGenreYears		( &sel );
	}

	else if ( m_ActivityType == MUSIK_LBTYPE_YEARS )
	{
		if		( nDstType == MUSIK_LBTYPE_ARTISTS	)	return	g_Library.GetYearArtists	( &sel );
		else if ( nDstType == MUSIK_LBTYPE_ALBUMS	)	return	g_Library.GetYearAlbums		( &sel );
		else if ( nDstType == MUSIK_LBTYPE_GENRES	)	return	g_Library.GetYearGenres		( &sel );
	}
	wxArrayString aNullArray;
	return aNullArray;
}

void CActivityBox::ResetCaption()
{
	if		( m_ActivityType == MUSIK_LBTYPE_ARTISTS	)	SetCaption( _("Artists") );
	else if ( m_ActivityType == MUSIK_LBTYPE_ALBUMS		)	SetCaption( _("Albums") );
	else if ( m_ActivityType == MUSIK_LBTYPE_GENRES		)	SetCaption( _("Genres") );
	else if ( m_ActivityType == MUSIK_LBTYPE_YEARS		)	SetCaption( _("Years") );
}

void CActivityBox::ResetContents()
{
	wxArrayString list;
	if			( m_ActivityType == MUSIK_LBTYPE_ARTISTS	)	list = g_Library.GetAllArtists();
	else if		( m_ActivityType == MUSIK_LBTYPE_ALBUMS		)	list = g_Library.GetAllAlbums();
	else if		( m_ActivityType == MUSIK_LBTYPE_GENRES		)	list = g_Library.GetAllGenres();
	else if		( m_ActivityType == MUSIK_LBTYPE_YEARS		)	list = g_Library.GetAllYears();
	SetContents( &list );
}

wxArrayString CActivityBox::GetFullList()
{
	if			( m_ActivityType == MUSIK_LBTYPE_ARTISTS	)	return g_Library.GetAllArtists();
	else if		( m_ActivityType == MUSIK_LBTYPE_ALBUMS		)	return g_Library.GetAllAlbums();
	else if		( m_ActivityType == MUSIK_LBTYPE_GENRES		)	return g_Library.GetAllGenres();
	else if		( m_ActivityType == MUSIK_LBTYPE_YEARS		)	return g_Library.GetAllYears();
	
	//--- if we fail for some reason, return a blank list ---//
	wxArrayString blank;
	return blank;
}

void CActivityBox::SetPlaylist()
{
	//--- if we show unselected entries ---//
	if ( g_Prefs.nSelStyle == 1 || g_Prefs.nSelStyle == 2 || ( g_ActivityAreaCtrl->GetParentBox() == this ) )
	{
	  wxArrayString list;
	  list = GetSelected();
	  switch ( m_ActivityType )
	  {
	  case MUSIK_LBTYPE_ARTISTS:
		  g_Playlist = g_Library.GetArtistSongs( &list );
		  break;
	  case MUSIK_LBTYPE_ALBUMS:
		  g_Playlist = g_Library.GetAlbumSongs( &list );
		  break;
	  case MUSIK_LBTYPE_GENRES:
		  g_Playlist = g_Library.GetGenreSongs( &list );
		  break;
	  case MUSIK_LBTYPE_YEARS:
		  g_Playlist = g_Library.GetYearSongs( &list );
		  break;
	  }
	}

	//---------------------------------------------------------//
	//--- if we hide unselected entires. this routine		---//
	//--- helps assure that, for example, if two artists	---//
	//--- share a common album name (such as "unplugged").	---//
	//--- only the correct, selected artist's album songs	---//
	//--- get displayed										---//
	//---------------------------------------------------------//
	else if ( g_Prefs.nSelStyle == 0 || g_Prefs.nSelStyle == 1 )
	{
		CActivityBox *pParentBox = g_ActivityAreaCtrl->GetParentBox();
		if ( pParentBox != NULL )
		{
			int nParentType				= pParentBox->GetActivityType();
			int nThisType				= GetActivityType();
			wxString sParentType		= wxT( "" );
			wxString sThisType			= wxT( "" );
			wxArrayString aParentSel	= pParentBox->GetSelected();
			wxArrayString aThisSel		= GetSelected();

			if ( nParentType == ACTIVITY_ARTISTS )
				sParentType = wxT( "artist" );
			else if ( nParentType == ACTIVITY_ALBUMS )
				sParentType = wxT( "album" );
			else if ( nParentType == ACTIVITY_GENRES )
				sParentType = wxT( "genre" );
			else if ( nParentType == ACTIVITY_YEARS )
				sParentType = wxT( "year" );

			if ( nThisType == ACTIVITY_ARTISTS )
				sThisType = wxT( "artist" );
			else if ( nThisType == ACTIVITY_ALBUMS )
				sThisType = wxT( "album" );
			else if ( nThisType == ACTIVITY_GENRES )
				sThisType = wxT( "genre" );
			else if ( nThisType == ACTIVITY_YEARS )
				sThisType = wxT( "year" );

			if ( sThisType == wxT( "" ) || sParentType == wxT( "" ) )
				return;

			//--- make this box'es portion of query ---//
			wxString sThis; 
			sThis.Alloc(aThisSel.GetCount() * 40); // optimization ( the 40 is a wild guess)
			sThis += sThisType + wxT(" like ");
			for ( size_t i = 0; i < aThisSel.GetCount(); i++ )
			{
				aThisSel.Item( i ).Replace( wxT( "'" ), wxT( "''" ), true );
				sThis += wxT("'");
				sThis += aThisSel.Item( i );

				//--- last item, so just add apostrophe ' ---//
				if ( i == ( aThisSel.GetCount() - 1 ) )
				    sThis + wxT("'");

				//--- not last item, so format string for another ---//
				else
				    sThis += wxT("' or ") + sThisType + wxT( " like " );
			}

			//--- make parent portion of query ---//
			wxString sParent;
			sParent.Alloc(aParentSel.GetCount() * (50+ sThis.Length()+sParentType.Length())); // optimization ( the 40 is a wild guess)
			sParent = sParentType + wxT(" like "); 
			for ( size_t i = 0; i < aParentSel.GetCount(); i++ )
			{
				aParentSel.Item( i ).Replace( wxT( "'" ), wxT( "''" ), true );
				sParent += wxT("'");
				sParent += aParentSel.Item( i );
				sParent += wxT("' and " );
				sParent += sThis;

				//--- last item, so just add apostrophe ' ---//
				if ( i == ( aParentSel.GetCount() - 1 ) )
					sParent += wxT( "'" );

				//--- not last item, so format string for another ---//
				else
					sParent += wxT("' or ") + sParentType + wxT( " like " );

			}

			//--- compile query ---//
			g_Playlist = g_Library.QuerySongs( sParent );
		}
	}
	
	g_PlaylistCtrl->Update();
	g_PlaylistChanged = true;
}

void CActivityBox::SetContents( wxArrayString *pList )
{
	if ( g_Prefs.nSelStyle == 0 || g_Prefs.nSelStyle == 1 )
		pList->Insert( _("Show all ") + GetActivityTypeStr() + wxT( "s" ), 0 );

	pListBox->SetList( pList );
	pListBox->Update( true );
}

//------------------------//
//--- tag info editing ---//
//------------------------//
void CActivityBox::EditBegin()
{
	//-- disallow further renaming until the current job is finished --//
	if ( g_MusikFrame->GetActiveThread() )
		return;

	wxString sVal = GetFirstSel();
	pSizer->Show( pEdit, TRUE );
	pEdit->Enable( TRUE );
	Layout();
	pEdit->SetValue( sVal );
	pEdit->SetFocus( );
	pEdit->SetSelection( -1, -1 );
	pListBox->Enable( false );
	m_EditVisible = true;
}

void CActivityBox::EditCancel()
{
	EditDone();
}

void CActivityBox::EditCommit()
{
	//-- disallow further renaming until the current job is finished --//
	if ( g_MusikFrame->GetActiveThread() )
		return;

	pEdit->Enable( FALSE );

	StartRenameThread( ACTIVITY_RENAME_ACTIVITY, GetSelected(), pEdit->GetValue() );
}

void CActivityBox::EnableProgress( bool enable )
{
	pSizer->Show( pEdit, !enable );
	pListBox->Enable( !enable );
	if ( !enable )
		EditDone();
	Layout();
}

void CActivityBox::EditDone()
{
	pSizer->Show( pEdit, FALSE );
	pListBox->Enable( true );
	Layout();
	m_EditVisible = false;
}

//-------------------//
//--- drag 'n drop---//
//-------------------//
void CActivityBox::DNDBegin()
{
	g_DragInProg = true;

	wxString sDrop = wxString( wxT("a\n") ) + DNDGetList();

	//-------------------------------------------------------//
	//--- initialize drag and drop                        ---//
	//--- Playlist / Sources should take care of the rest ---//
	//-------------------------------------------------------//
	wxDropSource dragSource( this );
	wxTextDataObject activity_data( sDrop );
	dragSource.SetData( activity_data );
	dragSource.DoDragDrop( TRUE );

	g_DragInProg = false;
}

wxString CActivityBox::DNDGetList()
{
	//--- get selected items ---//
	wxArrayString list;
	CMusikSongArray songs;
	list = GetSelected();
	switch ( m_ActivityType )
	{
	case MUSIK_LBTYPE_ARTISTS:
		songs = g_Library.GetArtistSongs( &list );
		break;
	case MUSIK_LBTYPE_ALBUMS:
		songs = g_Library.GetAlbumSongs( &list );
		break;
	case MUSIK_LBTYPE_GENRES:
		songs = g_Library.GetGenreSongs( &list );
		break;
	case MUSIK_LBTYPE_YEARS:
		songs = g_Library.GetYearSongs( &list );
		break;
	}
	list.Clear();

	//--- add songs to dnd string ---//
	wxString sRet = wxT("");
	for ( size_t i = 0; i < songs.GetCount(); i++ )
	{
		if	( i == ( songs.GetCount() - 1 ) )
			sRet += songs.Item( i ).Filename;	
		else
			sRet += songs.Item( i ).Filename + wxT("\n");
	}
	return sRet;
}

//-----------------------------//
//--- rename thread control ---//
//-----------------------------//
void CActivityBox::StartRenameThread( int mode, wxArrayString sel, wxString newvalue )
{
	if ( g_MusikFrame->GetActiveThread() == NULL )
        {
			pRenameThread = new MusikActivityRenameThread( this, mode, sel, newvalue );
			pRenameThread->Create();
			pRenameThread->Run();
        }
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the Musik development team with this error." ), MUSIK_VERSION, wxICON_STOP );
}


//----------------------------------//
//--- activity box rename thread ---//
//----------------------------------//
void CActivityBox::OnRenameThreadStart( wxCommandEvent& WXUNUSED(event) )
{
	//--- update locally ---//
	SetActiveThread	( pRenameThread );
	SetProgressType	( MUSIK_ACTIVITY_RENAME_THREAD );
	SetProgress		( 0 );

	//--- setup thread to begin in g_MusikFrame ---//
	g_MusikFrame->SetActiveThread	( pRenameThread );
	g_MusikFrame->SetProgressType	( MUSIK_ACTIVITY_RENAME_THREAD );
	g_MusikFrame->SetProgress		( 0 );

	//--- post the event. we're up and running now! ---//
	wxCommandEvent MusikStartProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_START );
	wxPostEvent( g_MusikFrame, MusikStartProgEvt );
}
	
void CActivityBox::OnRenameThreadProg( wxCommandEvent& WXUNUSED(event) )
{
	//--- relay thread progress message to g_MusikFrame ---//
	g_MusikFrame->SetProgress( GetProgress() );
	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_PROG );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}

void CActivityBox::OnRenameThreadEnd( wxCommandEvent& WXUNUSED(event) )
{
	if ( g_Prefs.nSelStyle == 2 || g_ActivityAreaCtrl->GetParentBox() == this )
		ResetContents();
	else
	{
		if ( g_ActivityAreaCtrl->GetParentBox() != NULL )
			g_ActivityAreaCtrl->UpdateSel( g_ActivityAreaCtrl->GetParentBox() );
	}

	EnableProgress( false );
	g_PlaylistCtrl->Update();

	//--- update locally ---//
	SetActiveThread	( NULL );
	SetProgressType	( 0 );
	SetProgress		( 0 );

    //--- relay thread end message to g_MusikFrame ---//
	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_END );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}
