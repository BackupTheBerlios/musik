/*
 *  MusikApp.cpp
 *
 *  Application start point
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"
#include "wx/textfile.h"
#include "wx/cmdline.h"
#include "MusikApp.h"
IMPLEMENT_APP( MusikApp )



//--- related frames ---//
#include "Frames/MusikFrame.h"
#include "Frames/MusikLibraryFrame.h"

//--- globals ---//
#include "MusikGlobals.h"
#include "MusikUtils.h"
#define MUSIK_APP_SERVICE wxT("/tmp/wxMusikApp.server")

class MusikAppConnection: public wxConnection
{
public:
	bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
	bool OnDisconnect();
};

class MusikAppClient: public wxClient
{
public:
	wxConnectionBase *OnMakeConnection()
	{
		return new MusikAppConnection;
	}
};


bool MusikAppConnection::OnPoke(const wxString& WXUNUSED(topic), const wxString& item, wxChar *data, int size, wxIPCFormat format)

{
	if((item == wxT("PlayFiles")) && (format == wxIPC_TEXT) )
	{
		wxString sData(data,size);
		wxArrayString aFilelist;
		DelimitStr(sData,wxT("\n"),aFilelist);
		wxGetApp().OnPlayFiles(aFilelist);
	}
	return TRUE;
}
bool MusikAppConnection::OnDisconnect()
{
	return wxConnection::OnDisconnect();
}
void MusikApp::OnPlayFiles(const wxArrayString &aFilelist)
{
	static bool bRecursiveEntry= false;
	if(bRecursiveEntry)
		return;
	bRecursiveEntry = true;
	if(g_MusikFrame)
	{
		g_MusikFrame->AutoUpdate(aFilelist,true);
	}
	bRecursiveEntry = false;
}
bool MusikApp::OnInit()
{
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_PARAM,  NULL, NULL, "mp3/ogg file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL},
		{ wxCMD_LINE_NONE }
	};

	wxCmdLineParser parser(argc, argv);
	parser.SetDesc(cmdLineDesc);

	parser.Parse(false);

	wxArrayString arrParams;
	for (size_t i = 0; i <parser.GetParamCount(); i++)
	{
		arrParams.Add(parser.GetParam(i));
	}
	m_pSingleInstanceChecker = new wxSingleInstanceChecker(GetAppName());
	if ( m_pSingleInstanceChecker->IsAnotherRunning() )
	{
		MusikAppClient client;
		MusikAppConnection *pConn = (MusikAppConnection *)client.MakeConnection(wxT("localhost"),MUSIK_APP_SERVICE,wxT("wxMusikInternal"));
		wxString sData;
		for( size_t i = 0; i < arrParams.GetCount(); i++)
		{
			sData += arrParams[i];
			sData += wxT("\n");
		}
		pConn->Poke(wxT("PlayFiles"),sData.GetWriteBuf(sData.Length()),sData.Length(),wxIPC_TEXT);

		return false;
	}

	//--- setup our home dir ---//
	if ( !wxDirExists( MUSIK_HOME_DIR ) )
		wxMkdir( MUSIK_HOME_DIR );

	//-----------------------------------------//
	//--- check to see if a new version has	---//
	//--- been installed. if it has, see	---//
	//--- if any core changes need to be	---//
	//--- made.								---//
	//-----------------------------------------//
	CheckVersion();
	
	//--- assure playlists directory exists ---//
	if ( !wxDirExists( MUSIK_PLAYLIST_DIR ) )
		wxMkdir( MUSIK_PLAYLIST_DIR );

	//--- load library and paths ---//
	if(!g_Library.Load())
	{
		wxMessageBox( _("Initialization of library failed."), MUSIKAPPNAME_VERSION, wxOK | wxICON_ERROR );
		return FALSE;
	}
	g_Paths.Load();
	g_Player.Init(arrParams.GetCount() > 0);
	//--- initialize fmod ---//
	if ( g_Player.InitializeFMOD( FMOD_INIT_START ) != FMOD_INIT_SUCCESS )
		wxMessageBox( _("Initialization of FMOD sound system failed."), MUSIKAPPNAME_VERSION, wxOK | wxICON_ERROR );


	wxImage::AddHandler( new wxXPMHandler );


	//-------------------//
	//--- main window ---//
	//-------------------//
    MusikFrame *pMain = new MusikFrame();

	//--- restore placement or use defaults ---//
	if ( !SetFramePlacement( pMain, g_Prefs.sFramePlacement ) )
	{
		wxSize Size( 
			wxSystemSettings::GetMetric( wxSYS_SCREEN_X ) * 75 / 100, 
			wxSystemSettings::GetMetric( wxSYS_SCREEN_Y ) * 75 / 100 );
		pMain->SetSize( Size );
		pMain->Center();
	}
	pMain->SetTitle( MUSIKAPPNAME_VERSION );
	pMain->Show();

	SetTopWindow( pMain );

	//--- start webserver if necessary ---//
	if ( g_Prefs.bWebServerEnable )
		g_WebServer.Start();

	//--- autostart stuff ---//
	if ( g_Prefs.bFirstRun )
	{
		wxCommandEvent dummy_ev;
		pMain->OnSetupPaths(dummy_ev);
	}
	else if (g_Prefs.bAutoAdd || arrParams.GetCount() > 0)
	{	if(g_Prefs.bAutoAdd)
			pMain->AutoUpdate();
		if(arrParams.GetCount() > 0)
			pMain->AutoUpdate(arrParams,true);
	}
	else
	{// as standard select now playing, its faster than selecting the library.
		g_SourcesCtrl->SelectNowPlaying();
		g_PlaylistBox->Update();
	}
	//--- startup the crossfader			---//
	g_FaderThread = new MusikFaderThread();
	g_FaderThread->Create();
	g_FaderThread->Run();

	m_Server.Create(MUSIK_APP_SERVICE);
	return TRUE;
}

void MusikApp::CheckVersion()
{
	wxString sVersion;
	if ( wxFileExists( MUSIK_VERSION_FILENAME ) )
	{
		sVersion = ReadVersion();

		if ( sVersion == wxT( "Musik 0.1.3" ) || sVersion == wxT( "Musik 0.1.3 CVS" ) )
		{
//			wxMessageBox( wxT( MUSIKAPPNAME" has detected 0.1.3 was previously installed. Due to the changes in the playlist display preferences, your columns will be reset. We apologize for any inconvenience this may cause." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
		}

		WriteVersion();
	}
	else
	{
		//-------------------------------------------------//
		//--- if these files exist, but the version.dat	---//
		//--- does not, some version prior to 0.1.3 was	---//
		//--- installed. give user a nice little		---//
		//--- warning.									---//
		//-------------------------------------------------//
		if ( wxFileExists( MUSIK_SOURCES_FILENAME ) || wxFileExists( MUSIK_DB_FILENAME ) )
		{
			wxMessageBox( wxT( MUSIKAPPNAME" has detected version 0.1.2 or earlier was previously installed.\n\nDue to the changes from 0.1.2 to the current version, your Sources list and Library must be reset. We apologize for any inconvenience this may cause." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
	
			if ( wxFileExists( MUSIK_SOURCES_FILENAME ) )
				wxRemoveFile( MUSIK_SOURCES_FILENAME );
			if ( wxFileExists( MUSIK_DB_FILENAME ) )
				wxRemoveFile( MUSIK_DB_FILENAME );
		}	

		WriteVersion();
	}
}

wxString MusikApp::ReadVersion()
{
	wxTextFile ver( MUSIK_VERSION_FILENAME );
	wxString sRet;
	if ( ver.Open() )
	{
		if ( ver.GetLineCount() != 0 )
			sRet = ver.GetLine( 0 );
		ver.Close();
	}
	return sRet;
}

void MusikApp::WriteVersion()
{
	wxTextFile ver;
	if ( !wxFileExists( MUSIK_VERSION_FILENAME ) )
	{
		ver.Create( MUSIK_VERSION_FILENAME );
	}
	ver.Open( MUSIK_VERSION_FILENAME );
    
	if ( ver.IsOpened() )
	{
		if ( ver.GetLineCount() != 0 )
    		ver.RemoveLine(0);
		ver.AddLine( MUSIKAPPNAME_VERSION );
		ver.Write();
		ver.Close();
	}
 	else
  	{
    		wxASSERT(0);
  	}
}

wxConnectionBase * MusikAppServer::OnAcceptConnection(const wxString& topic)
{

	return new MusikAppConnection;
}