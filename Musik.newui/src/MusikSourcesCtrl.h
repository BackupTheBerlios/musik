#ifndef C_MUSIK_SOURCES_CTRL_H
#define C_MUSIK_SOURCES_CTRL_H

#include "wx/wxprec.h"
#include "MusikListCtrl.h"

#include "images/sources/dynamic.xpm"
#include "images/sources/library.xpm"
#include "images/sources/standard.xpm"

enum E_MUSIK_SOURCES_TYPE
{
	MUSIK_SOURCES_LIBRARY = 0,
    MUSIK_SOURCES_PLAYLIST_STANDARD,
	MUSIK_SOURCES_PLAYLIST_DYNAMIC,
	MUSIK_SOURCES_CDROM_DRIVE,
	MUSIK_SOURCES_ALARM,
	MUSIK_SOURCES_NOW_PLAYING,
	MUSIK_SOURCES_NONE
};

class CMusikSourcesCtrl : public CMusikListCtrl
{
public:
	CMusikSourcesCtrl( wxWindow* parent, wxWindowID id );
	~CMusikSourcesCtrl( );

protected:
	virtual void OnResize			( wxSizeEvent& event );
	virtual void OnColBeginDrag		( wxListEvent& event );
	virtual void RescaleColumns		( bool refresh = false );
	virtual void Reset				( bool rescale = true, bool refresh = false );
	virtual void Update				( bool refresh = false );

private:

};

#endif