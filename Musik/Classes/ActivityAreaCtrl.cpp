/*
 *  ActivityAreaCtrl.h
 *
 *  The "Activity Area" control
 *	This control puts all the activity boxes next to each other
 *	and manages creation / deletion of them, as well as events.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "ActivityAreaCtrl.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

BEGIN_EVENT_TABLE(CActivityAreaCtrl, wxPanel)
	EVT_LIST_ITEM_FOCUSED		(MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBox1Focused	)
	EVT_LIST_ITEM_FOCUSED		(MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBox2Focused	)
	EVT_LIST_ITEM_FOCUSED		(MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBox3Focused	)
	EVT_LIST_ITEM_FOCUSED		(MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBox4Focused	)
	EVT_LIST_ITEM_SELECTED		(MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBox1SelChange	)
	EVT_LIST_ITEM_SELECTED		(MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBox2SelChange	)
	EVT_LIST_ITEM_SELECTED		(MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBox3SelChange	)
	EVT_LIST_ITEM_SELECTED		(MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBox4SelChange	)
	EVT_LIST_ITEM_ACTIVATED		(MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBox1Activated	)
	EVT_LIST_ITEM_ACTIVATED		(MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBox2Activated	)
	EVT_LIST_ITEM_ACTIVATED		(MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBox3Activated	)
	EVT_LIST_ITEM_ACTIVATED		(MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBox4Activated	)
	EVT_LIST_BEGIN_DRAG			(MUSIK_ACTIVITYBOX1, CActivityAreaCtrl::OnActivityBox1SelDrag	)
	EVT_LIST_BEGIN_DRAG			(MUSIK_ACTIVITYBOX2, CActivityAreaCtrl::OnActivityBox2SelDrag	)
	EVT_LIST_BEGIN_DRAG			(MUSIK_ACTIVITYBOX3, CActivityAreaCtrl::OnActivityBox3SelDrag	)
	EVT_LIST_BEGIN_DRAG			(MUSIK_ACTIVITYBOX4, CActivityAreaCtrl::OnActivityBox4SelDrag	)
END_EVENT_TABLE()

CActivityAreaCtrl::CActivityAreaCtrl( wxWindow *pParent )
	: wxPanel( pParent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxCLIP_CHILDREN )
{
	m_ActivityBox1	= NULL;
	m_ActivityBox2	= NULL;
	m_ActivityBox3	= NULL;
	m_ActivityBox4	= NULL;

	SetSelecting( false );
	
	pTopSizer = new wxBoxSizer( wxHORIZONTAL );
	SetSizerAndFit( pTopSizer );

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
			m_ActivityBox1 = new CActivityBox( this, &g_Items1, MUSIK_ACTIVITYBOX1, g_Prefs.nActBox1 );
			m_ActivityBox1->ResetCaption();

			pTopSizer->Add( m_ActivityBox1, 1, wxEXPAND | wxALL, 1 );
		}

		//--- box2 ---//
		if ( g_Prefs.nActBox2 > 0 && m_ActivityBox2 == NULL )
		{
			m_ActivityBox2 = new CActivityBox( this, &g_Items2, MUSIK_ACTIVITYBOX2, g_Prefs.nActBox2 );
			m_ActivityBox2->ResetCaption();

			pTopSizer->Add( m_ActivityBox2, 1, wxEXPAND | wxALL, 1 );
		}

		//--- box3 ---//
		if ( g_Prefs.nActBox3 > 0 && m_ActivityBox3 == NULL )
		{
			m_ActivityBox3 = new CActivityBox( this, &g_Items3, MUSIK_ACTIVITYBOX3, g_Prefs.nActBox3 );
			m_ActivityBox3->ResetCaption();

			pTopSizer->Add( m_ActivityBox3, 1, wxEXPAND | wxALL, 1 );
		}

		//--- box4 ---//
		if ( g_Prefs.nActBox4 > 0 && m_ActivityBox4 == NULL )
		{
			m_ActivityBox4 = new CActivityBox( this, &g_Items4, MUSIK_ACTIVITYBOX4, g_Prefs.nActBox4 );
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

void CActivityAreaCtrl::ResetAllContents()
{
	if ( m_ActivityBox1 != NULL )	m_ActivityBox1->ResetContents();
	if ( m_ActivityBox2 != NULL )	m_ActivityBox2->ResetContents();
	if ( m_ActivityBox3 != NULL )	m_ActivityBox3->ResetContents();
	if ( m_ActivityBox4 != NULL )	m_ActivityBox4->ResetContents();
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

	SetSelecting( true );

	//-------------------------------------//
	//--- which box are we?             ---//
	//--- and which are the other ones? ---//
	//-------------------------------------//
	CActivityBox *pBox1, *pBox2, *pBox3;
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
	if ( g_Prefs.nShowUnsel == 0 )
	{
		if ( GetParentId() == 0 )
			SetParent( pSel->GetListId(), false );
	}
 
	//---------------------------------------------------//
	//--- if we're hiding unselected entries          ---//
	//--- and reset is clicked or nothing is selected ---//
	//--- reset all the boxes						  ---//
	//---------------------------------------------------//
	if ( g_Prefs.nShowUnsel == 0 && ( pSel->IsSelected( 0 ) || pSel->GetSelectedItemCount() < 1 ) )
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
				g_Playlist = g_Library.GetAllSongs();
				g_PlaylistCtrl->Update();
			}
			SetSelecting( false );
			return;
		}
	}

	//-----------------------------------------------//
	//--- if we're hiding unselected entries      ---//
	//--- and a valid item is clicked, update the ---//
	//--- other controls with the right values    ---//
	//-----------------------------------------------//
	else if ( g_Prefs.nShowUnsel == 0 && ( !pSel->IsSelected( 0 ) && pSel->GetSelectedItemCount() > 0 ) )
	{
		wxArrayString temp_list;
		if ( /*GetParentId() == pSel->GetListId()*/ 1 == 1 )
		{
			if ( pBox1 != NULL )
			{
				temp_list = pSel->GetRelatedList( pBox1 );
				pBox1->SetContents( &temp_list );
			}
			if ( pBox2 != NULL )
			{			
				temp_list = pSel->GetRelatedList( pBox2 );
				pBox2->SetContents( &temp_list );
			}
			if ( pBox3 != NULL )
			{
				temp_list = pSel->GetRelatedList( pBox3 );
				pBox3->SetContents( &temp_list );
			}
		}
	}      

	//-------------------------------------------//
	//--- if we're showing unselected entries ---//
	//--- and no items are selected, unselect ---//
	//--- all the corresponding items		  ---//
	//-------------------------------------------//
	else if ( g_Prefs.nShowUnsel == 1 && pSel->GetSelectedItemCount() < 1 )
	{
		pSel->DeselectAll();
		if ( pBox1 != NULL )	pBox1->DeselectAll();
		if ( pBox2 != NULL )	pBox2->DeselectAll();
		if ( pBox3 != NULL )	pBox3->DeselectAll();

		if ( g_Prefs.nShowAllSongs == 1 )
		{
			if ( !g_FirstRun )
			{
				g_Playlist = g_Library.GetAllSongs();
				g_PlaylistCtrl->Update();
			}
			SetSelecting( false );
			return;
		}		
	}

	//-------------------------------------------//
	//--- if we're showing unselected entries ---//
	//--- and 1+ items are selected, select   ---//
	//--- all the corresponding items		  ---//
	//-------------------------------------------//
	else if ( g_Prefs.nShowUnsel == 1 && pSel->GetSelectedItemCount() > 0 )
	{
		wxArrayString rel;
		wxArrayString all;
		wxListItem item;
		int n;

		//--- box1 ---//
		if ( pBox1 != NULL )
		{
			rel = pSel->GetRelatedList( pBox1 );
			all = pBox1->GetFullList();

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
				pBox1->SetContents( &all );
				pSel->SetRelated( -1 );
				pSel->Update( false );
			}
		}

		//--- box2 ---//
		if ( pBox2 != NULL )
		{
			rel = pSel->GetRelatedList( pBox2 );
			all = pBox2->GetFullList();

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
				pBox2->SetContents( &all );
				pSel->SetRelated( -1 );
				pSel->Update( false );
			}
		}

		//--- box3 ---//
		if ( pBox3 != NULL )
		{
			rel = pSel->GetRelatedList( pBox3 );
			all = pBox2->GetFullList();

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
				pBox3->SetContents( &all );
				pSel->SetRelated( -1 );
				pSel->Update( false );
			}
		}
	}
	
	pSel->SetPlaylist();
	SetSelecting( false );
}



