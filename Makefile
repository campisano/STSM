# Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
#               2017 Fabio Porto
#               2017 Fabio Perosi
#               2017 Esther Pacitti
#               2017 Florent Masseglia
#               2017 Eduardo Ogasawara
#
# This file is part of STSM (Spatio-Temporal Sequence Miner).
#
# STSM is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# STSM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with STSM.  If not, see <http://www.gnu.org/licenses/>.



# version of this Makefile
MAKEFILE_VER =			1.1.3

# make options
MAKEFLAGS +=			-s



# NOTE: to edit this file, search for the word 'CUSTOM'



# CUSTOM output executable and lib files
TARGET_EXEC =			stsm
TARGET_LIB =			libSTSM.so

# CUSTOM paths
INC_DIRS =				inc ext
SRC_DIRS =				src/stsm src/utils
MAIN = 					src/main.cpp
TEST_DIRS =				src/tests
FORMAT_INC_DIRS = 		inc
FORMAT_SRC_DIRS = 		src
LIB_DIRS =				lib
ROOT_BUILD_DIR =		BUILD

# CUSTOM libs for different targets
RELEASE_LIBS =
STATIC_LIBS =			$(RELEASE_LIBS)
LIBRARY_LIBS =			$(RELEASE_LIBS)
DEBUG_LIBS =			$(RELEASE_LIBS)
PROFILE_LIBS =			$(RELEASE_LIBS)
TEST_LIBS =				$(RELEASE_LIBS)



# obtaining list of include and lib dirs
INC_DIRS :=				$(shell find $(INC_DIRS) -type d)
INC_FLAGS :=			$(addprefix -I,$(INC_DIRS))
LIB_DIRS :=				$(shell find $(LIB_DIRS) -type d)
LIB_FLAGS :=			$(addprefix -L,$(LIB_DIRS))



# compiler and linker binaries
CXX =					g++
LD =					g++

CPP_FLAGS =				$(INC_FLAGS) -MMD -MP
CXX_FLAGS =				-pipe -std=c++11 -fexceptions -pedantic -Wall -Wextra
LD_FLAGS =				$(LIB_FLAGS)



# CUSTOM specific flags for different targets
RELEASE_FLAGS =			-O3 -s				# optimization and remove all symbol table
RELEASE_LD_FLAGS =
STATIC_FLAGS =			$(RELEASE_FLAGS)
STATIC_LD_FLAGS =		-static -static-libgcc -static-libstdc++ -pthread
DEBUG_FLAGS =			-Og -ggdb -g3 -fno-omit-frame-pointer	# some debug optimization and increase debug level to 3, mantain frame pointer for 'prof' tool
DEBUG_LD_FLAGS =
PROFILE_FLAGS =			-g -pg				# add 'gprof' tool info
PROFILE_LD_FLAGS =		-pg
TEST_FLAGS =			$(DEBUG_FLAGS)
TEST_LD_FLAGS =			$(DEBUG_LD_FLAGS)
LIBRARY_FLAGS =			-fPIC
LIBRARY_LD_FLAGS =		-shared  -Wl,-soname



# defining existing targets
TARGETS =				release static debug profile test library

# defining default target
ifeq ($(MAKECMDGOALS),)
	MAKECMDGOALS =		release
endif

