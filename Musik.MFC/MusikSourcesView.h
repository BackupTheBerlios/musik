#pragma once

#include "MusikSourcesCtrl.h"

class CMusikSourcesView : public CView
{
	DECLARE_DYNCREATE(CMusikSourcesView)

public:
	virtual void OnDraw(CDC* pDC);
	virtual void OnSize( UINT nType, int cx, int cy );

	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

protected:
	CMusikSourcesView();           // protected constructor used by dynamic creation
	virtual ~CMusikSourcesView();

	DECLARE_MESSAGE_MAP()

private:
	CMusikSourcesCtrl m_SourcesCtrl;

};


