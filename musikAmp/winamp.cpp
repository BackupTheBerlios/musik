///////////////////////////////////////////////////

// rewritten by Casey Langen for the musik project
// original work done by Saïd Bougribate aka KarLKoX,
// see information below...

///////////////////////////////////////////////////

/*  Contact :
	--------
	author	: Saïd Bougribate aka KarLKoX
	mail	: KarLKoX@ifrance.com
	url		: http://www.starnetasso.fr.st
			  http://www.multimania.com/xxkarlkoxxx
	Thanx	: BlackShard, Albert L Faber (author of Cdex in wich i ve taken some code/ideas) 
			  Brett Patterson (author of the famous FMOD Library) and DjMaze (for the Memory Optimisation)
*/

///////////////////////////////////////////////////

#include "stdafx.h"
#include "vis.h"
#include "buffer.h"
#include "fmod.h"
#include "winamp.h"

///////////////////////////////////////////////////

// mutex
#include "afxmt.h"
CMutex protect;

///////////////////////////////////////////////////

// callback used to fill pcmData so the vis can use it

void* F_CALLBACKAPI dsp_vis_callback(void *originalbuffer, void *newbuffer, int length, int param)
{
    int             count;									// our counter 
	int             totalblocks;							// don't exced the totalbock calculated from the buffersize
    signed short    *dest;									// destination pointer in wich we stock our data
    signed short	*src = (signed short *)newbuffer;		// source pointer from the data
	signed short	*srcleft, *srcright, *destleft, *destright;
	float *fft;	
	
	if ( Vis_Enable_Rendering )
	{
		fft = FSOUND_DSP_GetSpectrum();

		if  (!fft && !length) 
		{
			return 0;
		}

		srcleft   = ((signed short *)newbuffer);
		srcright  = ((signed short *)newbuffer)+1;
		destleft  = ((signed short *)param);
		destright = ((signed short *)param)+1;

		totalblocks = FSOUND_DSP_GetBufferLengthTotal() / FSOUND_DSP_GetBufferLength(); 
		dest = &pcmBuffer[iBufferOffset * FSOUND_DSP_GetBufferLength()];


		if ( VisMode == SPECTRUM)
		{
			float fMult;
			float power_spectrum[512+1];
			int		k;

			fMult = (float) 0x7FFF;

			if (fft)
			{
				dest[0] = (short)(fft[0]*fft[0]);
				for (k = 1; k < (512+1)/2; ++k)						// (k < N/2 rounded up) 
					power_spectrum[k] = fft[k] + fft[512-k];

				if (512 % 2 == 0) // N is even 
					power_spectrum[511/2] = fft[512/2]*fft[512/2];  // Nyquist freq.

				for (count=0; count < 512/2; count++)
				{			
					dest[count] = (short)(power_spectrum[count]*fMult);
				}
			}
		} 
		
		else 
		{
			for (count=0; count < length; count++)
			{
				//  (((int)src[count << 1] + (int)src[(count << 1) + 1]) >> 1);	  <-- TEST : 16 bits mono
				dest[count] = (signed short)(src[count]);						//<-- 16 bits stereo 
				
				// BETTER ?
				// dest[count] = (signed short)(srcleft[count] );
				// dest[count+1] = (signed short)(srcright[count] );
			}
		}

		iBufferOffset++;
		if ( iBufferOffset >= totalblocks )
			iBufferOffset = 0;
	}

	// the buffer is not altered, so keep it clean and return it
	return newbuffer;
}

///////////////////////////////////////////////////

// emulated winamp window

