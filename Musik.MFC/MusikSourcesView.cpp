// MusikSourcesView.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesView.h"


// CMusikSourcesView

IMPLEMENT_DYNCREATE(CMusikSourcesView, CView)

CMusikSourcesView::CMusikSourcesView()
{
}

CMusikSourcesView::~CMusikSourcesView()
{
}

BEGIN_MESSAGE_MAP(CMusikSourcesView, CView)
END_MESSAGE_MAP()


// CMusikSourcesView drawing

void CMusikSourcesView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CMusikSourcesView diagnostics

#ifdef _DEBUG
void CMusikSourcesView::AssertValid() const
{
	CView::AssertValid();
}

void CMusikSourcesView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

void CMusikSourcesView::OnSize( UINT nType, int cx, int cy )
{
	CView::OnSize( nType, cx, cy );

	if ( ::IsWindow( m_SourcesCtrl.m_hWnd ) )
		m_SourcesCtrl.MoveWindow( 0, 0, cx, cy, TRUE );
}
// CMusikSourcesView message handlers
