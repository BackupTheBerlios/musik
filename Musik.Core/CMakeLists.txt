
PROJECT(Musik.Core)

IF(CMAKE_WX_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
ENDIF(CMAKE_WX_CXX_FLAGS)

# SQLite library
FIND_LIBRARY(SQLITE_LIBRARY NAMES sqlite PATHS /usr/lib /usr/local/lib)

# fmod library
FIND_LIBRARY(FMOD_LIBRARY NAMES fmod PATHS /usr/lib /usr/local/lib)

# ACE library
FIND_LIBRARY(ACE_LIBRARY NAMES ACE PATHS /usr/lib /usr/local/lib)

ADD_LIBRARY( Musik.Core
	${Musik.Core_SOURCE_DIR}/src/Equ
	${Musik.Core_SOURCE_DIR}/src/Fftsg_fl
	${Musik.Core_SOURCE_DIR}/src/MusikEqualizer
	${Musik.Core_SOURCE_DIR}/src/MusikLibrary
	${Musik.Core_SOURCE_DIR}/src/MusikPlayer
	${Musik.Core_SOURCE_DIR}/src/MusikOggInfo	
	${Musik.Core_SOURCE_DIR}/src/vcedit
	${Musik.Core_SOURCE_DIR}/src/MusikOggInfo
	)

TARGET_LINK_LIBRARIES(Musik.Core
	${SQLITE_LIBRARY}
	${FMOD_LIBRARY}
	${ACE_LIBRARY}
	)

# Set build variables
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -pipe -fomit-frame-pointer")