void Create_Winamp_Window()
{

	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style =  CS_PARENTDC | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = 0;
	wndclass.hCursor = (HICON) LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Winamp v1.x";
	wndclass.lpfnWndProc = WinampWndProc;
	wndclass.hIconSm = (HICON) NULL;

	// register the class
	if ( !RegisterClassEx( &wndclass ) )
		MessageBox( mainhwnd, "Winamp emulation window failed to register.", "Emulation", MB_ICONWARNING );

	// create the fake window
	vis_Window_Emu = CreateWindowEx(
					0,
					"Winamp v1.x", 
					"Winamp Emulation Window", 
					0,  //style
					0, 0, 256, 16, 
					(HWND) mainhwnd, (HMENU) NULL, NULL, NULL );


	if ( vis_Window_Emu )
		ShowWindow( vis_Window_Emu, SW_HIDE );
	else
		MessageBox(mainhwnd, "Winamp emulation class registered, but window failed to create", "Emulation", MB_ICONWARNING );
}

///////////////////////////////////////////////////

void Destroy_Winamp_Window()
{
	if ( vis_Window_Emu )
	{
		DestroyWindow(vis_Window_Emu);
		vis_Window_Emu  = 0;
		UnregisterClass("Winamp v1.x", NULL);
	}
}

///////////////////////////////////////////////////

// WindowProc responds to the Vis Plugin, sending keyboard 
// message to the app (according to the sdk)

LRESULT CALLBACK WinampWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
		case WM_GETTEXT: 
			{
				// if we're drawing and theres a song title,
				// copy it to the LPARAM, which I guess is a pointer
				// to something back in the plugin?
				if (Vis_Enable_Rendering && SongTitle[0] != '\0' )
				{
					memcpy( (LPTSTR)lParam, SongTitle, strlen(SongTitle) + 1 );
					return TRUE;
				}
			}
		
		case WM_USER:
			{
				switch(lParam)
				{

					case WA_USER_GETTIME:
					{
						if (wParam == 0) return Elapsed;	//temps courant
						if (wParam == 1) return Length;		//temps total
						break;
					}
					case WA_USER_GETVERSION:
					{
						return 0x2000;
						break;
					}
				
				}
			}

		// if the plugin requests our status,
		// tell it that we are playing...
		case WA_USER_STATUS:
			{
				return 1;
				break;
			}

		case WM_COMMAND:
			{
					/*switch(wParam)
					{
						case WINAMP_STOP:
						case WINAMP_STOP2:
						{

							// msg WINAMP_STOP à envoyer
							SetForegroundWindow(mainhwnd);
							SendMessage(mainhwnd, WINAMP_STOP, 0, 0);
							break;
						}
					}*/
					if (wParam == WINAMP_STOP)
						SendMessage(mainhwnd, WINAMP_STOP, 0, 0);
					break;
			}

		default:

			{
				int n = 0;
			}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);

}

///////////////////////////////////////////////////

// The main timer where we render the Data, it must be 
// well coded for speed reasons... interval is set from 
// the DelayMS member of the VisModule member

void CALLBACK vis_time_event(UINT uId, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	int			offset;		// position withing the buffer
	int			rendered;	// 0 on success
	signed short int *src;	// our buffer to render

	// we have at least ONE module 
	if ( module < 0 ) 
		module = 0;		
	
	if ( Vis_Enable_Rendering && gs_vWinAmpProps[vis_index].hDll != NULL )
	{
		protect.Lock();

		// the next pcmblock (iBufferOffset + 1) is the one that is audible.
		offset = (iBufferOffset + 1) * FSOUND_DSP_GetBufferLength();
		if (offset >= FSOUND_DSP_GetBufferLengthTotal())
		{
			offset -= FSOUND_DSP_GetBufferLengthTotal();
		}

		src = &pcmBuffer[offset];	// <-- TEST : seeking for speed improvement ...
	
		// convert the data to 8 bit then send it to the plugin
		Cnv16to8(&pcmBuffer[offset],(signed char*) &gs_vWinAmpProps[vis_index].pModule->getModule(module)->waveformData[0][0], vis_Plugin_Samples);
		Cnv16to8(&pcmBuffer[offset]+1,(signed char*) &gs_vWinAmpProps[vis_index].pModule->getModule(module)->waveformData[1][0], vis_Plugin_Samples);
	
		// render the data
 		rendered = gs_vWinAmpProps[vis_index].pModule->getModule(module)->Render(gs_vWinAmpProps[vis_index].pModule->getModule(module));
		
		protect.Unlock();
	}
}

