/*
 *  ActivityBoxThreads.cpp
 *
 *  Threads related to ActivityBox.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "ActivityBoxThreads.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#include "../Frames/MusikFrame.h"

MusikActivityRenameThread::MusikActivityRenameThread( CActivityBox* parent_box, int mode, const wxArrayString &sel, wxString newvalue )
{
	m_ParentBox	= parent_box;
	m_Mode		= mode;
	m_Type		= m_ParentBox->GetActivityType();
	m_TypeStr	= m_ParentBox->GetActivityTypeStr();
	m_Selected	= sel;
	m_Replace	= newvalue;
}

void* MusikActivityRenameThread::Entry()
{
	//----------------------------------------//
	//--- events we'll post as we go along ---//
	//----------------------------------------//
	wxCommandEvent RenameStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_ACTIVITY_RENAME_THREAD_START );
	wxCommandEvent RenameProgEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_ACTIVITY_RENAME_THREAD_PROG );	

	wxPostEvent( m_ParentBox, RenameStartEvt );

	if ( m_Mode == ACTIVITY_RENAME_ACTIVITY )
	{
		//---------------------------------------//
		//--- run query to find related songs ---//
		//---------------------------------------//
		wxString sQuery = m_TypeStr + wxT(" like ");
		for ( size_t i = 0; i < m_Selected.GetCount(); i++ )
		{
			m_Selected.Item( i ).Replace( wxT( "'" ), wxT( "''" ), true );
			if ( i == ( m_Selected.GetCount() - 1 ) )
				sQuery += wxT("'") + m_Selected.Item( i ) + wxT("'");
			else
				sQuery += wxT("'") + m_Selected.Item( i ) + wxT("' or ") + m_TypeStr + wxT(" like ");
		}
		g_Library.QuerySongs( sQuery, m_Songs );
	}

	else if ( m_Mode == ACTIVITY_RENAME_SONGS )
	{
		g_PlaylistCtrl->GetSelSongs( m_Songs );
	}

	float fPos = 0;
	int nLastProg = 0;
	int nCurrProg = 0;

	for ( size_t i = 0; i < m_Songs.GetCount(); i++ )
	{
		//-----------------------//
		//--- update progress ---//
		//-----------------------//
		fPos = (float)( i * 100 ) / (float)m_Songs.GetCount();
		nCurrProg = (int)fPos;
		if ( nCurrProg > nLastProg )
		{
			m_ParentBox->SetProgress( nCurrProg );
			wxPostEvent( m_ParentBox, RenameProgEvt );
		}
		nLastProg = nCurrProg;

		if ( TestDestroy() )
			break;

		else
		{
			//-------------------------------//
			//--- update the current item ---//
			//-------------------------------//
			switch( m_Type )
			{
			case MUSIK_LBTYPE_ARTISTS:
				m_Songs.Item( i ).Artist = m_Replace;
				break;
			case MUSIK_LBTYPE_ALBUMS:
				m_Songs.Item( i ).Album	= m_Replace;
				break;
			case MUSIK_LBTYPE_GENRES:
				m_Songs.Item( i ).Genre = m_Replace;
				break;
			case MUSIK_LBTYPE_YEARS:
				m_Songs.Item( i ).Year = m_Replace;
				break;
			}

			//--------------------------//
			//--- write tags to file ---//
			//--------------------------//
			if ( g_Prefs.nActBoxWrite == 1 )
				g_Library.WriteTag( m_Songs.Item( i ), (bool)g_Prefs.nActBoxClear );

			//-------------------//
			//--- rename file ---//
			//-------------------//
			if ( g_Prefs.nActBoxRename == 1 )
				g_Library.RenameFile( &m_Songs.Item( i ) );

			//----------------------------------//
			//--- if not writing, flag dirty ---//
			//----------------------------------//
			g_Library.UpdateItem( m_Songs.Item( i ).Filename, m_Songs.Item( i ), g_Prefs.nActBoxWrite == 0 );

		}
	}

	g_Playlist = m_Songs;
	return NULL;
}

void MusikActivityRenameThread::OnExit()
{
	wxCommandEvent RenameEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_ACTIVITY_RENAME_THREAD_END );
	wxPostEvent( m_ParentBox, RenameEndEvt );
}

