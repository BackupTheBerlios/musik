/*
 *  PlaylistCtrlThreads.cpp
 *
 *  Threads that relates to PlaylistCtrl.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "PlaylistCtrlThreads.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

MusikPlaylistRenameThread::MusikPlaylistRenameThread( CMusikSongArray songs )
{
	m_Songs = songs;
}

void* MusikPlaylistRenameThread::Entry()
{
	g_PlaylistCtrl->SetActiveThread	( this );
	g_PlaylistCtrl->SetProgressType	( MUSIK_PLAYLIST_RENAME_THREAD );
	g_PlaylistCtrl->SetProgress		( 0 );

	//----------------------------------------//
	//--- events we'll post as we go along ---//
	//----------------------------------------//
	wxCommandEvent RenameStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_THREAD_START );
	wxCommandEvent RenameProgEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_THREAD_PROG );	

	wxPostEvent( g_PlaylistCtrl, RenameStartEvt );

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
			g_PlaylistCtrl->SetProgress( nCurrProg );
			wxPostEvent( g_PlaylistCtrl, RenameProgEvt );
		}
		nLastProg = nCurrProg;

		if ( TestDestroy() )
			break;

		g_Library.RenameFile( &m_Songs.Item( i ) );
		Yield();
	}

	g_Playlist = m_Songs;

	return NULL;
}

void MusikPlaylistRenameThread::OnExit()
{
	wxCommandEvent RenameEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_THREAD_END );
	wxPostEvent( g_PlaylistCtrl, RenameEndEvt );
}

MusikPlaylistRetagThread::MusikPlaylistRetagThread( CMusikSongArray songs )
{
	m_Songs = songs;
}

void* MusikPlaylistRetagThread::Entry()
{
	g_PlaylistCtrl->SetActiveThread	( this );
	g_PlaylistCtrl->SetProgressType	( MUSIK_PLAYLIST_RETAG_THREAD );
	g_PlaylistCtrl->SetProgress		( 0 );

	//----------------------------------------//
	//--- events we'll post as we go along ---//
	//----------------------------------------//
	wxCommandEvent RetagStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_THREAD_START );
	wxCommandEvent RetagProgEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_THREAD_PROG );	

	wxPostEvent( g_PlaylistCtrl, RetagStartEvt );

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
			g_PlaylistCtrl->SetProgress( nCurrProg );
			wxPostEvent( g_PlaylistCtrl, RetagProgEvt );
		}
		nLastProg = nCurrProg;

		if ( TestDestroy() )
			break;

		g_Library.RetagFile( &m_Songs.Item( i ) );
		Yield();
	}

	g_Playlist = m_Songs;

	return NULL;
}

void MusikPlaylistRetagThread::OnExit()
{
	wxCommandEvent RetagEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_THREAD_END );
	wxPostEvent( g_PlaylistCtrl, RetagEndEvt );
}
