# CUSTOM output executable and lib files
DEFAULT_TARGET :=		debug
TARGET_EXEC :=			stsm
TARGET_LIB :=			libstsm.so
HEADER_EXT :=			.h
SOURCE_EXT :=			.cpp

# CUSTOM paths
INC_DIRS :=			inc
INC_EXT_DIRS :=			ext
FORMAT_INC_DIRS :=		inc
SRC_DIRS :=			src/stsm src/utils
TEST_SRC_DIRS :=		src/tests
FORMAT_SRC_DIRS :=		src
MAIN_SRC :=			src/main.cpp
LIB_DIRS :=			lib
ROOT_BUILD_DIR :=		BUILD

# CUSTOM libs for different targets
RELEASE_LIBS :=
STATIC_LIBS :=			$(RELEASE_LIBS)
LIBRARY_LIBS :=			$(RELEASE_LIBS)
DEBUG_LIBS :=			$(RELEASE_LIBS)
PROFILE_LIBS :=			$(RELEASE_LIBS)
TEST_LIBS :=			$(RELEASE_LIBS)

# finally, include the generic makefile
include ext/Makefile.inc
