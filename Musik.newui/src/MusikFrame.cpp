#include "wx/wxprec.h"

#include "MusikFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

CMusikFrame::CMusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, wxT( "Musik Test UI" ), wxDefaultPosition, wxSize( 640, 480 ), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE )
{
	Centre();

	SetIcon( wxICON( musicbox ) );
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	CreateCore();
	CreateSashes();
	CreateControls();
	CreateMenu();

	Layout();
}

CMusikFrame::~CMusikFrame()
{
	DeleteCore();
}

bool CMusikFrame::Show( bool show )
{
	bool ret = wxFrame::Show();

    return ret;
}

void CMusikFrame::CreateCore()
{
	m_MusikLibrary = new CMusikLibrary( MUSIK_DB_FILENAME );
}

void CMusikFrame::DeleteCore()
{
	delete m_MusikLibrary;
}

void CMusikFrame::CreateSashes()
{
	//---------------------------------//
	//--- main window				---//
	//---------------------------------//	
	wxSashLayoutWindow *pSash;
	pSash = new wxSashLayoutWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	wxSashLayoutWindow* pMain = pSash;

	//---------------------------------//
	//--- now playing				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, 52 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	//pSash->SetBackgroundColour( wxColour( 255, 0, 255 ) );
	pSash->SetSashVisible( wxSASH_TOP, true );
	m_NowPlayingSash = pSash;

	//---------------------------------//
	//--- left side					---//
	//---------------------------------//	
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_LEFT );
	pSash->SetDefaultSize( wxSize( 120, -1 ) );
	pSash->SetOrientation( wxLAYOUT_VERTICAL );
	pSash->SetSashVisible( wxSASH_RIGHT, true );
	wxWindow* pLeft = pSash;

	//---------------------------------//
	//--- simple query				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pLeft, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, 24 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetSashVisible( wxSASH_TOP, true );
	//pSash->SetBackgroundColour( wxColour( 0, 255, 255 ) );
	m_SimpleQuerySash = pSash;

	//---------------------------------//
	//--- sources					---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pLeft, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_TOP );
	pSash->SetDefaultSize( wxSize( -1, -1 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	//pSash->SetBackgroundColour( wxColour( 0, 0, 255 ) );
	m_SourcesSash = pSash;

	//---------------------------------//
	//--- right side				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_RIGHT );
	pSash->SetDefaultSize( wxSize( 20, -1 ) );
	pSash->SetOrientation( wxLAYOUT_VERTICAL );
	//pSash->SetBackgroundColour( wxColour( 0, 0, 255 ) );
	wxWindow* pRight = pSash;

	//---------------------------------//
	//--- selection area			---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pRight, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_TOP );
	pSash->SetDefaultSize( wxSize( -1, 120 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetSashVisible( wxSASH_BOTTOM, true );
	//pSash->SetBackgroundColour( wxColour( 0, 255, 0 ) );
	m_SelectionAreaSash = pSash;

	//---------------------------------//
	//--- playlist area				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pRight, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, -1 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	//pSash->SetBackgroundColour( wxColour( 255, 255, 0 ) );
	m_PlaylistSash = pSash;
}

void CMusikFrame::CreateControls()
{
	//-------------------------------------------------//
	//--- grab the default size of a wxTextCtrl,	---//
	//--- then set it to the sizer for the query	---//
	//-------------------------------------------------//
	wxTextCtrl* tmp = new wxTextCtrl( this, -1, wxT( "" ), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_SimpleQuerySash->SetDefaultSize( wxSize( -1, tmp->GetSize().GetHeight() ) );
	delete tmp;

	//-------------------------------------------------//
	//--- create all the controls. just create with	---//
	//--- a parent of the correct value, and the	---//
	//--- wxSashLayoutWindow will take care of the	---//
	//--- rest										---//
	//-------------------------------------------------//
	m_SourcesCtrl		= new CMusikSourcesCtrl			( m_SourcesSash, -1 );
	m_SelectionCtrl		= new CMusikSelectionAreaCtrl	( m_SelectionAreaSash, -1 );
	m_PlaylistCtrl		= new CMusikPlaylistCtrl		( m_PlaylistSash, -1 );
	m_SimpleQueryCtrl	= new CMusikSimpleQueryCtrl		( m_SimpleQuerySash, -1 );
	m_NowPlayingCtrl	= new CMusikNowPlayingCtrl		( m_NowPlayingSash, -1 );
}

void CMusikFrame::CreateMenu()
{
	//-------------------------------------------------//
	//--- file menu									---//
	//-------------------------------------------------//
	file_menu = new wxMenu();
	file_menu->Append( FILE_MENU_SETUP_LIBRARY, wxT( "Setup Library" ), wxT( "" ), wxITEM_NORMAL );
	file_menu->Append( FILE_MENU_PREFERENCES, wxT( "Preferences" ), wxT( "" ), wxITEM_NORMAL );
	file_menu->AppendSeparator();
	file_menu->Append( FILE_MENU_EXIT, wxT( "E&xit" ), wxT( "" ), wxITEM_NORMAL );

	//-------------------------------------------------//
	//--- view menu									---//
	//-------------------------------------------------//
	view_menu = new wxMenu();
	view_menu->Append( VIEW_MENU_SHOW_SOURCES, wxT( "Show Sources" ), wxT( "" ), wxITEM_CHECK );
	view_menu->Append( VIEW_MENU_SHOW_SELECTION_AREA, wxT( "Show Selection Area" ), wxT( "" ), wxITEM_CHECK );
	view_menu->Append( VIEW_MENU_SHOW_PLAYLIST_INFO, wxT( "Show Playlist Info" ), wxT( "" ), wxITEM_CHECK ); 

	//-------------------------------------------------//
	//--- help menu									---//
	//-------------------------------------------------//
	help_menu = new wxMenu();
	help_menu->Append( HELP_MENU_ONLINE_HELP, wxT( "Online Help" ), wxT( "" ), wxITEM_NORMAL );
	help_menu->Append( HELP_MENU_ABOUT, wxT( "About..." ), wxT( "" ), wxITEM_NORMAL );

	//-------------------------------------------------//
	//--- construct and set the main menu bar		---//
	//-------------------------------------------------//
	main_menu = new wxMenuBar();
	main_menu->Append( file_menu, wxT( "&File" ) );
	main_menu->Append( view_menu, wxT( "&View" ) );
	main_menu->Append( help_menu, wxT( "&Help" ) );
	SetMenuBar( main_menu );
}