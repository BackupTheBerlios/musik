#ifndef C_MUSIK_FRAME_H
#define C_MUSIK_FRAME_H

#include "wx/wxprec.h"
#include "wx/laywin.h"

#include "MusikSourcesCtrl.h"
#include "MusikSelectionAreaCtrl.h"
#include "MusikPlaylistCtrl.h"
#include "MusikSimpleQueryCtrl.h"
#include "MusikNowPlayingCtrl.h"
#include "MusikLibrary.h"

enum E_MUSIK_MAIN_MENU_ITEMS
{
	FILE_MENU_SETUP_LIBRARY = wxID_HIGHEST,
	FILE_MENU_PREFERENCES,
	FILE_MENU_EXIT,

	VIEW_MENU_SHOW_SOURCES,
	VIEW_MENU_SHOW_SELECTION_AREA,
	VIEW_MENU_SHOW_PLAYLIST_INFO,

	HELP_MENU_ONLINE_HELP,
	HELP_MENU_ABOUT
};

class CMusikFrame : public wxFrame
{
public:
	CMusikFrame();
	~CMusikFrame();

	//-------------------------------------------------//
	//--- virtual overrides							---//
	//-------------------------------------------------//
	virtual bool Show( bool show = true );

private:
	void CreateCore();
	void DeleteCore();
	void CreateSashes();
	void CreateControls();
	void CreateMenu();

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
	CMusikNowPlayingCtrl	*m_NowPlayingCtrl;

	//-------------------------------------------------//
	//--- menus										---//
	//-------------------------------------------------//
	wxMenuBar*	main_menu;
	wxMenu*		file_menu;
	wxMenu*		view_menu;
	wxMenu*		help_menu;

	//-------------------------------------------------//
	//--- the core stuff. library / player / etc	---//
	//-------------------------------------------------//
	CMusikLibrary* m_MusikLibrary;
};

#endif
