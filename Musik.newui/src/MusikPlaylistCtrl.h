#ifndef C_MUSIK_PLAYLIST_CTRL_H
#define C_MUSIK_PLAYLIST_CTRL_H

#include "wx/wxprec.h"
#include "MusikListCtrl.h"

class CMusikPlaylistCtrl : public CMusikListCtrl
{
public:
	CMusikPlaylistCtrl( wxWindow* parent, wxWindowID id );
	~CMusikPlaylistCtrl();

};

#endif