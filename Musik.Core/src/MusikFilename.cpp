///////////////////////////////////////////////////

#include "stdafx.h"

#include "MusikFilename.h"

///////////////////////////////////////////////////

CMusikFilename::CMusikFilename()
{
}

///////////////////////////////////////////////////

CMusikFilename::CMusikFilename( const CStdString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CMusikFilename::~CMusikFilename()
{
}

///////////////////////////////////////////////////

void CMusikFilename::SetFilename( const CStdString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetPathSeparator()
{
	return (CStdString)MUSIK_PATH_SEPARATOR;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetJustFilename()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Right( m_Filename.GetLength() - nPos - 1 );
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetFullFilename()
{
	return m_Filename;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetExtension( bool make_lower )
{
	int nPos = m_Filename.ReverseFind( "." );
	CStdString ret = m_Filename.Right( m_Filename.GetLength() - nPos - 1 );

	if ( make_lower )
		ret.MakeLower();

	return ret;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetPath()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Left( nPos + 1 );
}

///////////////////////////////////////////////////