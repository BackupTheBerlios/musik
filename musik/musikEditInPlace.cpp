///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikEditInPlace.h"
#include ".\musikeditinplace.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEditInPlace, CEdit)

///////////////////////////////////////////////////

int WM_MUSIKEDITCOMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
int WM_MUSIKEDITCANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );

///////////////////////////////////////////////////

CmusikEditInPlace::CmusikEditInPlace()
{
}

///////////////////////////////////////////////////

CmusikEditInPlace::~CmusikEditInPlace()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEditInPlace, CEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEditInPlace::Commit()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );

	if ( GetParent() )
	{
		int WM_MUSIKEDITCOMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
		GetParent()->SendMessage( WM_MUSIKEDITCOMMIT );
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::Cancel()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );

	if ( GetParent() )
	{
		int WM_MUSIKEDITCANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );
		GetParent()->SendMessage( WM_MUSIKEDITCANCEL );
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnEnKillfocus()
{
	Cancel();	
}

///////////////////////////////////////////////////

BOOL CmusikEditInPlace::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
		Commit();

	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
		Cancel();

	return CEdit::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	SetWindowText( m_Str );
	SetSel( 0, m_Str.GetLength() );
}

///////////////////////////////////////////////////
