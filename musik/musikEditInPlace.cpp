///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikEditInPlace
//
// Filename(s): 
//
//   musikEditInPlace.h, musikEditInPlace.cpp
//
// Information:
//
//   musikCube's EditInPlace control for CmusikListCtrls
// 
// Usage: 
//
//   See retagging stuff in CmusikSelectionCtrl.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikEditInPlace.h"

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
	m_IsVisible = false;

	if ( GetParent() )
		GetParent()->SendMessage( WM_MUSIKEDITCOMMIT, (WPARAM)this, (LPARAM)m_Args );
}

///////////////////////////////////////////////////

void CmusikEditInPlace::Cancel()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );
	m_IsVisible = false;

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
		m_IsVisible = true;
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnEnChange()
{
	if ( GetParent() )
		GetParent()->SendMessage( WM_MUSIKEDITCHANGE, (WPARAM)this, (LPARAM)m_Args );
}

///////////////////////////////////////////////////

bool CmusikEditInPlace::IsVisible()
{
	return m_IsVisible;
}

///////////////////////////////////////////////////