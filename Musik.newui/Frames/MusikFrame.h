#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include <wx/socket.h>
#include "../Classes/MusikSourcesCtrl.h"

class MusikFrame : public wxFrame
{
public:
	MusikFrame();
	~MusikFrame();

	CMusikSourcesCtrl *m_SourcesCtrl;
	
};

#endif