ifeq ($(MAKECMDGOALS), release)
	TITLE =				Release build
	BUILD_DIR =			$(ROOT_BUILD_DIR)/release
	TARGET =			$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=				$(shell find $(SRC_DIRS) -name *.cpp) $(MAIN)
	CXX_CMD_ARGS =		$(RELEASE_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(RELEASE_FLAGS) $(RELEASE_LD_FLAGS) $(LD_FLAGS) $(RELEASE_LIBS)
else ifeq ($(MAKECMDGOALS), static)
	TITLE =				Static build
	BUILD_DIR =			$(ROOT_BUILD_DIR)/static
	TARGET =			$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=				$(shell find $(SRC_DIRS) -name *.cpp) $(MAIN)
	CXX_CMD_ARGS =		$(STATIC_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(STATIC_FLAGS) $(STATIC_LD_FLAGS) $(LD_FLAGS) $(STATIC_LIBS)
else ifeq ($(MAKECMDGOALS), debug)
	TITLE =				Debug build
	BUILD_DIR =			$(ROOT_BUILD_DIR)/debug
	TARGET =			$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=				$(shell find $(SRC_DIRS) -name *.cpp) $(MAIN)
	CXX_CMD_ARGS =		$(DEBUG_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(DEBUG_FLAGS) $(DEBUG_LD_FLAGS) $(LD_FLAGS) $(DEBUG_LIBS)
else ifeq ($(MAKECMDGOALS), profile)
	TITLE =				Profile build
	BUILD_DIR =			$(ROOT_BUILD_DIR)/profile
	TARGET =			$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=				$(shell find $(SRC_DIRS) -name *.cpp) $(MAIN)
	CXX_CMD_ARGS =		$(PROFILE_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(PROFILE_FLAGS) $(PROFILE_LD_FLAGS) $(LD_FLAGS) $(PROFILE_LIBS)
else ifeq ($(MAKECMDGOALS), test)
	TITLE =				Test build
	BUILD_DIR =			$(ROOT_BUILD_DIR)/test
	TARGET =			$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=				$(shell find $(SRC_DIRS) $(TEST_DIRS) -name *.cpp)
	CXX_CMD_ARGS =		$(TEST_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(TEST_FLAGS) $(TEST_LD_FLAGS) $(LD_FLAGS) $(TEST_LIBS)
else ifeq ($(MAKECMDGOALS), library)
	TITLE =				Lib build
	BUILD_DIR =			$(ROOT_BUILD_DIR)/lib
	TARGET =			$(BUILD_DIR)/$(TARGET_LIB)
	SRCS :=				$(shell find $(SRC_DIRS) -name *.cpp)
	CXX_CMD_ARGS =		$(LIBRARY_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(LIBRARY_FLAGS) $(LIBRARY_LD_FLAGS) $(LD_FLAGS) $(LIBRARY_LIBS)
else ifeq ($(MAKECMDGOALS), clean)
	TITLE =				Clean
endif



################################################################################
# platform specifics
ifdef COMSPEC
# windows
# common binaries
	MKDIR =			md
	MV =			move
	RM =			del
	RMALL =			rd /s /q
else
# linux
# common binaries
	MKDIR =			mkdir -p
	MV = 			mv -f
	RM = 			rm -f
    RMALL = 		rm -rf
# colors and terminal details
# https://wiki.archlinux.org/index.php/Bash/Prompt_customization#Escapes_between_command_input_and_output
# https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
	NORMAL =		"\\033[0;39m"
	GREEN =			"\\033[1;32m"
	WHITE =			"\\033[1;37m"
	YELLOW =		"\\033[1;33m"
	RED =			"\\033[1;31m"
	BLUE =			"\\033[1;34m"
endif



################################################################################

# obtaining list of objs and deps to produce
OBJS :=					$(SRCS:%=$(BUILD_DIR)/%.o)
DEPS :=					$(OBJS:.o=.d)
-include $(DEPS)		# include dependencies

# targets
$(TARGETS):				INFO_TRG INFO_CMDS FORMAT $(TARGET) cleandeps

FORMAT:
	which astyle > /dev/null && astyle --options=none --quiet --style=allman \
--indent=spaces=4 --lineend=linux --align-pointer=middle \
--pad-oper --pad-comma --unpad-paren \
--add-brackets --convert-tabs --max-code-length=80 \
--suffix=none --recursive $(FORMAT_INC_DIRS:%='%/*.h') $(FORMAT_SRC_DIRS:%='%/*.cpp')

.PHONY:					$(TARGETS) clean cleandeps

clean:					INFO_TRG
	@echo Removing $(ROOT_BUILD_DIR)
	$(if $(ROOT_BUILD_DIR),($(RMALL) $(ROOT_BUILD_DIR)),)

cleandeps:
	$(RM) $(DEPS)



################################################################################
# redefine the implicit rule to compile

# rule for a c++ source file
$(BUILD_DIR)/%.cpp.o:			%.cpp
	@echo -n $(GREEN)"   + "$(WHITE)$<$(NORMAL)" -o "$@" ... "
	$(MKDIR) $(dir $@)
	$(CXX) -c $< $(CXX_CMD_ARGS) -o $@ $(CHECK)


# rule for executable target
$(BUILD_DIR)/$(TARGET_EXEC):	$(OBJS)
	@echo -n $(GREEN)"   + "$(WHITE)$(BUILD_DIR)" ... *.o"$(NORMAL)" -o "$@" ... "
	$(LD) $(OBJS) $(LD_CMD_ARGS) -o $@ $(CHECK)

# rule for lib target
$(BUILD_DIR)/$(TARGET_LIB):		$(OBJS)
	@echo -n $(GREEN)"   + "$(WHITE)$(BUILD_DIR)" ... *.o"$(NORMAL)" -o "$@" ... "
	$(LD) $(OBJS) $(LD_CMD_ARGS) -o $@ $(CHECK)



################################################################################
# info targets

INFO_TRG:
	@echo $(YELLOW)= $(TITLE) =$(NORMAL)

INFO_CMDS:
	@echo
	@echo $(YELLOW)"Command arguments used to"$(NORMAL)
	@echo $(YELLOW)" compile: "$(BLUE)$(CXX)" <FILE> "$(CXX_CMD_ARGS)" -c"$(NORMAL)
	@echo $(YELLOW)" link:    "$(BLUE)$(LD)" <FILE> "$(LD_CMD_ARGS)$(NORMAL)



################################################################################
# check command results
CHECK =				&& echo $(GREEN)"DONE"$(NORMAL) || (echo $(RED)"==== ERROR ===="$(NORMAL); exit 1)



# End