///////////////////////////////////////////////////

// vis thread initialization

HANDLE Vis_New_Thread_Init()
{
	// pass the index plugin to the thread
	Vis_hThread = CreateThread(0, 0, Vis_New_Thread, (void*)vis_index, 0, &dwVis_hThreadId);
	if (!Vis_hThread)
	{
		MessageBox(mainhwnd,"Thread creation failed !","Error !!", MB_ICONEXCLAMATION | MB_ICONWARNING);
		return 0;
	}

	CloseHandle(Vis_hThread);

	return Vis_hThread;

}

DWORD WINAPI Vis_New_Thread(LPVOID lpParam)
{
	MSG  message;
	UINT delay_ms;					// clip delay
	INT	 vis_timer;					// the new vis timer
	BOOL msg_return_value;			// check for specific message 

	vis_index = (int) lpParam;
	
	if ( vis_index != -1 && gs_vWinAmpProps[vis_index].hDll != NULL )
	{
		SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_LOWEST );
	
		// First : create the winamp window
		Destroy_Winamp_Window();
		Create_Winamp_Window();

		// Second : determine vis mode
		protect.Lock();

		if ( gs_vWinAmpProps[vis_index].pModule->getModule(module)->waveformNch > 0 )
			VisMode = OSCILLOSCOPE;
		else if ( gs_vWinAmpProps[vis_index].pModule->getModule(module)->spectrumNch > 0 )
			VisMode = SPECTRUM;

		gs_vWinAmpProps[vis_index].pModule->getModule(module)->hwndParent = vis_Window_Emu;
		gs_vWinAmpProps[vis_index].pModule->getModule(module)->Init(gs_vWinAmpProps[vis_index].pModule->getModule(module));

		delay_ms = gs_vWinAmpProps[vis_index].pModule->getModule(module)->delayMs;
	
		// set clip delay: it seems that setting minimum 
		// to 50 is the good value, but the SDK says 25 ...
		if (delay_ms < 25) 
			delay_ms = 25;


		// Third : create a new timer where we render the data
		timeBeginPeriod(250);
		vis_timer = timeSetEvent( delay_ms, 250, vis_time_event, (DWORD) NULL, TIME_PERIODIC );
		
		if ( !vis_timer)
			MessageBox(mainhwnd, "Failed to create necessary visualization timer.", "Emulation", MB_ICONWARNING);

		protect.Unlock();
	

		// unlock the mutex and loop the thread
		do
		{
			msg_return_value = GetMessage(&message, NULL, 0, 0);

    		TranslateMessage( &message );
      		DispatchMessage( &message );
  		}	
		while ( msg_return_value > 0 && GetVisHwnd() );

		// kill the timer
		timeKillEvent(vis_timer);
		timeEndPeriod(250);

		Vis_Enable_Rendering = 0;

		// lock down for the cleanup
		protect.Lock();
		if ( gs_vWinAmpProps[vis_index].hDll )
			gs_vWinAmpProps[vis_index].pModule->getModule(module)->Quit(gs_vWinAmpProps[vis_index].pModule->getModule(module));
	
		// deactivate dsp
		FSOUND_DSP_SetActive( DSP, FALSE );
		DSP = 0;

		// the vis window is no longer necessary
		Destroy_Winamp_Window();

		// 'null'ing our vars
		theThread		=	0;
		Vis_hThread		=	0;
		dwVis_hThreadId =	0;
		free(pcmBuffer);

		// and free memory used 
		FreeVisInfo();
		FreeVis(vis_index);

		protect.Unlock();
	}
				
	return 0;

}


//////////////////////////////
// VIS_STUFFS				//
//////////////////////////////

void WINEXPORT SetHwnd(HWND hwnd)
{
	mainhwnd = hwnd;
}

