#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include <wx/socket.h>
#include "MusikSourcesCtrl.h"
#include "MusikSelectionAreaCtrl.h"
#include "MusikPlaylistCtrl.h"

class CMusikFrame : public wxFrame
{
public:
	CMusikFrame();
	~CMusikFrame();

	CMusikSourcesCtrl *m_SourcesCtrl;
	CMusikSelectionAreaCtrl *m_SelectionCtrl;
	CMusikPlaylistCtrl *m_PlaylistCtrl;

private:
	void CreateSashes();
	void CreateControls();

	wxWindow* m_SourcesSash;
	wxWindow* m_SelectionAreaSash;
	wxWindow* m_PlaylistSash;	
};

#endif
