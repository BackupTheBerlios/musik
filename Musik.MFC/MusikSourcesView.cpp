// MusikSourcesView.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesView.h"


// CMusikSourcesView

IMPLEMENT_DYNCREATE(CMusikSourcesView, CFormView)

CMusikSourcesView::CMusikSourcesView()
	: CFormView(CMusikSourcesView::IDD)
{
}

CMusikSourcesView::~CMusikSourcesView()
{
}

void CMusikSourcesView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMusikSourcesView, CFormView)
END_MESSAGE_MAP()


// CMusikSourcesView diagnostics

#ifdef _DEBUG
void CMusikSourcesView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMusikSourcesView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CMusikSourcesView message handlers
