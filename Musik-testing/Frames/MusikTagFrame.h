/*
 *  MusikTagFrame.h
 *
 *  Tag editing frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_TAG_FRAME_H
#define MUSIK_TAG_FRAME_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 
#include "wx/thread.h"

//--- CMusikSongArray defined here ---//
#include "../Classes/MusikLibrary.h"

class MusikTagApplyThread;

enum EMUSIK_TAG_OBJECT_ID
{
	MUSIK_TAG_TITLE = 0,
	MUSIK_TAG_TRACKNUM,
	MUSIK_TAG_ARTIST,
	MUSIK_TAG_ALBUM,
	MUSIK_TAG_GENRE,
	MUSIK_TAG_YEAR,
	MUSIK_TAG_CHK_TITLE,
	MUSIK_TAG_CHK_TRACKNUM,
	MUSIK_TAG_CHK_ARTIST,
	MUSIK_TAG_CHK_ALBUM,
	MUSIK_TAG_CHK_GENRE,
	MUSIK_TAG_CHK_YEAR,
	MUSIK_TAG_CHK_WRITETAG,
	MUSIK_TAG_CHK_WRITETAG_CLEAR,
	MUSIK_TAG_CHK_RENAME,
	MUSIK_TAG_SINGLE,
	MUSIK_TAG_MULTIPLE,
	MUSIK_TAG_CHECK,
	MUSIK_TAG_CANCEL,
	MUSIK_TAG_APPLY,
	MUSIK_TAG_OK,
	MUSIK_TAG_NEXT,
	MUSIK_TAG_PREV
};

class MusikTagFrame : public wxFrame
{
public:
	MusikTagFrame( wxFrame* pParent, CMusikSongArray aSongs, int nCurFrame, int nEditType, int n );

	//--- objects ---//
	wxTextCtrl  *tcFilename;
	wxTextCtrl	*tcTitle;
	wxCheckBox	*chkTitle;
	wxTextCtrl	*tcTrackNum;
	wxCheckBox	*chkTrackNum;
	wxTextCtrl	*tcArtist;
	wxCheckBox	*chkArtist;
	wxTextCtrl	*tcAlbum;
	wxCheckBox	*chkAlbum;
	wxTextCtrl	*tcYear;
	wxCheckBox	*chkYear;

	wxCheckBox  *chkWriteTag;
	wxCheckBox  *chkClear;
	wxCheckBox	*chkRename;

	wxComboBox	*cmbGenre;
	wxCheckBox	*chkGenre;

	wxGauge		*gProgress;

	wxButton	*btnCancel;
	wxButton	*btnApply;
	wxButton	*btnOK;
	wxButton	*btnNext;
	wxButton	*btnPrev;

	wxBoxSizer	*hsRow0;
	wxBoxSizer	*hsRow1;
	wxBoxSizer	*hsRow2;
	wxBoxSizer	*hsRow3;
	wxBoxSizer	*hsRow4;
	wxBoxSizer	*hsRow5;
	wxBoxSizer	*hsRow6;
	wxBoxSizer	*vsRows;	
	wxBoxSizer	*hsNav;
	wxBoxSizer	*vsTopSizer;

	//--- regular event handlers ---//
	void OnClickOK				( wxCommandEvent &WXUNUSED(event) );
	void OnClickCancel			( wxCommandEvent &WXUNUSED(event) );
	void OnClickApply			( wxCommandEvent &WXUNUSED(event) ) { Apply(); 							}
	void OnClickCheckTitle		( wxCommandEvent &WXUNUSED(event) ) { nFrame = 0; SetEnabled();			}
	void OnClickCheckTrackNum	( wxCommandEvent &WXUNUSED(event) ) { nFrame = 1; SetEnabled();			}
	void OnClickCheckArtist		( wxCommandEvent &WXUNUSED(event) ) { nFrame = 2; SetEnabled();			}
	void OnClickCheckAlbum		( wxCommandEvent &WXUNUSED(event) ) { nFrame = 3; SetEnabled();			}
	void OnClickCheckGenre		( wxCommandEvent &WXUNUSED(event) ) { nFrame = 4; SetEnabled();			}
	void OnClickCheckYear		( wxCommandEvent &WXUNUSED(event) ) { nFrame = 5; SetEnabled();			}
	void OnClickNext			( wxCommandEvent &WXUNUSED(event) ) { Next();								}
	void OnClickPrev			( wxCommandEvent &WXUNUSED(event) ) { Prev();								}
	void OnClose				( wxCommandEvent &WXUNUSED(event) ); 
	void OnTranslateKeys		( wxKeyEvent& event		);

	//--- thread event handlers ---//
	void OnTagThreadStart	( wxCommandEvent &WXUNUSED(event) );
	void OnTagThreadEnd		( wxCommandEvent &WXUNUSED(event) );	
	void OnTagThreadProg	( wxCommandEvent &WXUNUSED(event) );	

	//--- regular functions ---//
	void PopulateTagDlg	();
	void SetFocus		();
	void SetChecks		( const int i );
	void SetEnabled		();
	void Apply			( bool close = false );
	void SaveCurSong	();
	void Next			();
	void Prev			();
	void LoadFirst		();
	void SetCaption		();

	void CheckChangesSingle	();
	void CheckChangesBatch	();

	//--- thread related functions ---//
	void SetProgress			( int n	)				{ m_Progress = n; }
	void SetProgressType		( int n )				{ m_ProgressType = n;		}
	void SetActiveThread		( wxThread* thread	)	{ m_ActiveThread = thread;	}
	int  GetProgress			()						{ return m_Progress;		}
	int  GetProgressType		()						{ return m_ProgressType;	}
	wxThread* GetActiveThread	()						{ return m_ActiveThread;	}
	CMusikSongArray* GetSongs	()						{ return &m_Songs;			}

	void EnableProgress			( bool enable = true );

	CMusikSongArray m_Songs;
	bool			m_WriteTag;
	int				nIndex;
	int				nFrame;
	int				nType;

	DECLARE_EVENT_TABLE()
private:
	int m_FrameType;

	//--- thread related stuff ---//
	MusikTagApplyThread	*pApplyThread;
	int m_Progress;
	int m_ProgressType;
	wxThread* m_ActiveThread;

	bool m_Close;

};

#endif