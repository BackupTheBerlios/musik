/*
 *  GaugeSeek.cpp
 *
 *  Custom event to allow mouse drag seeking for
 *  (currently only) horizontal wxGauge objects.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
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
	EVT_LEAVE_WINDOW		(CGaugeSeekEvt::OnLeaveWindow	) 
END_EVENT_TABLE()

void CGaugeSeekEvt::OnLeftDown( wxMouseEvent& event )
{
	//-----------------------------------------------------//
	//--- we're using the seek bar AND music is playing ---//
	//--- OR we're adjusting volume                     ---//
	//-----------------------------------------------------//
	if ( lType == wxGA_HORIZONTAL )
		g_TimeSeeking = true;

	if ( ( lType == wxGA_HORIZONTAL && g_Player.IsPlaying() ) || lType == wxGA_VERTICAL )
		SetFromMousePos( event );

	event.Skip();

}

void CGaugeSeekEvt::OnMouseMove( wxMouseEvent& event )
{
	if ( event.LeftIsDown() )
	{
		if ( ( lType == wxGA_HORIZONTAL && g_Player.IsPlaying() ) || lType == wxGA_VERTICAL )
			SetFromMousePos( event );		
	}

	event.Skip();
}

void CGaugeSeekEvt::OnLeftUp( wxMouseEvent& event )
{
	if ( !event.LeftIsDown() && g_ActiveStreams.GetCount() )
	{
	//-----------------------------------------------//
	//--- if we have left up and modifying volume ---//
	//--- assume user is done. destroy the frame  ---//
	//-----------------------------------------------//
		if ( lType == wxGA_VERTICAL )
		{
		g_VolumeFrame->Close();
		}

     	//-----------------------------------------------//
		//--- if we have left up and modifying time	  ---//
		//--- assume user is done. update pos		  ---//
		//-----------------------------------------------//   		
		else if ( lType == wxGA_HORIZONTAL )
		{
			//--- set player pos ---//
			int nTime = ( (int)fPos * (int)g_Player.GetDuration( FMOD_SEC ) ) / (int)100;
			g_Player.SetTime( nTime );	
			g_TimeSeeking = false;
		}
	}	
}

void CGaugeSeekEvt::OnLeaveWindow( wxMouseEvent& WXUNUSED(event) )
{
	if ( g_TimeSeeking )
		g_TimeSeeking = false;
}

void CGaugeSeekEvt::SetFromMousePos( wxMouseEvent& WXUNUSED(event) )
{
	//--- stuff we'll need for calculation ---//
	wxPoint mousepos	= pParent->ScreenToClient( wxGetMousePosition() );
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
		int nThisPos = (int)fPos;

		if ( nThisPos != m_LastPos )
		pParent->SetValue( (int)fPos );
		m_LastPos = nThisPos;

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
		pParent->SetValue( 255 - (int)fPos );

		//--- set volume ---//
		g_Prefs.nSndVolume = (int)pParent->GetValue();

		if ( g_FaderThread->IsCrossfaderActive() )
			g_FaderThread->CrossfaderStop();

		g_Player.SetVolume();
	}
}
