#include "wx/wxprec.h"
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/wxhtml.h"
#include "wx/statline.h"

#include "MusikAboutDlg.h"
#include "wx/html/m_templ.h"

#include "MusikDefines.h"
#include "MusikUtils.h"
#include <sqlite.h>
#include <fmod.h>


TAG_HANDLER_BEGIN(MUSIK_TAG, "MUSIK")

TAG_HANDLER_PROC(tag)
{

	wxString sTextToInsert;
	wxString sParamValue;
	if (tag.HasParam(wxT("Version")))
	{
		sParamValue = tag.GetParam(wxT("Version"));
		if(	sParamValue == wxT("wxMusik"))
		{
			sTextToInsert = MUSIK_VERSION_STR;
		}
		if(	sParamValue == wxT("wxMusikEx"))
		{
#ifdef  wxUSE_UNICODE
			sTextToInsert = wxT("UNICODE ");
#endif
			sTextToInsert = 
			sTextToInsert += MUSIK_VERSION_DEBUG MUSIK_VERSION_ADDENDUM;
		}
		else if(	sParamValue == wxT("sqlite"))
		{
			
			sTextToInsert = ConvA2W(sqlite_version);
		}
		else if(sParamValue == wxT("wxWidgets"))
		{
#define WX_VERSION_STR		MUSIK_STRINGIZE_T(wxMAJOR_VERSION) wxT(".") \
								MUSIK_STRINGIZE_T(wxMINOR_VERSION) wxT(".") \
								MUSIK_STRINGIZE_T(wxRELEASE_NUMBER) 
			sTextToInsert =WX_VERSION_STR;

		}
		else if(sParamValue == wxT("fmod"))
		{
			sTextToInsert = wxString::Format(wxT("%.2f"),FSOUND_GetVersion());

		}
	//		else if(sParamValue == wxT("id3lib"))
//		{
//			sTextToInsert	<< ID3LIB_MAJOR_VERSION << wxT(".") 
//							<< ID3LIB_MINOR_VERSION << wxT(".") 
//							<< ID3LIB_PATCH_VERSION << wxT(".") 
//							<< ID3LIB_INTERFACE_AGE	;
//
//		}
		else if(sParamValue == wxT("OggVorbis"))
		{
			sTextToInsert = wxT("");
		}
		else if(sParamValue == wxT("Shibatch"))
		{
			sTextToInsert = wxT("0.3");
		}

		m_WParser->GetContainer()->InsertCell(new wxHtmlWordCell(sTextToInsert, *(m_WParser->GetDC())));
		return FALSE;
	}


	return FALSE;
}

TAG_HANDLER_END(MUSIK_TAG)



TAGS_MODULE_BEGIN(MUSIK_TAG)

TAGS_MODULE_ADD(MUSIK_TAG)

TAGS_MODULE_END(MUSIK_TAG)


CMusikAboutDlg::CMusikAboutDlg(wxWindow *pParent)
:wxDialog(pParent, wxID_ANY, wxString(_("About")),wxDefaultPosition,wxDefaultSize)
{
	wxBoxSizer *topsizer;
	wxHtmlWindow *html;

	topsizer = new wxBoxSizer(wxVERTICAL);

	html = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize/* , 0wxHW_SCROLLBAR_NEVER*/);
	html -> SetBorders(0);
	html -> LoadPage(MusikGetStaticDataPath() + wxT("about.htm"));
	html -> SetSize(html -> GetInternalRepresentation() -> GetWidth()+wxSystemSettings::GetMetric(wxSYS_VSCROLL_X), 
		400);//html -> GetInternalRepresentation() -> GetHeight());

	topsizer -> Add(html, 1, wxBOTTOM, 10);

	topsizer -> Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

	wxButton *bu1 = new wxButton(this, wxID_OK, _("OK"));
	bu1 -> SetDefault();
	topsizer -> Add(bu1, 0, wxALL | wxALIGN_RIGHT, 15);
	SetAutoLayout(true);
	SetSizer(topsizer);
	topsizer->Fit(this);
	Layout();	
	Centre();
}

CMusikAboutDlg::~CMusikAboutDlg(void)
{
}
