/////////////////////////////////////////////////////////////////////////////
// Name:        MusikAutoTaggerFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     12/28/03 16:11:36
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "MusikAutoTaggerFrame.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
#include "wx/wx.h"
#include "wx/valgen.h"
////@end includes

#include "MusikAutoTaggerFrame.h"
#include "../MusikGlobals.h"
////@begin XPM images
////@end XPM images

/*!
 * CMusikAutoTaggerFrame type definition
 */

IMPLEMENT_CLASS( CMusikAutoTaggerFrame, wxDialog )

/*!
 * CMusikAutoTaggerFrame event table definition
 */

BEGIN_EVENT_TABLE( CMusikAutoTaggerFrame, wxDialog )

////@begin CMusikAutoTaggerFrame event table entries
    EVT_BUTTON( ID_BN_ADDMASK, CMusikAutoTaggerFrame::OnBnAddMask )

    EVT_BUTTON( ID_BN_REMOVEMASK, CMusikAutoTaggerFrame::OnBnRemoveMask )

////@end CMusikAutoTaggerFrame event table entries
    EVT_BUTTON( wxID_OK, CMusikAutoTaggerFrame::OnOK )

END_EVENT_TABLE()

/*!
 * CMusikAutoTaggerFrame constructors
 */

CMusikAutoTaggerFrame::CMusikAutoTaggerFrame( )
{
}

CMusikAutoTaggerFrame::CMusikAutoTaggerFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);

	wxArrayString aMasks =DelimitStr(g_Prefs.sAutoTag,wxT("|"));
	for(size_t i = 0; i < aMasks.Count()-1;i++)
        m_CBTagMask->Append(aMasks[i]);
	if(aMasks.Count() && (wxStringToInt(aMasks[i]) < aMasks.Count()))
		m_sMask = aMasks[wxStringToInt(aMasks[i])];
}

/*!
 * CMusikAutoTaggerFrame creator
 */

bool CMusikAutoTaggerFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin CMusikAutoTaggerFrame member initialisation
////@end CMusikAutoTaggerFrame member initialisation

////@begin CMusikAutoTaggerFrame creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end CMusikAutoTaggerFrame creation
    return TRUE;
}

/*!
 * Control creation for CMusikAutoTaggerFrame
 */

void CMusikAutoTaggerFrame::CreateControls()
{    
////@begin CMusikAutoTaggerFrame content construction

    CMusikAutoTaggerFrame* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxBoxSizer* item3 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item4 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item4, 0, wxGROW, 5);

    wxStaticText* item5 = new wxStaticText( item1, wxID_STATIC, _("Enter Tag Mask:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxString* item6Strings = NULL;
    wxComboBox* item6 = new wxComboBox( item1, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize, 0, item6Strings, wxCB_DROPDOWN );
    m_CBTagMask = item6;
    item4->Add(item6, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxHORIZONTAL);
    item4->Add(item7, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxButton* item8 = new wxButton( item1, ID_BN_ADDMASK, _("+"), wxDefaultPosition, item1->ConvertDialogToPixels(wxSize(12, -1)), 0 );
    item7->Add(item8, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxButton* item9 = new wxButton( item1, ID_BN_REMOVEMASK, _("-"), wxDefaultPosition, item1->ConvertDialogToPixels(wxSize(12, -1)), 0 );
    item7->Add(item9, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxStaticBox* item10Static = new wxStaticBox(item1, -1, _("Usage:"));
    wxStaticBoxSizer* item10 = new wxStaticBoxSizer(item10Static, wxHORIZONTAL);
    item3->Add(item10, 1, wxGROW|wxALL, 5);

    wxStaticText* item11 = new wxStaticText( item1, wxID_STATIC, _("%t for Title, %a for Artist, %b for album,\n%n for tracknumber and %y for year.\n%u can be use for unused parts of the filename."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add(item11, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item12 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item12, 0, wxGROW, 5);

    item12->Add(0, 4, 6, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item14 = new wxButton( item1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->SetDefault();
    item12->Add(item14, 0, wxALIGN_BOTTOM|wxALL, 5);

    wxButton* item15 = new wxButton( item1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item12->Add(item15, 0, wxALIGN_BOTTOM|wxALL, 5);


    // Set validators
    item6->SetValidator( wxGenericValidator( & m_sMask) );
////@end CMusikAutoTaggerFrame content construction
}

/*!
 * Should we show tooltips?
 */

bool CMusikAutoTaggerFrame::ShowToolTips()
{
    return TRUE;
}




/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BN_ADDMASK
 */

void CMusikAutoTaggerFrame::OnBnAddMask( wxCommandEvent& event )
{
   	wxString val = m_CBTagMask->GetValue();
	if(!val.IsEmpty())
	{
		m_CBTagMask->Append(val);
	}
    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BN_REMOVEMASK
 */

void CMusikAutoTaggerFrame::OnBnRemoveMask( wxCommandEvent& event )
{
	wxString val = m_CBTagMask->GetValue();
	if(!val.IsEmpty())
	{
		int pos = m_CBTagMask->FindString(val);
		if(pos  != wxNOT_FOUND)
		{
			m_CBTagMask->Delete(pos);
		}
	}
    // Insert custom code here
    event.Skip();
}

void CMusikAutoTaggerFrame::OnOK(wxCommandEvent& event)
{
	g_Prefs.sAutoTag.Empty();
	for(int i = 0 ; i < m_CBTagMask->GetCount();i++)
	{
		g_Prefs.sAutoTag += m_CBTagMask->GetString(i);
		g_Prefs.sAutoTag +='|';
	}
	if(m_CBTagMask->GetCount() == 0)
		g_Prefs.sAutoTag += wxT("%a-%b-%n%t|");
	int sel = m_CBTagMask->GetSelection();
	sel = (wxNOT_FOUND == sel) ? 0 : sel;
	g_Prefs.sAutoTag += IntTowxString(sel);
	wxDialog::OnOK(event);
}
