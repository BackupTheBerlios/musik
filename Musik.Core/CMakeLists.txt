##################################
#
# project name
# 
##################################

PROJECT(MusikCore)

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

ADD_LIBRARY( MusikCore
	${MusikCore_SOURCE_DIR}/src/Equ
	${MusikCore_SOURCE_DIR}/src/Fftsg_fl
	${MusikCore_SOURCE_DIR}/src/MusikEqualizer
	${MusikCore_SOURCE_DIR}/src/MusikFilename
	${MusikCore_SOURCE_DIR}/src/MusikLibrary
	${MusikCore_SOURCE_DIR}/src/MusikPlayer
	${MusikCore_SOURCE_DIR}/src/MusikPlaylist
	${MusikCore_SOURCE_DIR}/src/MusikOggInfo	
	${MusikCore_SOURCE_DIR}/src/MusikMp3Info
	${MusikCore_SOURCE_DIR}/src/MusikThread
	${MusikCore_SOURCE_DIR}/src/MusikBatchAdd
	${MusikCore_SOURCE_DIR}/src/MusikDir
	${MusikCore_SOURCE_DIR}/src/vcedit
	)

##################################
#
# link against needed libraries
#
##################################

TARGET_LINK_LIBRARIES(MusikCore
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

