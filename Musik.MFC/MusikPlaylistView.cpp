// MusikPlaylistView.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPlaylistView.h"


// CMusikPlaylistView

IMPLEMENT_DYNCREATE(CMusikPlaylistView, CView)

CMusikPlaylistView::CMusikPlaylistView()
{
}

CMusikPlaylistView::~CMusikPlaylistView()
{
}

BEGIN_MESSAGE_MAP(CMusikPlaylistView, CView)
END_MESSAGE_MAP()


// CMusikPlaylistView drawing

void CMusikPlaylistView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CMusikPlaylistView diagnostics

#ifdef _DEBUG
void CMusikPlaylistView::AssertValid() const
{
	CView::AssertValid();
}

void CMusikPlaylistView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CMusikPlaylistView message handlers
