/*
 *  MusikTagThreads.cpp
 *
 *  Threads related to MusikTagFrame.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "MusikTagThreads.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//-------------------------//
//---	 apply thread	---//
//-------------------------//
MusikTagApplyThread::MusikTagApplyThread()
        : wxThread()
{
	m_Songs = g_MusikTagFrame->GetSongs();
}

void *MusikTagApplyThread::Entry()
{
	//----------------------------------------//
	//--- events we'll post as we go along ---//
	//----------------------------------------//
	wxCommandEvent TagStartEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_TAG_THREAD_START );
	wxCommandEvent TagProgEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_TAG_THREAD_PROG );	

	wxPostEvent( g_MusikTagFrame, TagStartEvt );

	float fPos = 0;
	int nLastProg = 0;
	int nCurrProg = 0;
	bool bRenameOK;
	for( size_t i = 0; i < m_Songs->GetCount(); i++ )
	{
		//-----------------------//
		//--- update progress ---//
		//-----------------------//
		fPos = (float)( i * 100 ) / (float)m_Songs->GetCount();
		nCurrProg = (int)fPos;
		if ( nCurrProg > nLastProg )
		{
			g_MusikTagFrame->SetProgress( nCurrProg );
			wxPostEvent( g_MusikTagFrame, TagProgEvt );
		}
		nLastProg = nCurrProg;

		if ( TestDestroy() )
			break;
		else
		{
			if ( m_Songs->Item( i ).Check1 == 1 )
			{
				//-----------------------//
				//--- rename the file ---//
				//-----------------------//
				bRenameOK = true;
				if ( g_Prefs.nTagDlgRename == 1 )
				{
					bRenameOK = g_Library.RenameFile( &m_Songs->Item( i ), true );
				}

				//--------------------------//
				//--- write tags to file ---//
				//--------------------------//
				if ( bRenameOK )
				{
					if ( g_Prefs.nTagDlgWrite == 1 )
					{
						//-----------------------------------------//
						//--- rename will update the lib, so if	---//
						//--- we're not renaming, update first	---//
						//-----------------------------------------//
    					if ( g_Prefs.nTagDlgRename == 0 )
							g_Library.UpdateItem( m_Songs->Item( i ).Filename, m_Songs->Item( i ), false );

						if ( m_Songs->Item( i ).Format == MUSIK_FORMAT_MP3 )
							g_Library.WriteMP3Tag( m_Songs->Item( i ).Filename, (bool)g_Prefs.nTagDlgClear );
						else if ( m_Songs->Item( i ).Format == MUSIK_FORMAT_OGG )
							g_Library.WriteOGGTag( m_Songs->Item( i ).Filename, (bool)g_Prefs.nTagDlgClear );
					}

					//-----------------------------//
					//--- write tag for db only ---//
					//-----------------------------//
					if ( g_Prefs.nTagDlgWrite == 0 && g_Prefs.nTagDlgRename == 0 )
					{
						g_Library.UpdateItem( m_Songs->Item( i ).Filename, m_Songs->Item( i ), true );
					}
				}
			}
		}
	}

	return NULL;
}

void MusikTagApplyThread::OnExit()
{
	wxCommandEvent TagEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_TAG_THREAD_END );	
	wxPostEvent( g_MusikTagFrame, TagEndEvt );
}