HWND WINEXPORT GetVisHwnd(void)
{
	HWND visHwnd;

	visHwnd = vis_Window_Emu;

	return visHwnd;

	return 0;
}

void WINEXPORT SetSongTitle(LPTSTR TheTitle)
{
	strcpy( ( char* )SongTitle, TheTitle );
}

void WINEXPORT SetElapsed(int elapsed)
{
	Elapsed = elapsed;
}

void WINEXPORT SetLength(int length)
{
	Length = length;
}

void WINEXPORT IsPlaying(int playing)
{
	protect.Lock();
	Vis_Enable_Rendering = playing;
	protect.Unlock();
}

void WINEXPORT SetVisModule(int the_module)
{
	module = the_module;
}

LPSTR WINEXPORT GetVisInfo(int i )
{	
	LPSTR strRet;

	protect.Lock();
	if ( gs_vWinAmpProps[i].pModule->getModule(0)->description != NULL )
		strRet = gs_vWinAmpProps[i].pModule->description;
	protect.Unlock();

	return strRet;
}

UINT WINEXPORT GetVisCount()
{
	int i = 0;
	protect.Lock();
	while  (gs_vWinAmpProps[i].pModule)
		i++; 
	protect.Unlock();

	return i;
}

UINT WINEXPORT GetVisModuleCount(int i)
{
	protect.Lock();
	int ret = gs_vWinAmpProps[i].NumberOfModules;
	protect.Unlock();

	return ret;

}

LPSTR WINEXPORT GetVisModuleInfo(int plugin, int the_module)
{
	LPTSTR ModuleInfo;
	
	protect.Lock();
	ModuleInfo = gs_vWinAmpProps[plugin].pModule->getModule(the_module)->description;
	protect.Unlock();

	return ModuleInfo;

}

void WINEXPORT Config_Vis(int i, int module_index)
{
	LoadVis(i);

	protect.Lock();
	gs_vWinAmpProps[i].pModule->getModule(module_index)->Config(gs_vWinAmpProps[i].pModule->getModule(module_index));	
	protect.Unlock();
	
	FreeVis(i);
}


void WINEXPORT Start_Vis( int i )
{
	// make sure to unload old vis
	if ( i != vis_index )
	{
		Stop_Vis( vis_index );
		vis_index = i;
	}

	// load it
	LoadVis( vis_index );

	// prepare samples and buffer
	protect.Lock();

	vis_Plugin_Samples	= SINGLE_BUFFER_SIZE >> 1;

	if ( vis_Plugin_Samples > 1152) 
		vis_Plugin_Samples = 1152;

	pcmBuffer = (signed short*)calloc( FSOUND_DSP_GetBufferLengthTotal() + 16, 2 );

	// initialize DSP
	DSP = FSOUND_DSP_Create(&dsp_vis_callback, FSOUND_DSP_DEFAULTPRIORITY_USER, (int)pcmBuffer );
	if (!FSOUND_DSP_GetActive(DSP) )
		FSOUND_DSP_SetActive(DSP, TRUE);

	protect.Unlock();
	
	// startup the new thread
	theThread = Vis_New_Thread_Init();
	
	if ( !theThread )
		MessageBox( mainhwnd, "Visualization thread failed to spawn.","Emulation", MB_ICONEXCLAMATION | MB_ICONWARNING);
}


void WINEXPORT Stop_Vis( int n )
{
	if ( dwVis_hThreadId ) 
		PostThreadMessage(dwVis_hThreadId, WM_QUIT, 0, 0);
}


/*----------------------------------------------------------------------------------

Free all plugins

----------------------------------------------------------------------------------*/
void Release_Vis_Plugin(int cnt)
{
	protect.Lock();
	if ( gs_vWinAmpProps[cnt].pModule->getModule(0)->hDllInstance != 0 )
		FreeLibrary(gs_vWinAmpProps[cnt].pModule->getModule(0)->hDllInstance);
	protect.Unlock();
}