//-------------------------------------------------------------------------------------------------//
//---											events											---//
//-------------------------------------------------------------------------------------------------//
//---							*IMPORTANT* *IMPORTANT* *IMPORTANT*								---//
//--- when using a virtual wxListCtrl, there are a few selection problems. specifically,		---//
//--- if the user holds shift to select, the selection will never get updated if the			---//
//--- event EVT_LIST_ITEM_SELECTED is used. so, instead we use EVT_LIST_ITEM_FOCUSED...			---//
//--- only this will not get called if items are DESLECTED. in windows, EVT_LIST_ITEM_FOCUSED	---//
//--- gets called, first, then EVT_LIST_ITEM_SELECTED. HOWEVER... in linux its the				---//
//--- opposite. EVT_LIST_ITEM_SELECTED then EVT_LIST_ITEM_FOCUSED. we have to assure only one	---//
//--- of these is called, becuase if they are both called, the selection event will be called	---//
//--- twice. no good. thats why there are two events that do the same thing. thats why there	---//
//--- is an #ifdef																				---//
//-------------------------------------------------------------------------------------------------//

//----------------------------//
//--- BEGIN WIN32 SPECIFIC ---//
//----------------------------//
#if defined __WXMSW__
void CActivityAreaCtrl::OnActivityBox1Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox1 );
		SetSelecting( false );
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox2Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox2 );
		SetSelecting( false );	
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox3Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox3 ); 
		SetSelecting( false );	
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox4Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox4 );
		SetSelecting( false );
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox1SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox1 );
		SetSelecting( false );
	} 
	g_FocusDone = false;
}

