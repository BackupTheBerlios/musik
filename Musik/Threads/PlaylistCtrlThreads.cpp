/*
 *  PlaylistCtrlThreads.cpp
 *
 *  Threads that relates to PlaylistCtrl.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
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
	//----------------------------------------//
	//--- events we'll post as we go along ---//
	//----------------------------------------//
	wxCommandEvent RenameStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_RENAME_THREAD_START );
	wxCommandEvent RenameProgEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_RENAME_THREAD_PROG );	

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
	wxCommandEvent RenameEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYLIST_RENAME_THREAD_END );
	wxPostEvent( g_PlaylistCtrl, RenameEndEvt );
}

