#include "wx/wxprec.h"
#include "MusikFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

//--- images ---//
#include "images/sources/dynamic.xpm"
#include "images/sources/library.xpm"
#include "images/sources/standard.xpm"
#include "images/ratings/rating0.xpm"
#include "images/ratings/rating1.xpm"
#include "images/ratings/rating2.xpm"
#include "images/ratings/rating3.xpm"	
#include "images/ratings/rating4.xpm"
#include "images/ratings/rating5.xpm"

// main dialog constructor
MusikFrame::MusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, wxT( "Musik Test UI" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN )
{
	SetIcon( wxICON( musicbox ) );
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	wxBoxSizer *p = new wxBoxSizer( wxHORIZONTAL );
	m_SourcesCtrl = new CMusikSourcesCtrl( this, -1 );
	m_SelectionCtrl = new CMusikSelectionAreaCtrl( this, -1 );
	p->Add( m_SourcesCtrl, 1, wxEXPAND | wxALL, 2 );
	p->Add( m_SelectionCtrl, 3, wxEXPAND | wxALL, 2 );
	SetSizer( p );
	Layout();
	Centre();
}

MusikFrame::~MusikFrame()
{
}