/*
 *  ListHeader.h
 *
 *  Header used for ActivityBox and SourcesBox controls
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef LIST_HEADER
#define LIST_HEADER

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

class CListHeader : public wxPanel
{
public:
	CListHeader( wxPanel *pParent, wxString sCaption );
	~CListHeader();

	//--- sets ---//
	void SetCaption	( wxString sCaption );

private:	
	wxBoxSizer			*pSizer;
	wxStaticText		*pHeader;
};

#endif
