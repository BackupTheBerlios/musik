/*
 *  MusikLibraryThreads.cpp
 *
 *  Threads related to MusikLibraryFrame.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikLibraryThreads.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//---------------------------------//
//---		update library		---//
//---------------------------------//
MusikUpdateLibThread::MusikUpdateLibThread( wxArrayString* del, wxArrayString & m_refFiles, bool bCompleteRebuild)
	:MusikScanNewThread(m_refFiles)
{
	m_Add	= g_Paths.GetList();
	m_Del	= del;
	m_bCompleteRebuild = bCompleteRebuild;
}

void *MusikUpdateLibThread::Entry()
{
	//--- setup ---//
	g_MusikLibraryFrame->SetProgress		( 0 );
	g_MusikLibraryFrame->SetProgressType	( MUSIK_LIBRARY_UPDATE_THREAD );

	//--- events we'll post as we go along ---//
	wxCommandEvent UpdateLibStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_START );
	wxCommandEvent UpdateProgEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );	

	wxPostEvent( g_MusikLibraryFrame, UpdateLibStartEvt );

	if ( m_Del->GetCount() <= 0 && m_Add->GetCount() <= 0 )
		return NULL;
   g_Library.BeginTransaction();
	//--- remove old songs ---//
	if ( m_Del->GetCount() > 0 )
	{
		for ( size_t i = 0; i < m_Del->GetCount(); i++ )
		{
			if ( m_Del->Item( i ) != wxT("") )
				g_Library.RemoveSongDir( m_Del->Item( i ) );
		}
	}

	//--- search / add new songs ---//
	if(m_refFiles.IsEmpty())
		GetMusicDirs( *m_Add,m_refFiles);

	float fPos;
	int nLastProg = 0;
	int nCurrProg = 0;
	wxString sProgress;
	size_t nTotal = m_refFiles.GetCount();

	g_MusikLibraryFrame->SetTotal( nTotal );
	
	for ( size_t i = 0; i < nTotal; i++ )
	{
		if ( TestDestroy() )
			break;

		else
		{
			//--- figure out progress, then post the event ---//
			fPos = ( i * 100 ) / nTotal;
			nCurrProg = (int)fPos;
			if ( nCurrProg > nLastProg )
			{
				g_MusikLibraryFrame->SetCurrent( i );
				g_MusikLibraryFrame->SetProgress( nCurrProg );
				wxPostEvent( g_MusikLibraryFrame, UpdateProgEvt );
				Yield();
			}
			nLastProg = nCurrProg;
			
			//--- add the item ---//
			if (!g_Library.FileInLibrary( m_refFiles.Item( i ), true ) )
				g_Library.AddSongDataFromFile( m_refFiles.Item( i ) );
			else if(m_bCompleteRebuild)
			{
				g_Library.UpdateSongDataFromFile( m_refFiles.Item( i ) );
			}


		}

	}
	g_Library.EndTransaction();
	//--- clear del list, add list is part of g_Paths ---//
	m_Del->Clear();

	return NULL;
}

void MusikUpdateLibThread::OnExit()
{
	wxCommandEvent UpdateLibEndEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_END );	
	wxPostEvent( g_MusikLibraryFrame, UpdateLibEndEvt );
}

//---------------------------------//
//---	scan for new files		---//
//---------------------------------//
MusikScanNewThread::MusikScanNewThread(wxArrayString & refFiles)
        : wxThread(wxTHREAD_JOINABLE)
		,m_refFiles(refFiles)
{
}

void *MusikScanNewThread::Entry()
{
	//--- setup ---//
	g_MusikLibraryFrame->SetProgress		( 0 );
	g_MusikLibraryFrame->SetProgressType	( MUSIK_LIBRARY_SCANNEW_THREAD );

	//--- events we'll post as we go along ---//
	wxCommandEvent ScanNewStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_START );
	wxCommandEvent ScanNewProgEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );	

	wxPostEvent( g_MusikLibraryFrame, ScanNewStartEvt );

	if ( g_Paths.GetCount() <= 0 )
		return NULL;

	wxString sCurrPath;
	
	float fPos;
	for ( size_t i = 0; i < g_Paths.GetCount(); i++ )
	{
		if ( TestDestroy() )
			break;
		else 
		{
			sCurrPath = g_Paths.Item( i );

			//--- get directory ---//
			int nLastCount = m_refFiles.GetCount();
			GetMusicDir( sCurrPath, m_refFiles );

			//--- do math ---//
			int nTotal		= m_refFiles.GetCount() - nLastCount;
			int nCompare	= g_Library.GetSongDirCount( sCurrPath );
			int nResult		= nTotal - nCompare;

			//--- post update progress event ---//
			fPos = ( i * 100 ) / g_Paths.GetCount();
			g_MusikLibraryFrame->SetProgress( (int)fPos );
			g_MusikLibraryFrame->SetTotal( nTotal );
			g_MusikLibraryFrame->SetNew( nResult );
			g_MusikLibraryFrame->SetListItem( i );
			wxPostEvent( g_MusikLibraryFrame, ScanNewProgEvt );
			Yield();
		}
	}

	return NULL;
}

void MusikScanNewThread::OnExit()
{
	wxCommandEvent ScanNewEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_END );	
	wxPostEvent( g_MusikLibraryFrame, ScanNewEndEvt );
}

class wxMusicTraverser : public wxDirTraverser
{
public:

	wxMusicTraverser(MusikScanNewThread * pThread,wxArrayString& files ) 
		: m_files(files) 
		,m_pThread(pThread)
	{ }

    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
		if(m_pThread->TestDestroy())
		{
			m_files.Clear();
			return wxDIR_STOP;
		}
		wxFileName fn( filename );
		wxString ext = fn.GetExt();
		ext.MakeLower();

		if ( ext == wxT( "mp3" ) || ext == wxT( "ogg" ) ) 
		{
			m_files.Add( filename );

			if ( m_files.GetCount() % 100 == 0 )
			{

				wxCommandEvent UpdateScanProg( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_SCAN_PROG );
				g_MusikLibraryFrame->SetScanCount( m_files.GetCount() );
				wxPostEvent( g_MusikLibraryFrame, UpdateScanProg );
			}
		}
        
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
 		if(m_pThread->TestDestroy())
		{
			m_files.Clear();
			return wxDIR_STOP;
		}
       return wxDIR_CONTINUE;
    }

private:
	wxArrayString& m_files;
	MusikScanNewThread *m_pThread;
};

void MusikScanNewThread::GetMusicDirs( const wxArrayString & aDirs, wxArrayString & aFiles )
{
	aFiles.Clear();
	for ( int i = 0; i < (int)aDirs.GetCount(); i++ )
	{
		GetMusicDir( aDirs[i], aFiles );
	}
}

void MusikScanNewThread::GetMusicDir( const wxString & sDir, wxArrayString & aFiles )
{
	wxString sPath = sDir;
	sPath.Replace( wxT( " " ), wxT( "" ), true );
	if ( !sPath.IsEmpty() )
	{
		wxDir dir( sDir );
		if ( dir.IsOpened() )
		{
			wxMusicTraverser traverser(this, aFiles );
			dir.Traverse( traverser );

			//-----------------------------------------//
			//--- the traverser will post update	---//
			//--- scan new events					---//
			//-----------------------------------------//
		}
	}
}

//---------------------------------//
//---	  purge old files		---//
//---------------------------------//
MusikPurgeLibThread::MusikPurgeLibThread()
        : wxThread(wxTHREAD_JOINABLE)
{
}

void *MusikPurgeLibThread::Entry()
{
	//--- setup ---//
	g_MusikLibraryFrame->SetProgress		( 0 );
	g_MusikLibraryFrame->SetProgressType	( MUSIK_LIBRARY_PURGE_THREAD );

	//--- events we'll post as we go along ---//
	wxCommandEvent PurgeStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_START );
	wxCommandEvent PurgeProgEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );	

	wxPostEvent( g_MusikLibraryFrame, PurgeStartEvt );

	//--- start the process ---//
	wxArrayString songs;
	g_Library.Query( wxT("select filename from songs;"), songs );

	float fPos;
	int nLastProg = 0;
	int nCurrProg = 0;
	wxString sProgress;
	size_t nTotal = songs.GetCount();

	g_MusikLibraryFrame->SetTotal( nTotal );
	g_Library.BeginTransaction();
	for ( size_t i = 0; i < nTotal; i++ )
	{
		if ( TestDestroy() )
			break;
		else 
		{
			//--- post progress event ---//
			fPos = ( i * 100 ) / nTotal;
			nCurrProg = (int)fPos;
			if ( nCurrProg > nLastProg )
			{
				g_MusikLibraryFrame->SetCurrent( i );
				g_MusikLibraryFrame->SetProgress( nCurrProg );
				wxPostEvent( g_MusikLibraryFrame, PurgeProgEvt );
				Yield();
			}
			nLastProg = nCurrProg;

			//--- check item and purge ---//
			g_Library.CheckAndPurge( songs.Item( i ) );
		}
	}
	g_Library.EndTransaction();
	return NULL;
}

void MusikPurgeLibThread::OnExit()
{
	wxCommandEvent PurgeEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_END );	
	wxPostEvent( g_MusikLibraryFrame, PurgeEndEvt );
}

