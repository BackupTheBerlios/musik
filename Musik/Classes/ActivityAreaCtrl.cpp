/*
 *  ActivityAreaCtrl.h
 *
 *  The "Activity Area" control
 *	This control puts all the activity boxes next to each other
 *	and manages creation / deletion of them, as well as events.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "ActivityAreaCtrl.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

BEGIN_EVENT_TABLE(CActivityAreaCtrl, wxPanel)
	EVT_LIST_ITEM_FOCUSED		( MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBoxFocused	)
	EVT_LIST_ITEM_FOCUSED		( MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBoxFocused	)
	EVT_LIST_ITEM_FOCUSED		( MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBoxFocused	)
	EVT_LIST_ITEM_FOCUSED		( MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBoxFocused	)
	EVT_LIST_ITEM_SELECTED		( MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBoxSelected	)
	EVT_LIST_ITEM_SELECTED		( MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBoxSelected	)
	EVT_LIST_ITEM_SELECTED		( MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBoxSelected	)
	EVT_LIST_ITEM_SELECTED		( MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBoxSelected	)
	EVT_LIST_ITEM_DESELECTED	( MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBoxSelected	) // just use the same method for deselection
	EVT_LIST_ITEM_DESELECTED	( MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBoxSelected	)
	EVT_LIST_ITEM_DESELECTED	( MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBoxSelected	)
	EVT_LIST_ITEM_DESELECTED	( MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBoxSelected	)

	EVT_LIST_ITEM_ACTIVATED		( MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBoxActivated	)
	EVT_LIST_ITEM_ACTIVATED		( MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBoxActivated	)
	EVT_LIST_ITEM_ACTIVATED		( MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBoxActivated	)
	EVT_LIST_ITEM_ACTIVATED		( MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBoxActivated	)
	EVT_LIST_BEGIN_DRAG			( MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBox1SelDrag	)
	EVT_LIST_BEGIN_DRAG			( MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBox2SelDrag	)
	EVT_LIST_BEGIN_DRAG			( MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBox3SelDrag	)
	EVT_LIST_BEGIN_DRAG			( MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBox4SelDrag	)
END_EVENT_TABLE()

CActivityAreaCtrl::CActivityAreaCtrl( wxWindow *pParent )
	: wxPanel( pParent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxCLIP_CHILDREN )
{
	m_ActivityBox1	= NULL;
	m_ActivityBox2	= NULL;
	m_ActivityBox3	= NULL;
	m_ActivityBox4	= NULL;

	
	pTopSizer = new wxBoxSizer( wxHORIZONTAL );
	SetSizerAndFit( pTopSizer );
	m_Selected = m_bFocused = m_Selecting = false;
	m_UpdatePlaylist = true;
	Create();
}

CActivityAreaCtrl::~CActivityAreaCtrl()
{

}

bool CActivityAreaCtrl::Create()
{
	if ( m_ActivityBox1 == NULL && m_ActivityBox2 == NULL && m_ActivityBox3 == NULL && m_ActivityBox4 == NULL )
	{
		//--- box1 ---//
		if ( g_Prefs.nActBox1 > 0 && m_ActivityBox1 == NULL )
		{
			m_ActivityBox1 = new CActivityBox( this, MUSIK_ACTIVITYBOX1, g_Prefs.nActBox1 );
			m_ActivityBox1->ResetCaption();

			pTopSizer->Add( m_ActivityBox1, 1, wxEXPAND | wxALL, 1 );
		}

		//--- box2 ---//
		if ( g_Prefs.nActBox2 > 0 && m_ActivityBox2 == NULL )
		{
			m_ActivityBox2 = new CActivityBox( this, MUSIK_ACTIVITYBOX2, g_Prefs.nActBox2 );
			m_ActivityBox2->ResetCaption();

			pTopSizer->Add( m_ActivityBox2, 1, wxEXPAND | wxALL, 1 );
		}

		//--- box3 ---//
		if ( g_Prefs.nActBox3 > 0 && m_ActivityBox3 == NULL )
		{
			m_ActivityBox3 = new CActivityBox( this, MUSIK_ACTIVITYBOX3, g_Prefs.nActBox3 );
			m_ActivityBox3->ResetCaption();

			pTopSizer->Add( m_ActivityBox3, 1, wxEXPAND | wxALL, 1 );
		}

		//--- box4 ---//
		if ( g_Prefs.nActBox4 > 0 && m_ActivityBox4 == NULL )
		{
			m_ActivityBox4 = new CActivityBox( this, MUSIK_ACTIVITYBOX4, g_Prefs.nActBox4 );
			m_ActivityBox4->ResetCaption();

			pTopSizer->Add( m_ActivityBox4, 1, wxEXPAND | wxALL, 1 );
		}

		Layout();
		SetSize( pTopSizer->GetMinSize() );

		SetParent( 0, false );

		return true;
	}

	return false;
}

void CActivityAreaCtrl::Delete()
{
	if ( m_ActivityBox1 != NULL )
	{
		pTopSizer->Remove( m_ActivityBox1 );
		delete m_ActivityBox1;
		m_ActivityBox1 = NULL;
	}
	if ( m_ActivityBox2 != NULL )
	{
		pTopSizer->Remove( m_ActivityBox2 );
		delete m_ActivityBox2;
		m_ActivityBox2 = NULL;
	}
	if ( m_ActivityBox3 != NULL )
	{
		pTopSizer->Remove( m_ActivityBox3 );
		delete m_ActivityBox3;
		m_ActivityBox3 = NULL;
	}
	if ( m_ActivityBox4 != NULL )
	{
		pTopSizer->Remove( m_ActivityBox4 );
		delete m_ActivityBox4;
		m_ActivityBox4 = NULL;
	}
}

void CActivityAreaCtrl::ResetAllContents( bool bUpdatePlaylist )
{
	m_UpdatePlaylist = bUpdatePlaylist;

	if ( m_ActivityBox1 != NULL )	m_ActivityBox1->ResetContents();
	if ( m_ActivityBox2 != NULL )	m_ActivityBox2->ResetContents();
	if ( m_ActivityBox3 != NULL )	m_ActivityBox3->ResetContents();
	if ( m_ActivityBox4 != NULL )	m_ActivityBox4->ResetContents();

	m_UpdatePlaylist = true;
}

void CActivityAreaCtrl::RescaleColumns()
{
	if ( m_ActivityBox1 != NULL )
		m_ActivityBox1->RescaleColumns();
	if ( m_ActivityBox2 != NULL )
		m_ActivityBox2->RescaleColumns();
	if ( m_ActivityBox3 != NULL )
		m_ActivityBox3->RescaleColumns();
	if ( m_ActivityBox4 != NULL )
		m_ActivityBox4->RescaleColumns();
}

void CActivityAreaCtrl::SetParent( int nID, bool bUpdate )
{
	m_ParentId = nID;
	if ( nID == 0 )
{
		m_ParentBox = NULL;
		return;
	}
	else if ( nID == MUSIK_ACTIVITYBOX1 )
	{
		m_ParentBox = m_ActivityBox1;
		if ( m_ActivityBox1 != NULL && bUpdate )
			UpdateSel( m_ActivityBox1 );
		return;
	}
	else if ( nID == MUSIK_ACTIVITYBOX2 )
	{
		m_ParentBox = m_ActivityBox2;
		if ( m_ActivityBox2 != NULL && bUpdate )
			UpdateSel( m_ActivityBox2 );
		return;
	}
	else if ( nID == MUSIK_ACTIVITYBOX3 )
	{
		m_ParentBox = m_ActivityBox3;
		if ( m_ActivityBox3 != NULL && bUpdate  )
			UpdateSel( m_ActivityBox3 );
		return;
	}
	else if ( nID == MUSIK_ACTIVITYBOX4 )
	{
		m_ParentBox = m_ActivityBox4;
		if ( m_ActivityBox4 != NULL && bUpdate )
			UpdateSel( m_ActivityBox4 );
		return;
	}
}

void CActivityAreaCtrl::UpdateSel( CActivityBox *pSel )
{
	//-- avoid updating playlists when dragging over activity area --//
	if ( g_DragInProg )
		return;

	if ( !pSel )
		return;

	//-------------------------------------//
	//--- which box are we?             ---//
	//--- and which are the other ones? ---//
	//-------------------------------------//
	CActivityBox *pBox1 = NULL, *pBox2 = NULL , *pBox3 = NULL;
	if ( pSel == m_ActivityBox1 )
	{	pBox1 = m_ActivityBox2;		pBox2 = m_ActivityBox3;		pBox3 = m_ActivityBox4;		}
	else if ( pSel == m_ActivityBox2 )
	{	pBox1 = m_ActivityBox1;		pBox2 = m_ActivityBox3;		pBox3 = m_ActivityBox4;		}
	else if ( pSel == m_ActivityBox3 )
	{	pBox1 = m_ActivityBox1;		pBox2 = m_ActivityBox2;		pBox3 = m_ActivityBox4;		}
	else if ( pSel == m_ActivityBox4 )
	{	pBox1 = m_ActivityBox1;		pBox2 = m_ActivityBox2;		pBox3 = m_ActivityBox3;		}

	//-------------------------------------------------//
	//--- if we're showing unsel, find which is the	---//
	//--- parent and which are children, if there	---//
	//--- is no parent already.						---//
	//-------------------------------------------------//
	if ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_STANDARD )
	{
		if ( GetParentId() == 0 )
			SetParent( pSel->GetListId(), false );
	}
 
	//---------------------------------------------------//
	//--- if we're hiding unselected entries          ---//
	//--- and reset is clicked or nothing is selected ---//
	//--- reset all the boxes						  ---//
	//---------------------------------------------------//
	if ( ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_STANDARD || g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_SLOPPY ) && ( pSel->IsSelected( 0 ) || pSel->GetSelectedItemCount() < 1 ) )
	{
		SetParent( 0, false );
		pSel->ResetContents();	

		if ( pBox1 != NULL )	pBox1->ResetContents();	
		if ( pBox2 != NULL )	pBox2->ResetContents();
		if ( pBox3 != NULL )	pBox3->ResetContents();

		if ( g_Prefs.nShowAllSongs == 1 )
		{
			if ( !g_FirstRun )
			{
				g_Library.GetAllSongs( g_Playlist );
				g_PlaylistCtrl->Update();
			}
			return;
		}
	}

	//-----------------------------------------------//
	//--- if we're hiding unselected entries      ---//
	//--- and a valid item is clicked, update the ---//
	//--- other controls with the right values    ---//
	//-----------------------------------------------//
	else if ( ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_STANDARD || g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_SLOPPY ) && ( !pSel->IsSelected( 0 ) && pSel->GetSelectedItemCount() > 0 ) )
	{
		wxArrayString temp_list;
		if ( ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_STANDARD && GetParentId() == pSel->GetListId() ) || g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_SLOPPY )
		{
			if ( pBox1 != NULL )
			{
				pSel->GetRelatedList( pBox1, temp_list );
				pBox1->SetContents( temp_list );
			}
			if ( pBox2 != NULL )
			{			
				pSel->GetRelatedList( pBox2, temp_list );
				pBox2->SetContents( temp_list );
			}
			if ( pBox3 != NULL )
			{
				pSel->GetRelatedList( pBox3, temp_list );
				pBox3->SetContents( temp_list );
			}
		}
	}      

	//-------------------------------------------//
	//--- if we're showing unselected entries ---//
	//--- and no items are selected, unselect ---//
	//--- all the corresponding items		  ---//
	//-------------------------------------------//
	else if ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_HIGHLIGHT && pSel->GetSelectedItemCount() < 1 )
	{
		pSel->DeselectAll();
		if ( pBox1 != NULL )	pBox1->DeselectAll();
		if ( pBox2 != NULL )	pBox2->DeselectAll();
		if ( pBox3 != NULL )	pBox3->DeselectAll();

		if ( g_Prefs.nShowAllSongs == 1 )
		{
			if ( !g_FirstRun )
			{
				g_Library.GetAllSongs( g_Playlist );
				g_PlaylistCtrl->Update();
			}
			return;
		}		
	}

	//-------------------------------------------//
	//--- if we're showing unselected entries ---//
	//--- and 1+ items are selected, select   ---//
	//--- all the corresponding items		  ---//
	//-------------------------------------------//
	else if ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_HIGHLIGHT && pSel->GetSelectedItemCount() > 0 )
	{
		wxArrayString rel;
		wxArrayString all;
		wxListItem item;
		int n;

		//--- box1 ---//
		if ( pBox1 != NULL )
		{
			pSel->GetRelatedList( pBox1, rel );
			pBox1->GetFullList( all );

			if ( all.GetCount() > 0 )
			{
				for ( size_t i = 0; i < rel.GetCount(); i++ )
				{
					n = FindStrInArray( &all, rel.Item( i ) );
					if ( n  > -1 )
					{
						all.RemoveAt( n );
						all.Insert( rel.Item( i ), i );
					}
				}
				pBox1->SetRelated( rel.GetCount() );
				pBox1->SetContents( all );
				pSel->SetRelated( -1 );
				pSel->Update( false );
			}
		}

		//--- box2 ---//
		if ( pBox2 != NULL )
		{
			pSel->GetRelatedList( pBox2, rel );
			pBox2->GetFullList( all );

			if ( all.GetCount() > 0 )
			{
				for ( size_t i = 0; i < rel.GetCount(); i++ )
				{
					n = FindStrInArray( &all, rel.Item( i ) );
					if ( n  > -1 )
					{
						all.RemoveAt( n );
						all.Insert( rel.Item( i ), i );
					}
				}
				pBox2->SetRelated( rel.GetCount() );
				pBox2->SetContents( all );
				pSel->SetRelated( -1 );
				pSel->Update( false );
			}
		}

		//--- box3 ---//
		if ( pBox3 != NULL )
		{
			pSel->GetRelatedList( pBox3, rel );
			pBox2->GetFullList( all );

			if ( all.GetCount() > 0 )
			{
				for ( size_t i = 0; i < rel.GetCount(); i++ )
				{
					n = FindStrInArray( &all, rel.Item( i ) );
					if ( n  > -1 )
					{
						all.RemoveAt( n );
						all.Insert( rel.Item( i ), i );
					}
				}
				pBox3->SetRelated( rel.GetCount() );
				pBox3->SetContents( all );
				pSel->SetRelated( -1 );
				pSel->Update( false );
			}
		}
	}

	if ( ( g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_STANDARD || g_Prefs.eSelStyle == MUSIK_SELECTION_TYPE_SLOPPY ) && ( pSel->IsSelected( 0 ) || pSel->GetSelectedItemCount() < 1 ) )
	{
		g_Playlist.Clear();
		g_PlaylistCtrl->Update();
	}
	else
		pSel->SetPlaylist();
}



//-------------------------------------------------------------------------------------------------//
//---											events											---//
//-------------------------------------------------------------------------------------------------//
//---							*IMPORTANT* *IMPORTANT* *IMPORTANT*								---//
//--- when using a virtual wxListCtrl, there are a few selection problems. specifically,		---//
//--- if the user holds shift to select, the selection will never get updated if the			---//
//--- event EVT_LIST_ITEM_SELECTED is used. so, instead we use EVT_LIST_ITEM_FOCUSED...			---//
//--- EVT_LIST_ITEM_SELECTED is not fired on virtual list box if multi selection is done		---//
//----by the user.																				---//			
//--- On Windows EVT_LIST_ITEM_FOCUSED is not fired if the item already has the focus.			---//
//--- So in windows, we cannot use  EVT_LIST_ITEM_FOCUSED alone.								---//
//--- Order of  EVT_LIST_ITEM_SELECTED and EVT_LIST_ITEM_FOCUSED is different on linux and win. ---//
//--- we have to assure only one is execute, because if they are both called, the selection		---//
//--- event will be called twice. no good. thats why there are three events that do the same	---//
//--- thing.																					---//
//--- to save the #ifdef code on linux and win is the same, would not be						---//
//--- neccessary but doesnt harm.																---//
//-------------------------------------------------------------------------------------------------//

void CActivityAreaCtrl::OnActivityBoxFocused( wxListEvent& event )
{
	if( m_Selected )
	{
		m_Selected = false;
		return;
	}

	wxListEvent ev(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, event.GetId() );
	m_bFocused = true;
	::wxPostEvent( this, ev );
}

void CActivityAreaCtrl::OnActivityBoxSelected( wxListEvent& event )
{
	if( m_bFocused )
	{
		m_bFocused = false;
		return;
	}
	wxListEvent ev(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,event.GetId());
	m_Selected = true;
	::wxPostEvent( this, ev );
}
//--------------------------//
//--- drag and drop event ---//
//---------------------------//
void CActivityAreaCtrl::OnActivityBox1SelDrag( wxListEvent& WXUNUSED(event) )
{	
	m_ActivityBox1->DNDBegin();		
}

void CActivityAreaCtrl::OnActivityBox2SelDrag( wxListEvent& WXUNUSED(event) )	
{	
	m_ActivityBox2->DNDBegin();		
}

void CActivityAreaCtrl::OnActivityBox3SelDrag( wxListEvent& WXUNUSED(event) )	
{	
	m_ActivityBox3->DNDBegin();		
}

void CActivityAreaCtrl::OnActivityBox4SelDrag( wxListEvent& WXUNUSED(event) )	
{	
	m_ActivityBox4->DNDBegin();		
}

//----------------------------//
//--- item activated event ---//
//----------------------------//
void CActivityAreaCtrl::OnActivityBoxActivated	( wxListEvent& event)
{	
	if ( !m_Selecting && m_UpdatePlaylist )
	{
		m_Selecting = true;

		switch( event.GetId() )
		{
		case MUSIK_ACTIVITYBOX1:
			UpdateSel( m_ActivityBox1 );
			break;
		case MUSIK_ACTIVITYBOX2:
			UpdateSel( m_ActivityBox2 );
			break;
		case MUSIK_ACTIVITYBOX3:
			UpdateSel( m_ActivityBox3 );
			break;
		case MUSIK_ACTIVITYBOX4:
			UpdateSel( m_ActivityBox4 );
			break;
		default:
			break;
}

		m_Selected = m_bFocused = m_Selecting = false;
	}
}
