#ifndef C_SELECTION_AREA_CTRL_H
#define C_SELECTION_AREA_CTRL_H

#include "wx/wxprec.h"
#include "MusikSelectionCtrl.h"

#define MAX_SELECTION_CTRLS 2
#define HORIZONTAL 1

class CMusikSelectionAreaCtrl : public wxPanel
{
public:
	CMusikSelectionAreaCtrl( wxWindow *parent, wxWindowID id );
	~CMusikSelectionAreaCtrl();

	CMusikSelectionCtrl* GetControl( size_t nCtrlID ); 
	void Reset();

private:
	void Create();
	void Delete();

	wxBoxSizer *m_SelectionSizer;
	CMusikSelectionCtrl *m_SelectionCtrls[MAX_SELECTION_CTRLS];
};

#endif