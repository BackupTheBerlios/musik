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

#ifndef _WINAMP_H_
#define _WINAMP_H_

///////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////

#define			WA_USER_GETTIME    105								
#define			WA_USER_STATUS     104
#define			WA_USER_GETVERSION 0
#define			WINAMP_STOP		   40047
#define			WINAMP_STOP2	   40147

#define BUFFERSIZE			( 44100 * 25 ) / 1000
#define SINGLE_BUFFER_SIZE  BUFFERSIZE << 2

enum VIS_MODE
{
	OSCILLOSCOPE,
	SPECTRUM
};

///////////////////////////////////////////////////

// these prototypes are not needed, but declared
// anyway

LRESULT CALLBACK WinampWndProc( HWND, UINT, WPARAM, LPARAM );
static void CALLBACK vis_time_event( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );
long Cnv16to8 ( signed short *, signed char *, unsigned long );

///////////////////////////////////////////////////

// general variables

#define	WINEXPORT __declspec(dllexport) WINAPI		// Export Declaration
#define	MAX_PLUGINS 128								// Max Plugins We Can Handle

HWND	mainhwnd;									// The Hwnd Of The Parent Window
char	SongTitle[256] = "\0";						// SongTitle Returned By The WM_GETTEXT Message
int		Elapsed,Length;								// Values Returned By The Winamp Command Messages

///////////////////////////////////////////////////

// vis export symbols

typedef winampVisHeader*	(*WINAMPGETVISMODULE)( void );
WINAMPPLUGINPROPVIS			gs_vWinAmpProps[MAX_PLUGINS];

///////////////////////////////////////////////////

// variables

static HANDLE	Vis_hThread = NULL;					// handle to the visual thread
HANDLE			theThread = NULL;			 
DWORD			dwVis_hThreadId = NULL;

int	VisMode;										// return one of the previous enum : 0 = oscillo, 1 = spectrum
int	vis_index;									// global index of the module
int	module = 0;										// wich module to load

UINT			Vis_Enable_Rendering = 0;			// a flag to check if we should render
HWND			vis_Window_Emu = NULL;				// the hWnd of the fake winamp window (for visual plugins)

UINT					vis_Plugin_Samples;
FSOUND_DSPUNIT			*DSP;						// handle to the DSP callback
static signed short		*pcmBuffer = NULL;
static int				iBufferOffset	= 0;		// offset in the buffer

///////////////////////////////////////////////////

// external vis prototypes

bool	WINEXPORT LoadVisPlugins(LPCTSTR path);
void	WINEXPORT SetVisModule(int );
void	WINEXPORT Start_Vis(int );
void	WINEXPORT Stop_Vis(int);
void	WINEXPORT Config_Vis(int , int );
void	WINEXPORT LoadVis(int );
void	WINEXPORT FreeVis(int );
void	WINEXPORT FreeVisInfo(void);
LPSTR	WINEXPORT GetVisInfo(int );
UINT	WINEXPORT GetVisCount(void);
UINT	WINEXPORT GetVisModuleCount(int );
LPSTR	WINEXPORT GetVisModuleInfo(int , int );

// internal vis prototypes
void	LoadWinampPlugin(LPCSTR path, int currplug);
void	Release_Vis_Plugin(int );
HANDLE	Vis_New_Thread_Init();
DWORD	WINAPI Vis_New_Thread(LPVOID );

///////////////////////////////////////////////////

// fake winamp window

void	Create_Winamp_Window(void );
void	Destroy_Winamp_WindowB(void );

///////////////////////////////////////////////////

// misc external prototypes

void WINEXPORT  SetHwnd(HWND );
HWND WINEXPORT  GetVisHwnd(void);
void WINEXPORT  SetSongTitle(LPTSTR );
void WINEXPORT  SetElapsed(int );
void WINEXPORT  SetLength(int );
void WINEXPORT  IsPlaying(int );
LPSTR WINEXPORT GetWinampInputDescription(int );
LPSTR WINEXPORT GetWinampInputExtensions(int );

///////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////