#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include <wx/socket.h>
#include <wx/laywin.h>
#include "MusikSourcesCtrl.h"
#include "MusikSelectionAreaCtrl.h"
#include "MusikPlaylistCtrl.h"
#include "MusikSimpleQueryCtrl.h"

class CMusikFrame : public wxFrame
{
public:
	CMusikFrame();
	~CMusikFrame();

private:
	void CreateSashes();
	void CreateControls();

	//-------------------------------------------------//
	//--- sashes that are of interest				---//
	//-------------------------------------------------//
	wxSashLayoutWindow* m_SourcesSash;
	wxSashLayoutWindow* m_SelectionAreaSash;
	wxSashLayoutWindow* m_PlaylistSash;	

	//-------------------------------------------------//
	//--- main controls								---//
	//-------------------------------------------------//
	CMusikSourcesCtrl *m_SourcesCtrl;
	CMusikSelectionAreaCtrl *m_SelectionCtrl;
	CMusikPlaylistCtrl *m_PlaylistCtrl;
	CMusikSimpleQueryCtrl *m_SimpleQueryCtrl;

	//-------------------------------------------------//
	//--- main controls' respective sizers			---//
	//-------------------------------------------------//
	wxBoxSizer *m_SourcesSizer;
	wxBoxSizer *m_SelectionSizer;
	wxBoxSizer *m_PlaylistSizer;
};

#endif