bool WINEXPORT LoadVisPlugins(LPCTSTR path)
{
	protect.Lock();

	static int				currplug=0;
	WIN32_FIND_DATA			sFF = {0};
	HANDLE					hFind = NULL;
	CString					m_path, m_fullpath;
	WINAMPPLUGINPROPVIS		tmpPropVis;
	winampVisModule			*pVis = NULL;

	memset( &pVis, 0 , sizeof( pVis ) );
	m_path += path;
	m_path += "\\vis_*.dll";
	hFind = FindFirstFile(m_path, &sFF); 	

	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	do 
	{
		m_fullpath = path;
		m_fullpath += sFF.cFileName;

		tmpPropVis.hDll = NULL;				
		tmpPropVis.hDll = LoadLibrary(m_fullpath);

		if (tmpPropVis.hDll)
		{
			int cnt = 0;

			WINAMPGETVISMODULE pGetMod = (WINAMPGETVISMODULE)
			GetProcAddress(	tmpPropVis.hDll,
							"winampVisGetHeader" );

			tmpPropVis.pModule  = pGetMod();

			// set the file name
			tmpPropVis.strFileName = m_fullpath;
			pVis = tmpPropVis.pModule->getModule(0);
			pVis->hDllInstance = tmpPropVis.hDll;
			pVis->hwndParent  = mainhwnd;
			pVis->sRate = 44100;
			pVis->nCh = 2;

			while ( tmpPropVis.pModule->getModule(cnt) > 0 )
				cnt++;

			gs_vWinAmpProps[currplug] = tmpPropVis;
			gs_vWinAmpProps[currplug].NumberOfModules = cnt;
			currplug++;

		} 
        else 
			return FALSE;
	} 
	while ( FindNextFile( hFind, &sFF ) );

	FindClose( hFind );

	protect.Unlock();

	return TRUE;
}



void WINEXPORT FreeVisInfo()
{
	protect.Lock();
	for ( int cnt=0; cnt < (int)GetVisCount(); cnt++ )
		FreeLibrary( gs_vWinAmpProps[cnt].hDll );
	protect.Unlock();
}

void WINEXPORT FreeVis(int i)
{
	protect.Lock();
	FreeLibrary(gs_vWinAmpProps[i].hDll);
	protect.Unlock();
}

void WINEXPORT LoadVis(int i)
{
	LoadWinampPlugin(gs_vWinAmpProps[i].strFileName, i);
}

void  LoadWinampPlugin(LPCSTR path, int currplug)
{
	protect.Lock();

	winampVisModule		*pVis;
	WINAMPPLUGINPROPVIS	tmpPropVis;	
	int					cnt;

	cnt = 0;
	pVis = NULL;
	tmpPropVis.hDll = LoadLibrary( path );

		if ( tmpPropVis.hDll )
		{
			WINAMPGETVISMODULE pGetMod = ( WINAMPGETVISMODULE )
				GetProcAddress(	tmpPropVis.hDll,
								"winampVisGetHeader" );
				
			tmpPropVis.pModule  = pGetMod();

			// set the file name
			tmpPropVis.strFileName = path;
			pVis = tmpPropVis.pModule->getModule(0);
			pVis->hDllInstance = tmpPropVis.hDll;					
			pVis->hwndParent  = mainhwnd;								
			pVis->sRate = 44100;
			pVis->nCh = 2;


			while ( tmpPropVis.pModule->getModule(cnt) > 0 )
				cnt++;

			gs_vWinAmpProps[currplug] = tmpPropVis;
			gs_vWinAmpProps[currplug].NumberOfModules = cnt;
		}

	protect.Unlock();

}


/*----------------------------------------------------------------------------------

convert from 16 to 8 bits

----------------------------------------------------------------------------------*/
long Cnv16to8 (signed short *source,signed char *dest,unsigned long samples)
{
	unsigned long i;

	for (i=0;i<samples;i=i+2)
	{
		dest[i>>1]=source[i]>>8;
	}

	return i;
}

