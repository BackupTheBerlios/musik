#include "wx/wxprec.h"
#include <wx/laywin.h>
#include "MusikFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

CMusikFrame::CMusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, wxT( "Musik Test UI" ), wxDefaultPosition, wxSize( 800, 600 ), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE )
{
	SetIcon( wxICON( musicbox ) );
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	CreateSashes();
	CreateControls();


	Layout();
	Centre();
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
	pSash->SetAlignment( wxLAYOUT_LEFT );
	pSash->SetDefaultSize( wxSize( 500, 500 ) );
	pSash->SetOrientation( wxLAYOUT_VERTICAL );
	pSash->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	wxWindow* pMain = pSash;

	//---------------------------------//
	//--- sources					---//
	//---------------------------------//	
	pSash = new wxSashLayoutWindow( pMain, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_LEFT );
	pSash->SetDefaultSize( wxSize( 120, -1 ) );
	pSash->SetOrientation( wxLAYOUT_VERTICAL );
	pSash->SetBackgroundColour( wxColour( 255, 0, 0 ) );
	pSash->SetSashVisible( wxSASH_RIGHT, true );
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
	pSash = new wxSashLayoutWindow( pRight, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
	pSash->SetAlignment( wxLAYOUT_BOTTOM );
	pSash->SetDefaultSize( wxSize( -1, -1 ) );
	pSash->SetOrientation( wxLAYOUT_HORIZONTAL );
	pSash->SetBackgroundColour( wxColour( 255, 255, 0 ) );
	m_PlaylistSash = pSash;
}

void CMusikFrame::CreateControls()
{
	/*
	wxBoxSizer *p1 = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *p2 = new wxBoxSizer( wxHORIZONTAL );

	m_SourcesCtrl = new CMusikSourcesCtrl( this, -1 );
	m_SelectionCtrl = new CMusikSelectionAreaCtrl( this, -1 );
	m_PlaylistCtrl = new CMusikPlaylistCtrl( this, -1 );

	p1->Add( m_SelectionCtrl, 1, wxEXPAND | wxALL, 1 );
	p1->Add( m_PlaylistCtrl, 3, wxEXPAND | wxALL, 1 );

	p2->Add( m_SourcesCtrl, 1, wxEXPAND | wxALL, 1 );
	p2->Add( p1, 5, wxEXPAND | wxALL, 1 );

	SetSizer( p2 );
	*/
}