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
MusikUpdateLibThread::MusikUpdateLibThread(wxEvtHandler *pParent, wxArrayString* del, wxArrayString & m_refFiles, bool bCompleteRebuild)
	:MusikScanNewThread(pParent,m_refFiles)
{
	m_Add	= g_Paths.GetList();
	m_Del	= del;
	m_bCompleteRebuild = bCompleteRebuild;
}

void *MusikUpdateLibThread::Entry()
{
	bool bDatabaseChanged = false;
	//--- events we'll post as we go along ---//
	wxCommandEvent UpdateLibStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_START );
	
	UpdateLibStartEvt.SetExtraLong(MUSIK_LIBRARY_UPDATE_THREAD);
	wxPostEvent( Parent(), UpdateLibStartEvt );

	if ( m_Del->GetCount() || m_Add->GetCount()  )
	{

		wxGetApp().Library.BeginTransaction();
		//--- remove old songs ---//
		if ( m_Del->GetCount() > 0 )
		{
			for ( size_t i = 0; i < m_Del->GetCount(); i++ )
			{
				if ( m_Del->Item( i ) != wxT("") )
					wxGetApp().Library.RemoveSongDir( m_Del->Item( i ) );
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


		wxCommandEvent evtSetTotalFiles	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );
		evtSetTotalFiles.SetInt(SET_TOTAL);
		evtSetTotalFiles.SetExtraLong( nTotal );
		wxPostEvent( Parent(), evtSetTotalFiles );

		wxCommandEvent UpdateProgEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );	
		UpdateProgEvt.SetInt(SET_CURRENT);
		UpdateProgEvt.SetExtraLong( 0 );
		
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
					UpdateProgEvt.SetExtraLong(i);
					wxPostEvent( Parent(), UpdateProgEvt );
				}
				nLastProg = nCurrProg;
				
				//--- add the item ---//
				if (!wxGetApp().Library.FileInLibrary( m_refFiles.Item( i ), true ) )
				{
					bDatabaseChanged = true;
					wxGetApp().Library.AddSongDataFromFile( m_refFiles.Item( i ) );
				}
				else if(m_bCompleteRebuild)
				{
					bDatabaseChanged = true;
					wxGetApp().Library.UpdateSongDataFromFile( m_refFiles.Item( i ) );
				}


			}

		}
		wxGetApp().Library.EndTransaction();
		//--- clear del list, add list is part of g_Paths ---//
		m_Del->Clear();
	}
	wxCommandEvent UpdateLibEndEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_END );	
	UpdateLibEndEvt.SetExtraLong(bDatabaseChanged ? 1:0);
	wxPostEvent( Parent(), UpdateLibEndEvt );
	return NULL;
}

void MusikUpdateLibThread::OnExit()
{

}

//---------------------------------//
//---	scan for new files		---//
//---------------------------------//
MusikScanNewThread::MusikScanNewThread(wxEvtHandler *pParent,wxArrayString & refFiles)
        :m_pParent(pParent), wxThread(wxTHREAD_JOINABLE)
		,m_refFiles(refFiles)
{
}

void *MusikScanNewThread::Entry()
{

	//--- events we'll post as we go along ---//
	wxCommandEvent ScanNewStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_START );
	ScanNewStartEvt.SetExtraLong(MUSIK_LIBRARY_SCANNEW_THREAD);
	wxPostEvent( Parent(), ScanNewStartEvt );

	if ( g_Paths.GetCount() > 0 )
	{

		wxString sCurrPath;
		wxCommandEvent evtSetTotalFiles	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );
		evtSetTotalFiles.SetInt(SET_TOTAL);
		wxCommandEvent ScanNewProgEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );	
		ScanNewProgEvt.SetInt(SET_CURRENT);
		wxCommandEvent evtSetNewFiles	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );
		evtSetNewFiles.SetInt(SET_NEW);
		
		
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
				evtSetTotalFiles.SetExtraLong( nTotal );
				wxPostEvent( Parent(), evtSetTotalFiles );

				int nCompare	= wxGetApp().Library.GetSongDirCount( sCurrPath );
				int nResult		= nTotal - nCompare;

				//--- post update progress event ---//
				evtSetNewFiles.SetExtraLong( nResult );
				wxPostEvent( Parent(), evtSetNewFiles );
				ScanNewProgEvt.SetExtraLong(i);
				wxPostEvent( Parent(), ScanNewProgEvt );
			}
		}
	}
	wxCommandEvent ScanNewEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_END );	
	wxPostEvent( Parent(), ScanNewEndEvt );

	return NULL;
}

void MusikScanNewThread::OnExit()
{
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
				UpdateScanProg.SetExtraLong((long)m_files.GetCount());
				wxPostEvent( m_pThread->Parent(), UpdateScanProg );
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
MusikPurgeLibThread::MusikPurgeLibThread(wxEvtHandler *pParent)
        : m_pParent(pParent), wxThread(wxTHREAD_JOINABLE)
{
}

void *MusikPurgeLibThread::Entry()
{
	//--- setup ---//
	 bool bDatabaseChanged = false;
	//--- events we'll post as we go along ---//
	wxCommandEvent PurgeStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_START );
	

	PurgeStartEvt.SetExtraLong(MUSIK_LIBRARY_PURGE_THREAD);
	wxPostEvent( Parent(), PurgeStartEvt );

	//--- start the process ---//
	wxArrayString songs;
	wxGetApp().Library.Query( wxT("select filename from songs;"), songs );

	float fPos;
	int nLastProg = 0;
	int nCurrProg = 0;
	wxString sProgress;
	size_t nTotal = songs.GetCount();

	wxCommandEvent evtSetTotalFiles	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );
	evtSetTotalFiles.SetInt(SET_TOTAL);
	evtSetTotalFiles.SetExtraLong( nTotal );
	wxPostEvent( Parent(), evtSetTotalFiles );
	
	wxCommandEvent PurgeProgEvt		( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_PROG );	
	PurgeProgEvt.SetInt(SET_CURRENT);

	wxGetApp().Library.BeginTransaction();
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
				PurgeProgEvt.SetExtraLong( i );
				wxPostEvent( Parent(), PurgeProgEvt );
			}
			nLastProg = nCurrProg;

			//--- check item and purge ---//
			if(wxGetApp().Library.CheckAndPurge( songs.Item( i ) ))
				bDatabaseChanged = true;
		}
	}
	wxGetApp().Library.EndTransaction();
	wxCommandEvent PurgeEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_LIBRARY_THREAD_END );
	PurgeEndEvt.SetExtraLong(bDatabaseChanged ? 1:0);
	wxPostEvent( Parent(), PurgeEndEvt );

	return NULL;
}

void MusikPurgeLibThread::OnExit()
{
}

