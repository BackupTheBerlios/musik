#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include <wx/socket.h>
#include "MusikSourcesCtrl.h"
#include "MusikSelectionAreaCtrl.h"
#include "MusikPlaylistCtrl.h"

class MusikFrame : public wxFrame
{
public:
	MusikFrame();
	~MusikFrame();

	CMusikSourcesCtrl *m_SourcesCtrl;
	CMusikSelectionAreaCtrl *m_SelectionCtrl;
	CMusikPlaylistCtrl *m_PlaylistCtrl;
	
};

#endif
