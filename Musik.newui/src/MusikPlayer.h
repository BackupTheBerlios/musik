#ifndef C_MUSIK_PLAYER_H
#define C_MUSIK_PLAYER_H

#include "wx/wxprec.h"
#include "wx/thread.h"

class CMusikPlayer : public wxThread
{
public:
	CMusikPlayer( wxEvtHandler* parent );
	~CMusikPlayer();

protected:

private:
	wxEvtHandler* m_Parent;

};

#endif