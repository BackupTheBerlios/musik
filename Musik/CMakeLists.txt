
PROJECT(Musik)

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

ADD_EXECUTABLE(Musik
        MusikApp
        MusikGlobals
        MusikUtils

        ${Musik_SOURCE_DIR}/Classes/ActivityBox
        ${Musik_SOURCE_DIR}/Classes/ActivityAreaCtrl
        ${Musik_SOURCE_DIR}/Classes/BtnDown
        ${Musik_SOURCE_DIR}/Classes/GaugeSeek
        ${Musik_SOURCE_DIR}/Classes/MusikLibrary
        ${Musik_SOURCE_DIR}/Classes/MusikPlayer
        ${Musik_SOURCE_DIR}/Classes/MusikPrefs
        ${Musik_SOURCE_DIR}/Classes/NowPlayingCtrl
        ${Musik_SOURCE_DIR}/Classes/PlaylistCtrl
	${Musik_SOURCE_DIR}/Classes/PlaylistInfoCtrl
        ${Musik_SOURCE_DIR}/Classes/SourcesBox
        ${Musik_SOURCE_DIR}/Classes/WebServer
        ${Musik_SOURCE_DIR}/Classes/MusikFX
        ${Musik_SOURCE_DIR}/Classes/MusikEQCtrl
        ${Musik_SOURCE_DIR}/Classes/MusikEQGauge
        ${Musik_SOURCE_DIR}/Classes/Tunage

        ${Musik_SOURCE_DIR}/Classes/Library/CFrameHeader
        ${Musik_SOURCE_DIR}/Classes/Library/CMP3Info
        ${Musik_SOURCE_DIR}/Classes/Library/COggInfo
        ${Musik_SOURCE_DIR}/Classes/Library/CVBitRate
        ${Musik_SOURCE_DIR}/Classes/Library/vcedit

        ${Musik_SOURCE_DIR}/Frames/MusikFrame
        ${Musik_SOURCE_DIR}/Frames/MusikFrameEvents
        ${Musik_SOURCE_DIR}/Frames/MusikLibraryFrame
        ${Musik_SOURCE_DIR}/Frames/MusikPrefsFrame
        ${Musik_SOURCE_DIR}/Frames/MusikTagFrame
        ${Musik_SOURCE_DIR}/Frames/MusikVolumeFrame
        ${Musik_SOURCE_DIR}/Frames/MusikFXFrame

 	${Musik_SOURCE_DIR}/Threads/ActivityBoxThreads
        ${Musik_SOURCE_DIR}/Threads/PlaylistCtrlThreads
        ${Musik_SOURCE_DIR}/Threads/MusikLibraryThreads
        ${Musik_SOURCE_DIR}/Threads/MusikTagThreads
        ${Musik_SOURCE_DIR}/Threads/MusikThreads

        ${Musik_SOURCE_DIR}/shibatch/Equ.cpp
        ${Musik_SOURCE_DIR}/shibatch/Fftsg_fl.cpp
    )

TARGET_LINK_LIBRARIES(Musik 
        ${WXWINDOWS_LIBRARY} 
        ${OGG_LIBRARY}
        ${SQLITE_LIBRARY}
        ${ID3_LIBRARY}
        ${FMOD_LIBRARY}
        ${VORBIS_LIBRARY}
    )

# Set build variables
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -pipe -fomit-frame-pointer")

# Set install options
INSTALL_TARGETS(/bin Musik)

