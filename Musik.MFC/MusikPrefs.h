#pragma once

#include "stdafx.h"
#include "CIniEx/iniFile.h"

#include "../Musik.Core/include/MusikArrays.h"

using namespace std;

class CMusikPrefs
{
public:
	CMusikPrefs( CString filename );
	~CMusikPrefs();

	void LoadPrefs();
	void SavePrefs();
	//-----------------------------------------------------//
	//--- whole dialog									---//
	//-----------------------------------------------------//
	CSize	GetDlgSize	()						{ return m_Dlg_Size; }
	CPoint	GetDlgPos	()						{ return m_Dlg_Pos; }

	void	SetDlgSize	( const CSize size )	{ m_Dlg_Size = size; }
	void	SetDlgPos	( const CPoint pt )		{ m_Dlg_Pos = pt; }

	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	size_t	GetSelBoxCount()					{ return m_SelectionBox_Count; }

	void	SetSelBoxCount( size_t n )			{ m_SelectionBox_Count = n; }

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//
	CIntArray	GetPlaylistOrder()				{ return m_Playlist_Order; }
	CIntArray	GetPlaylistSizes()				{ return m_Playlist_Sizes; }
	int			GetPlaylistCol( int n )			{ return m_Playlist_Order.at( n ); }
	int			GetPlaylistColWidth( int n )	{ return m_Playlist_Sizes.at( n ); }
	size_t		GetPlaylistColCount()			{ return m_Playlist_Order.size(); }
	COLORREF	GetPlaylistStripeColor()		{ return m_Playlist_Stripe_Color; }
	
	void		SetPlaylistOrder( CIntArray n )		{ m_Playlist_Order = n; }	
	void		SetPlaylistSizes( CIntArray n )		{ m_Playlist_Sizes = n; }
	void		SetPlaylistStripColor( COLORREF c )	{ m_Playlist_Stripe_Color = c; }
	
	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- now playing									---//
	//-----------------------------------------------------//
	int		GetNowPlayingHeight()				{ return m_NowPlaying_Height; }

protected:
	CIniFile* config;
	string GetDefPlaylistOrder();
	string GetDefPlaylistSizes();
	string GetDefPlaylistStripeColor();

private:
	//-----------------------------------------------------//
	//--- whole dialog									---//
	//-----------------------------------------------------//
	CSize	m_Dlg_Size;
	CPoint	m_Dlg_Pos;

	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	size_t	m_SelectionBox_Count;

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//
	CIntArray m_Playlist_Order;
	CIntArray m_Playlist_Sizes;
	COLORREF m_Playlist_Stripe_Color;

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- now playing									---//
	//-----------------------------------------------------//
	int		m_NowPlaying_Height;
};