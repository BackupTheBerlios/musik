#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include "wx/wxprec.h"

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
	wxWindow*	m_NowPlayingSash;
	wxWindow*	m_SourcesSash;
	wxWindow*	m_SelectionAreaSash;
	wxWindow*	m_PlaylistSash;	
	wxWindow*	m_SimpleQuerySash;

	//-------------------------------------------------//
	//--- main controls								---//
	//-------------------------------------------------//
	CMusikSourcesCtrl *m_SourcesCtrl;
	CMusikSelectionAreaCtrl *m_SelectionCtrl;
	CMusikPlaylistCtrl *m_PlaylistCtrl;
	CMusikSimpleQueryCtrl *m_SimpleQueryCtrl;
};

#endif
