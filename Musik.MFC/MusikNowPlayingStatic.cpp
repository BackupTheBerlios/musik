///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikNowPlayingStatic.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CNowPlayingStatic, CStatic)

///////////////////////////////////////////////////

CNowPlayingStatic::CNowPlayingStatic()
{
}

///////////////////////////////////////////////////

CNowPlayingStatic::~CNowPlayingStatic()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CNowPlayingStatic, CStatic)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CNowPlayingStatic::SetDynText( const CString& str )
{
	SetWindowText( str );
	UpdateDynSize();
}

///////////////////////////////////////////////////

void CNowPlayingStatic::UpdateDynSize()
{
	// get text
	CString str;
	GetWindowText( str );

	// get text extent
	CSize szText;
	HDC	hMemDC	= NULL;
	hMemDC = CreateCompatibleDC(NULL);
	if ( hMemDC )
	{
		CDC* pDC = CDC::FromHandle( hMemDC );
		if ( pDC )
		{
			CFont* pOldFont = pDC->SelectObject( this->GetFont() );
			szText = pDC->GetTextExtent( (LPCTSTR)str, str.GetLength() );
			pOldFont = pDC->SelectObject( pOldFont );
		}
	}

	// resize the window
	MoveWindow( CRect( CPoint( 0, 0 ), szText ) );
}

///////////////////////////////////////////////////