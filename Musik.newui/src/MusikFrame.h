#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include "wx/wxprec.h"
#include "wx/laywin.h"

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
	wxSashLayoutWindow*	m_NowPlayingSash;
	wxSashLayoutWindow*	m_SourcesSash;
	wxSashLayoutWindow*	m_SelectionAreaSash;
	wxSashLayoutWindow*	m_PlaylistSash;	
	wxSashLayoutWindow*	m_SimpleQuerySash;

	//-------------------------------------------------//
	//--- main controls								---//
	//-------------------------------------------------//
	CMusikSourcesCtrl		*m_SourcesCtrl;
	CMusikSelectionAreaCtrl *m_SelectionCtrl;
	CMusikPlaylistCtrl		*m_PlaylistCtrl;
	CMusikSimpleQueryCtrl	*m_SimpleQueryCtrl;
};

#endif
