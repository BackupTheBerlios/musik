#ifndef C_MUSIK_SIMPLE_QUERY_CTRL_H
#define C_MUSIK_SIMPLE_QUERY_CTRL_H

#include "wx/wxprec.h"

class CMusikSimpleQueryCtrl : public wxTextCtrl
{
public:
	CMusikSimpleQueryCtrl( wxWindow* parent, wxWindowID id );
	~CMusikSimpleQueryCtrl();

private:
	
	wxWindow* m_Parent;
};

#endif