void CActivityAreaCtrl::OnActivityBox2SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox2 );
		SetSelecting( false );
	} 
	g_FocusDone = false;
}

void CActivityAreaCtrl::OnActivityBox3SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox3 );
		SetSelecting( false );
	} 
	g_FocusDone = false;
}

void CActivityAreaCtrl::OnActivityBox4SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox4 );
		SetSelecting( false );
	} 
	g_FocusDone = false;
}
//----------------------------//
//--- END WIN32 SPECIFIC   ---//
//----------------------------//
//--- BEGIN GTK2 SPECIFIC  ---//
//----------------------------//
#elif defined __WXGTK__
void CActivityAreaCtrl::OnActivityBox1SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox1 );
		SetSelecting( false );
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox2SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox2 );
		SetSelecting( false );
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox3SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox3 );
		SetSelecting( false );
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox4SelChange( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() )
	{
		UpdateSel( m_ActivityBox4 );
		SetSelecting( false );	
		g_FocusDone = true;
	}
}

void CActivityAreaCtrl::OnActivityBox1Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox1 );
		SetSelecting( false );	
	} 
	g_FocusDone = false;
}

void CActivityAreaCtrl::OnActivityBox2Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox2 );
		SetSelecting( false );	
	} 
	g_FocusDone = false;
}

void CActivityAreaCtrl::OnActivityBox3Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox3 );
		SetSelecting( false );	
	} 
	g_FocusDone = false;
}

void CActivityAreaCtrl::OnActivityBox4Focused( wxListEvent& WXUNUSED(event) )
{
	if ( !IsSelecting() && !g_FocusDone )
	{
		UpdateSel( m_ActivityBox4 );
		SetSelecting( false );	
	} 
	g_FocusDone = false;
}
#endif
//--------------------------//
//--- END GTK2 SPECIFIC  ---//
//--------------------------//
//--- drag and drop event ---//
//---------------------------//
void CActivityAreaCtrl::OnActivityBox1SelDrag( wxListEvent& WXUNUSED(event) )
{	
	if ( !IsSelecting() )	
		m_ActivityBox1->DNDBegin();		
}

void CActivityAreaCtrl::OnActivityBox2SelDrag( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() )	
		m_ActivityBox2->DNDBegin();		
}

void CActivityAreaCtrl::OnActivityBox3SelDrag( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() )	
		m_ActivityBox3->DNDBegin();		
}

void CActivityAreaCtrl::OnActivityBox4SelDrag( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() )
		m_ActivityBox4->DNDBegin();		
}

//----------------------------//
//--- item activated event ---//
//----------------------------//
void CActivityAreaCtrl::OnActivityBox1Activated	( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() )
		SetParent( MUSIK_ACTIVITYBOX1, true );
}

void CActivityAreaCtrl::OnActivityBox2Activated	( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() ) 
		SetParent( MUSIK_ACTIVITYBOX2, true );	
}

void CActivityAreaCtrl::OnActivityBox3Activated	( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() ) 
		SetParent( MUSIK_ACTIVITYBOX3, true );	
}

void CActivityAreaCtrl::OnActivityBox4Activated	( wxListEvent& WXUNUSED(event) )	
{	
	if ( !IsSelecting() ) 
		SetParent( MUSIK_ACTIVITYBOX4, true );	
}
