##################################
#
# project name
# 
##################################

PROJECT(Musik.Core)

##################################
#
# build flags
#
##################################

IF(CMAKE_WX_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
ENDIF(CMAKE_WX_CXX_FLAGS)

##################################
#
# find target link libraries
#
##################################

FIND_LIBRARY(SQLITE_LIBRARY NAMES sqlite PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(FMOD_LIBRARY NAMES fmod PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(ACE_LIBRARY NAMES ACE PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(ID3_LIBRARY NAMES id3 PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(VORBIS_LIBRARY NAMES vorbis PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(OGG_LIBRARY NAMES ogg PATHS /usr/lib /usr/local/lib)

##################################
#
# Musik.Core source files
#
##################################

ADD_LIBRARY( Musik.Core
	${Musik.Core_SOURCE_DIR}/src/Equ
	${Musik.Core_SOURCE_DIR}/src/Fftsg_fl
	${Musik.Core_SOURCE_DIR}/src/MusikEqualizer
	${Musik.Core_SOURCE_DIR}/src/MusikFilename
	${Musik.Core_SOURCE_DIR}/src/MusikLibrary
	${Musik.Core_SOURCE_DIR}/src/MusikPlayer
	${Musik.Core_SOURCE_DIR}/src/MusikPlaylist
	${Musik.Core_SOURCE_DIR}/src/MusikOggInfo	
	${Musik.Core_SOURCE_DIR}/src/MusikMp3Info
	${Musik.Core_SOURCE_DIR}/src/vcedit
	)

##################################
#
# link against needed libraries
#
##################################

TARGET_LINK_LIBRARIES(Musik.Core
	${SQLITE_LIBRARY}
	${FMOD_LIBRARY}
	${ACE_LIBRARY}
	${ID3_LIBRARY}
	${VORBIS_LIBRARY}
	)

##################################
#
# Set build variables
#
##################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -pipe -fomit-frame-pointer")

