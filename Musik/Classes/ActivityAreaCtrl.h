/*
 *  ActivityAreaCtrl.h
 *
 *  The "Activity Area" control
 *	This control puts all the activity boxes next to each other
 *	and manages creation / deletion of them, as well as events.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_ACTIVITYAREA_CTRL
#define MUSIK_ACTIVITYAREA_CTRL

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "ActivityBox.h"

class CActivityAreaCtrl : public wxPanel
{
public:
	CActivityAreaCtrl( wxWindow *pParent );
	~CActivityAreaCtrl();

	//--- events ---//
	void OnActivityBox1Focused		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox2Focused		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox3Focused		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox4Focused		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox1SelChange	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox2SelChange	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox3SelChange	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox4SelChange	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox1SelDrag		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox2SelDrag		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox3SelDrag		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox4SelDrag		( wxListEvent& WXUNUSED(event) );
	void OnActivityBox1Activated	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox2Activated	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox3Activated	( wxListEvent& WXUNUSED(event) );
	void OnActivityBox4Activated	( wxListEvent& WXUNUSED(event) );
	
	//--- member functions ---//
	void UpdateSel( CActivityBox *pSel );
	bool Create();
	void Delete();
	void ResetAllContents();
	void RescaleColumns();

	bool IsSelecting(){ return m_SelInProg; }
	void SetSelecting( bool bSelInProg = true ){ m_SelInProg = bSelInProg; }

	void SetParent( int ACTIVITY_BOX_ID, bool bUpdate = false );

	int GetParentId(){ return m_ParentId; }
	CActivityBox* GetParentBox(){ return m_ParentBox; }

	DECLARE_EVENT_TABLE()

private:
	bool m_SelInProg;
	int m_ParentId;
	CActivityBox *m_ParentBox;

	CActivityBox *m_ActivityBox1;		
	CActivityBox *m_ActivityBox2;	
	CActivityBox *m_ActivityBox3;	
	CActivityBox *m_ActivityBox4;
	wxBoxSizer* pTopSizer;

};

#endif
