#include "wx/wxprec.h"
#include <wx/laywin.h>
#include "MusikFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

MusikFrame::MusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, wxT( "Musik Test UI" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE )
{
	SetIcon( wxICON( musicbox ) );
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

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
	Layout();
	Centre();
}

MusikFrame::~MusikFrame()
{
}