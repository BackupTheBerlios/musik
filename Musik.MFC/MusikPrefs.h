///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "stdafx.h"

#include "CIniEx/iniFile.h"

#include "../Musik.Core/include/MusikArrays.h"

///////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////

class CMusikPrefs
{
public:
	CMusikPrefs( CString filename );
	~CMusikPrefs();

	void		LoadPrefs();
	void		SavePrefs();

	// main dialog (gets)
	CSize		GetDlgSize	()								{ return m_Dlg_Size; }
	CPoint		GetDlgPos	()								{ return m_Dlg_Pos; }
	bool		IsMaximized ()								{ return m_Dlg_Maximized; }

	// main dialog (sets)
	void		SetDlgSize	( const CSize size )			{ m_Dlg_Size = size; }
	void		SetDlgPos	( const CPoint pt )				{ m_Dlg_Pos = pt; }
	void		SetMaximized( bool max )					{ m_Dlg_Maximized = max; }

	// selection area (gets)
	size_t		GetSelBoxCount()							{ return m_SelectionBox_Count; }

	// selection area (sets)
	void		SetSelBoxCount( size_t n )					{ m_SelectionBox_Count = n; }

	// playlist (gets)
	CIntArray	GetPlaylistOrder()							{ return m_Playlist_Order; }
	CIntArray	GetPlaylistSizes()							{ return m_Playlist_Sizes; }
	int			GetPlaylistCol( int n )						{ return m_Playlist_Order.at( n ); }
	int			GetPlaylistColWidth( int n )				{ return m_Playlist_Sizes.at( n ); }
	size_t		GetPlaylistColCount()						{ return m_Playlist_Order.size(); }
	COLORREF	GetPlaylistStripeColor()					{ return m_Playlist_Stripe_Color; }
	
	// playlist (sets)
	void		SetPlaylistOrder( CIntArray n )				{ m_Playlist_Order = n; }	
	void		SetPlaylistSizes( CIntArray n )				{ m_Playlist_Sizes = n; }
	void		SetPlaylistStripColor( COLORREF c )			{ m_Playlist_Stripe_Color = c; }
	
	// now playing (gets)
	int			GetNowPlayingHeight()						{ return m_NowPlaying_Height; }
	int			GetNowPlayingTitleFontSize()				{ return m_NowPlaying_TitleFont; }
	int			GetNowPlayingAlbumArtistFontSize()			{ return m_NowPlaying_AlbumArtistFont; }
	bool		IsNowPlayingTitleFontBold()					{ return m_NowPlaying_IsTitleFontBold; }
	bool		IsNowPlayingAlbumArtistFontBold()			{ return m_NowPlaying_IsAlbumArtistFontBold; }

	// now playing (sets)
	void		SetNowPlayingTitleFontSize( int y )			{ m_NowPlaying_TitleFont = y; }
	void		SetNowPlayingAlbumArtistFontSize( int y )	{ m_NowPlaying_AlbumArtistFont = y; }
	void		SetNowPlayingTitleFontBold( bool b )		{ m_NowPlaying_IsTitleFontBold = b; }
	void		SetNowPlayingAlbumArtistFontBold( bool b )	{ m_NowPlaying_IsAlbumArtistFontBold = b; }

	// player (gets)
	int			GetPlayerDriver()							{ return m_Player_Driver; }					
	int			GetPlayerDevice()							{ return m_Player_Device; }
	int			GetPlayerRate()								{ return m_Player_Rate; }
	int			GetPlayerMaxChannels()						{ return m_Player_Max_Channels; }

	// player (sets)
	void		SetPlayerDrive( int driver )				{ m_Player_Driver = driver; }
	void		SetPlayerDevice( int device )				{ m_Player_Device = device; }
	void		SetPlayerRate( int rate )					{ m_Player_Rate = rate; }
	void		SetPlayerMaxChannels( int max )				{ m_Player_Max_Channels = max; }

protected:
	CIniFile* config;
	string GetDefPlaylistOrder();
	string GetDefPlaylistSizes();
	string GetDefPlaylistStripeColor();

private:

	// main dialog
	CSize	m_Dlg_Size;
	CPoint	m_Dlg_Pos;
	bool	m_Dlg_Maximized;

	// selection area
	size_t	m_SelectionBox_Count;

	// playlist
	CIntArray m_Playlist_Order;
	CIntArray m_Playlist_Sizes;
	COLORREF m_Playlist_Stripe_Color;

	// now playing
	int	m_NowPlaying_Height;
	int m_NowPlaying_TitleFont;
	int m_NowPlaying_AlbumArtistFont;
	bool m_NowPlaying_IsTitleFontBold;
	bool m_NowPlaying_IsAlbumArtistFontBold;

	// player
	int m_Player_Device;
	int m_Player_Driver;
	int m_Player_Rate;
	int m_Player_Max_Channels;
};

///////////////////////////////////////////////////