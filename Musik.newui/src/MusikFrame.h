#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include <wx/socket.h>
#include "MusikSourcesCtrl.h"
#include "MusikSelectionCtrl.h"

class MusikFrame : public wxFrame
{
public:
	MusikFrame();
	~MusikFrame();

	CMusikSourcesCtrl *m_SourcesCtrl;
	CMusikSelectionCtrl *m_SelectionCtrl;
	
};

#endif
