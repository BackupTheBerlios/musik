/*
 *  GaugeSeek.cpp
 *
 *  Custom event to allow mouse drag seeking for
 *  (currently only) horizontal wxGauge objects.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "GaugeSeek.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h" 

BEGIN_EVENT_TABLE(CGaugeSeekEvt, wxEvtHandler)
	EVT_LEFT_DOWN			(CGaugeSeekEvt::OnLeftDown		) 
	EVT_LEFT_UP				(CGaugeSeekEvt::OnLeftUp		) 
	EVT_MOTION				(CGaugeSeekEvt::OnMouseMove		) 
	EVT_ERASE_BACKGROUND	( CGaugeSeekEvt::OnEraseBackground )
END_EVENT_TABLE()

CGaugeSeekEvt::CGaugeSeekEvt( wxGauge *parent, long style )
{
	pParent		= parent; 
	lType		= style; 
	m_LastPos	= -1;
	m_Dragging	= false;
}
void CGaugeSeekEvt::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{	
	// empty => no background erasing to avoid flicker
}

void CGaugeSeekEvt::OnLeftDown( wxMouseEvent& event )
{
	//-----------------------------------------------------//
	//--- we're using the seek bar AND music is playing ---//
	//--- OR we're adjusting volume                     ---//
	//-----------------------------------------------------//
	if ( event.LeftDown() )
	{
		m_Dragging = true;
		pParent->CaptureMouse();

		if ( lType == wxGA_HORIZONTAL )
			g_TimeSeeking = true;

		if ( ( lType == wxGA_HORIZONTAL && g_Player.IsPlaying() ) || lType == wxGA_VERTICAL )
			SetFromMousePos( event );
	}

	event.Skip();
}

void CGaugeSeekEvt::OnMouseMove( wxMouseEvent& event )
{
	if ( m_Dragging )
	{
		if ( event.LeftIsDown() )
		{
			if ( ( lType == wxGA_HORIZONTAL && g_Player.IsPlaying() ) || lType == wxGA_VERTICAL )
				SetFromMousePos( event );		
		}
	}

	event.Skip();
}

void CGaugeSeekEvt::OnLeftUp( wxMouseEvent& WXUNUSED(event) )
{
	if ( m_Dragging )
	{
		pParent->ReleaseMouse();

		if ( g_ActiveStreams.GetCount() )
		{
     		//-----------------------------------------------//
			//--- if we have left up and modifying time	  ---//
			//--- assume user is done. update pos		  ---//
			//-----------------------------------------------//   		
			if ( lType == wxGA_HORIZONTAL )
			{
				//--- set player pos ---//
				int nTime = ( (int)fPos * (int)g_Player.GetDuration( FMOD_SEC ) ) / (int)100;
				g_Player.SetTime( nTime );	
				g_TimeSeeking = false;
			}
		}
	}
}

void CGaugeSeekEvt::SetFromMousePos( wxMouseEvent& event )
{
	//--- stuff we'll need for calculation ---//
	wxPoint mousepos	= event.GetPosition();
	wxSize	wndsize		= pParent->GetSize();

	//---------------------------------------------//
	//--- if horizontal, we're seeking the song ---//
	//---------------------------------------------//
	if ( lType == wxGA_HORIZONTAL )
	{
		//--- get percent ---//
		int nSizeX	= wndsize.GetWidth();	
		int nMouseX = mousepos.x;

		//--- set value ---//
		fPos = (float)100* ( (float)nMouseX / (float)nSizeX );
		if ( fPos < 0.0f )
			fPos = 0.0f;
		else if ( fPos > 100.0f )
			fPos = 100.0f;

		int nThisPos = (int)fPos;
		if ( nThisPos != m_LastPos )
		{
			pParent->SetValue( nThisPos );
			m_LastPos = nThisPos;
		}

		//--- set string ---//
		int nCurrTime = (int)( (float)g_Player.GetDuration( FMOD_MSEC ) * (float)( fPos / (float)100 ) );
		g_NowPlayingCtrl->SetTime( MStoStr( nCurrTime ) );
	}

	//-----------------------------------------------//
	//--- if vertical, we're adjusting the volume ---//
	//-----------------------------------------------//
	if ( lType == wxGA_VERTICAL )
	{
		//--- get percent ---//
		int nSizeY	= wndsize.GetHeight();	
		int nMouseY = mousepos.y;

		//--- set value ---//
		fPos = (float)255* ( (float)nMouseY / (float)nSizeY );
		if ( fPos < 0.0f )
			fPos = 0.0f;
		else if ( fPos > 255.0f )
			fPos = 255.0f;

		int nThisPos = (int)fPos;
		if ( nThisPos != m_LastPos )
		{
			pParent->SetValue( 255 - (int)fPos );
			m_LastPos = nThisPos;
		}

		//--- set volume ---//
		g_Prefs.nSndVolume = (int)pParent->GetValue();

		if ( g_FaderThread->IsCrossfaderActive() )
			g_FaderThread->CrossfaderStop();

		g_Player.SetVolume();
	}
}
