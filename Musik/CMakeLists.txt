
PROJECT(wxMusik)

# Configure for a wxWindows build
INCLUDE(${CMAKE_ROOT}/Modules/FindwxWindows.cmake)

IF(WXWINDOWS_INCLUDE_PATH)
    INCLUDE_DIRECTORIES(${WXWINDOWS_INCLUDE_PATH})
ENDIF(WXWINDOWS_INCLUDE_PATH)

IF(CMAKE_WX_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
ENDIF(CMAKE_WX_CXX_FLAGS)

# ogg-vorbis library
FIND_LIBRARY(OGG_LIBRARY NAMES ogg PATHS /usr/lib /usr/local/lib)

# SQLite library
FIND_LIBRARY(SQLITE_LIBRARY NAMES sqlite PATHS /usr/lib /usr/local/lib)

# id3 library
FIND_LIBRARY(ID3_LIBRARY NAMES id3 PATHS /usr/lib /usr/local/lib)

# fmod library
FIND_LIBRARY(FMOD_LIBRARY NAMES fmod PATHS /usr/lib /usr/local/lib)

# Vorbis library
FIND_LIBRARY(VORBIS_LIBRARY NAMES vorbis PATHS /usr/lib /usr/local/lib)

ADD_EXECUTABLE(wxMusik
        MusikApp
        MusikGlobals
        MusikUtils
		MusikAboutDlg
        ${wxMusik_SOURCE_DIR}/Classes/ActivityBox
        ${wxMusik_SOURCE_DIR}/Classes/ActivityAreaCtrl
        ${wxMusik_SOURCE_DIR}/Classes/BtnDown
        ${wxMusik_SOURCE_DIR}/Classes/GaugeSeek
        ${wxMusik_SOURCE_DIR}/Classes/MusikLibrary
        ${wxMusik_SOURCE_DIR}/Classes/MusikPlayer
        ${wxMusik_SOURCE_DIR}/Classes/MusikPrefs
        ${wxMusik_SOURCE_DIR}/Classes/NowPlayingCtrl
        ${wxMusik_SOURCE_DIR}/Classes/MusikListCtrl
        ${wxMusik_SOURCE_DIR}/Classes/PlaylistCtrl
		${wxMusik_SOURCE_DIR}/Classes/PlaylistInfoCtrl
        ${wxMusik_SOURCE_DIR}/Classes/SourcesBox
        ${wxMusik_SOURCE_DIR}/Classes/WebServer
        ${wxMusik_SOURCE_DIR}/Classes/MusikFX
        ${wxMusik_SOURCE_DIR}/Classes/MusikEQCtrl
        ${wxMusik_SOURCE_DIR}/Classes/MusikEQGauge
        ${wxMusik_SOURCE_DIR}/Classes/Tunage
        ${wxMusik_SOURCE_DIR}/Classes/PictureBox

        ${wxMusik_SOURCE_DIR}/Classes/Library/CFrameHeader
        ${wxMusik_SOURCE_DIR}/Classes/Library/CMP3Info
        ${wxMusik_SOURCE_DIR}/Classes/Library/COggInfo
        ${wxMusik_SOURCE_DIR}/Classes/Library/CVBitRate
        ${wxMusik_SOURCE_DIR}/Classes/Library/vcedit

        ${wxMusik_SOURCE_DIR}/Frames/MusikFrame
        ${wxMusik_SOURCE_DIR}/Frames/MusikFrameEvents
        ${wxMusik_SOURCE_DIR}/Frames/MusikLibraryFrame
        ${wxMusik_SOURCE_DIR}/Frames/MusikPrefsFrame
        ${wxMusik_SOURCE_DIR}/Frames/MusikTagFrame
        ${wxMusik_SOURCE_DIR}/Frames/MusikVolumeFrame
        ${wxMusik_SOURCE_DIR}/Frames/MusikFXFrame
		${wxMusik_SOURCE_DIR}/Frames/MusikAutoTaggerFrame
 		${wxMusik_SOURCE_DIR}/Threads/ActivityBoxThreads
        ${wxMusik_SOURCE_DIR}/Threads/PlaylistCtrlThreads
        ${wxMusik_SOURCE_DIR}/Threads/MusikLibraryThreads
        ${wxMusik_SOURCE_DIR}/Threads/MusikTagThreads
        ${wxMusik_SOURCE_DIR}/Threads/MusikThreads

        ${wxMusik_SOURCE_DIR}/shibatch/Equ.cpp
        ${wxMusik_SOURCE_DIR}/shibatch/Fftsg_fl.cpp
    )

TARGET_LINK_LIBRARIES(wxMusik 
        ${WXWINDOWS_LIBRARY} 
        ${OGG_LIBRARY}
        ${SQLITE_LIBRARY}
        ${ID3_LIBRARY}
        ${FMOD_LIBRARY}
        ${VORBIS_LIBRARY}
    )

# Set build variables
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi ")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -pipe -fomit-frame-pointer")

SET(MyExecutable_PATH ${EXECUTABLE_OUTPUT_PATH})

IF(NOT MyExecutable_PATH)
  SET(MyExecutable_PATH ${CMAKE_CURRENT_BINARY_DIR})

ENDIF(NOT MyExecutable_PATH)

IF(APPLE)
  FIND_PROGRAM(VTK_APPLE_RESOURCE Rez /Developer/Tools)
  IF(VTK_APPLE_RESOURCE)
    ADD_CUSTOM_COMMAND(SOURCE wxMusik
                       COMMAND ${VTK_APPLE_RESOURCE}
                       ARGS -d __DARWIN__ -t APPL Carbon.r /usr/local/lib/libwx_mac-2.5.1.r -o
                       ${MyExecutable_PATH}/wxMusik
                       TARGET wxMusik)
  ENDIF(VTK_APPLE_RESOURCE)
ENDIF(APPLE)

# Set install options
INSTALL_TARGETS(/usr/local/bin wxMusik)

