##################################
#
# set project name
# 
##################################

PROJECT(musikCore)

##################################
#
# set build flags
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
FIND_LIBRARY(OPENTHREADS_LIBRARY NAMES OpenThreads PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(ID3_LIBRARY NAMES id3 PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(VORBIS_LIBRARY NAMES vorbis PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(OGG_LIBRARY NAMES ogg PATHS /usr/lib /usr/local/lib)

##################################
#
# set musikCore source files
#
##################################

ADD_LIBRARY( musikCore
	${musikCore_SOURCE_DIR}/src/Equ
	${musikCore_SOURCE_DIR}/src/Fftsg_fl
	${musikCore_SOURCE_DIR}/src/musikBatchAdd
	${musikCore_SOURCE_DIR}/src/musikBatchRetag
	${musikCore_SOURCE_DIR}/src/musikCrossfader
	${musikCore_SOURCE_DIR}/src/musikDir
	${musikCore_SOURCE_DIR}/src/musikDynDspInfo
	${musikCore_SOURCE_DIR}/src/musikEQSettings
	${musikCore_SOURCE_DIR}/src/musikEqualizer
	${musikCore_SOURCE_DIR}/src/musikFilename
	${musikCore_SOURCE_DIR}/src/musikFunctor
	${musikCore_SOURCE_DIR}/src/musikLibrary
	${musikCore_SOURCE_DIR}/src/musikMp3Info
	${musikCore_SOURCE_DIR}/src/musikOggInfo
	${musikCore_SOURCE_DIR}/src/musikPlayer
	${musikCore_SOURCE_DIR}/src/musikPlaylist
	${musikCore_SOURCE_DIR}/src/musikRemoveOld
	${musikCore_SOURCE_DIR}/src/musikTask
	${musikCore_SOURCE_DIR}/src/vcedit
	)

##################################
#
# link against needed libraries
#
##################################

TARGET_LINK_LIBRARIES(musikCore
	${SQLITE_LIBRARY}
	${FMOD_LIBRARY}
	${OPENTHREADS_LIBRARY}
	${ID3_LIBRARY}
	${VORBIS_LIBRARY}
	)

##################################
#
# set build variables
#
##################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -pipe -fomit-frame-pointer")

