#include "wx/wxprec.h"

#include "MusikFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

CMusikFrame::CMusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, wxT( "Musik Test UI" ), wxDefaultPosition, wxSize( 800, 600 ), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE )
{
	Centre();

	SetIcon( wxICON( musicbox ) );
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	CreateSashes();
	CreateControls();

	Layout();
	Refresh();
}

CMusikFrame::~CMusikFrame()
{
}

void CMusikFrame::CreateSashes()
{
	//---------------------------------//
	//--- main window.				---//
	//---------------------------------//	
	wxSashLayoutWindow *pSash;
	pSash = new wxSashLayoutWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	wxSashLayoutWindow* pMain = pSash;

	//---------------------------------//
	//--- now playing				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, 80 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetBackgroundColour( wxColour( 255, 0, 255 ) );
	pSash->SetSashVisible( wxSASH_TOP, true );
	m_NowPlayingSash = pSash;

	//---------------------------------//
	//--- left side					---//
	//---------------------------------//	
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_LEFT );
	pSash->SetDefaultSize( wxSize( 120, -1 ) );
	pSash->SetOrientation( wxLAYOUT_VERTICAL );
	pSash->SetSashVisible( wxSASH_RIGHT, true );
	wxWindow* pLeft = pSash;

	//---------------------------------//
	//--- simple query				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pLeft, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, 24 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetBackgroundColour( wxColour( 0, 255, 255 ) );
	m_SimpleQuerySash = pSash;

	//---------------------------------//
	//--- sources					---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pLeft, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_TOP );
	pSash->SetDefaultSize( wxSize( -1, -1 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetBackgroundColour( wxColour( 0, 0, 255 ) );
	m_SourcesSash = pSash;

	//---------------------------------//
	//--- right side				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_RIGHT );
	pSash->SetDefaultSize( wxSize( 20, -1 ) );
	pSash->SetOrientation( wxLAYOUT_VERTICAL );
	pSash->SetBackgroundColour( wxColour( 0, 0, 255 ) );
	wxWindow* pRight = pSash;

	//---------------------------------//
	//--- selection area			---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pRight, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_TOP );
	pSash->SetDefaultSize( wxSize( -1, 120 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetSashVisible( wxSASH_BOTTOM, true );
	pSash->SetBackgroundColour( wxColour( 0, 255, 0 ) );
	m_SelectionAreaSash = pSash;

	//---------------------------------//
	//--- playlist area				---//
	//---------------------------------//
	pSash = new wxSashLayoutWindow( pRight, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTRANSPARENT_WINDOW );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, -1 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetBackgroundColour( wxColour( 255, 255, 0 ) );
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

	//--- left or "sources" area	---//
	m_SourcesCtrl		= new CMusikSourcesCtrl			( m_SourcesSash, -1 );
	m_SelectionCtrl		= new CMusikSelectionAreaCtrl	( m_SelectionAreaSash, -1 );
	m_PlaylistCtrl		= new CMusikPlaylistCtrl		( m_PlaylistSash, -1 );
	m_SimpleQueryCtrl	= new CMusikSimpleQueryCtrl		( m_SimpleQuerySash, -1 );

	Layout();
}