///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"
#include "MusikEditInPlace.h"
#include ".\musikeditinplace.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikEditInPlace, CEdit)

///////////////////////////////////////////////////

int WM_MUSIKEDITCOMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
int WM_MUSIKEDITCANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );

///////////////////////////////////////////////////

CMusikEditInPlace::CMusikEditInPlace()
{
}

///////////////////////////////////////////////////

CMusikEditInPlace::~CMusikEditInPlace()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikEditInPlace, CEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikEditInPlace::Commit()
{
	ShowWindow( SW_HIDE );

	if ( GetParent() )
	{
		int WM_MUSIKEDITCOMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
		GetParent()->SendMessage( WM_MUSIKEDITCOMMIT );
	}
}

///////////////////////////////////////////////////

void CMusikEditInPlace::Cancel()
{
	ShowWindow( SW_HIDE );

	if ( GetParent() )
	{
		int WM_MUSIKEDITCANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );
		GetParent()->SendMessage( WM_MUSIKEDITCANCEL );
	}
}

///////////////////////////////////////////////////

void CMusikEditInPlace::OnEnKillfocus()
{
	Cancel();	
}

///////////////////////////////////////////////////

BOOL CMusikEditInPlace::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
		Commit();

	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
		Cancel();

	return CEdit::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////



void CMusikEditInPlace::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		SetWindowText( m_Str );
		SetSel( 0, m_Str.GetLength() );
	}
}
