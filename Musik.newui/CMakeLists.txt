
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
        ${Musik_SOURCE_DIR}/src/MusikApp
        ${Musik_SOURCE_DIR}/src/MusikDragGaugeCtrl
        ${Musik_SOURCE_DIR}/src/MusikFrame
        ${Musik_SOURCE_DIR}/src/MusikLibrary
        ${Musik_SOURCE_DIR}/src/MusikListCtrl
        ${Musik_SOURCE_DIR}/src/MusikNowPlayingCtrl       
	${Musik_SOURCE_DIR}/src/MusikPlayer
        ${Musik_SOURCE_DIR}/src/MusikPlaylistCtrl
        ${Musik_SOURCE_DIR}/src/MusikSeekCtrl
        ${Musik_SOURCE_DIR}/src/MusikSelectionAreaCtrl
        ${Musik_SOURCE_DIR}/src/MusikSimpleQueryCtrl
        ${Musik_SOURCE_DIR}/src/MusikSourcesCtrl
        ${Musik_SOURCE_DIR}/src/wxprec
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

