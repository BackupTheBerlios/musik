// MusikPlaylistView.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPlaylistView.h"


// CMusikPlaylistView

IMPLEMENT_DYNCREATE(CMusikPlaylistView, CFormView)

CMusikPlaylistView::CMusikPlaylistView()
	: CFormView(CMusikPlaylistView::IDD)
{
}

CMusikPlaylistView::~CMusikPlaylistView()
{
}

void CMusikPlaylistView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMusikPlaylistView, CFormView)
END_MESSAGE_MAP()


// CMusikPlaylistView diagnostics

#ifdef _DEBUG
void CMusikPlaylistView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMusikPlaylistView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CMusikPlaylistView message handlers
