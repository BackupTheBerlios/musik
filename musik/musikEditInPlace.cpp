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
int WM_MUSIKEDITCHANGE = RegisterWindowMessage( "MUSIKEDITCHANGE" );

///////////////////////////////////////////////////

CmusikEditInPlace::CmusikEditInPlace()
{
	m_IsVisible = false;
	m_Args = NULL;
}

///////////////////////////////////////////////////

CmusikEditInPlace::~CmusikEditInPlace()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEditInPlace, CEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_SHOWWINDOW()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEditInPlace::Commit()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );

	if ( GetParent() )
		GetParent()->SendMessage( WM_MUSIKEDITCOMMIT, (WPARAM)this, (LPARAM)m_Args );
}

///////////////////////////////////////////////////

void CmusikEditInPlace::Cancel()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );

	if ( GetParent() )
		GetParent()->SendMessage( WM_MUSIKEDITCANCEL, (WPARAM)this, (LPARAM)m_Args );
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
	{
		Commit();
		return TRUE;
	}

	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		Cancel();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		SetWindowText( m_Str );
		SetSel( 0, m_Str.GetLength() );
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnEnChange()
{
	if ( GetParent() )
		GetParent()->SendMessage( WM_MUSIKEDITCHANGE, (WPARAM)this, (LPARAM)m_Args );
}

///////////////////////////////////////////////////
