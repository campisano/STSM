# Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
#
# This file is part of GenericMakefile project.
#
# This project is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this project. If not, see <http://www.gnu.org/licenses/>.



# version of this Makefile
MAKEFILE_VER =			2.2

# make options
MAKEFLAGS +=			-s



################################################################################
# NOTE: the follow variables must be specified here
# or before the inclusion of this file in another Makefile
# an example project is https://github.com/campisano/STSM/

# # CUSTOM output executable and lib files
# DEFAULT_TARGET =		debug
# TARGET_EXEC =			stsm
# TARGET_LIB =			libstsm.so
# HEADER_EXT =			.hpp
# SOURCE_EXT =			.cpp

# # CUSTOM paths
# INC_DIRS =			inc
# INC_EXT_DIRS =		ext
# FORMAT_INC_DIRS =		inc
# SRC_DIRS =			src/stsm src/utils
# TEST_SRC_DIRS =		src/tests
# FORMAT_SRC_DIRS =		src
# MAIN_SRC =			src/main.cpp
# LIB_DIRS =			lib
# ROOT_BUILD_DIR =		bin

# # CUSTOM libs for different targets
# RELEASE_LIBS =
# STATIC_LIBS =			$(RELEASE_LIBS)
# LIBRARY_LIBS =		$(RELEASE_LIBS)
# DEBUG_LIBS =			$(RELEASE_LIBS)
# PROFILE_LIBS =		$(RELEASE_LIBS)
# TEST_LIBS =			$(RELEASE_LIBS)

# # finally, include the generic makefile
# include ext/Makefile.inc

################################################################################



# obtaining list of include and lib dirs
INC_DIRS :=			$(shell find $(INC_DIRS) -type d) $(INC_EXT_DIRS)                       # TODO check if empty
INC_FLAGS :=			$(addprefix -I,$(INC_DIRS))
LIB_DIRS :=			$(shell find $(LIB_DIRS) -type d)					# TODO check if empty
LIB_FLAGS :=			$(addprefix -L,$(LIB_DIRS))



# compiler and linker binaries
CXX =				g++
LD =				g++

CPP_FLAGS =			$(INC_FLAGS) -MMD -MP
CXX_FLAGS =			-pipe -std=c++11 -fexceptions -pedantic -Wall -Wextra
LD_FLAGS =			$(LIB_FLAGS)



# CUSTOM specific flags for different targets
RELEASE_FLAGS =			-O3 -s									# optimization and remove all symbol table
RELEASE_LD_FLAGS =
STATIC_FLAGS =			$(RELEASE_FLAGS)
STATIC_LD_FLAGS =		-static -static-libgcc -static-libstdc++ -pthread
DEBUG_FLAGS =			-Og -ggdb -g3 -fno-omit-frame-pointer					# some debug optimization and increase debug level to 3, mantain frame pointer for 'prof' tool
DEBUG_LD_FLAGS =
PROFILE_FLAGS =			-g -pg									# add 'gprof' tool info
PROFILE_LD_FLAGS =		-pg
TEST_FLAGS =			$(DEBUG_FLAGS)
TEST_LD_FLAGS =			$(DEBUG_LD_FLAGS)
LIBRARY_FLAGS =			-fPIC
LIBRARY_LD_FLAGS =		-shared
#LIBRARY_LD_FLAGS =		-shared -Wl,-soname



# defining existing targets
TARGETS =			release static debug profile test library
NODEPS_TARGETS =		clean



# defining default target
ifeq ($(MAKECMDGOALS),)
	MAKECMDGOALS =		$(DEFAULT_TARGET)
endif

ifeq ($(MAKECMDGOALS), release)
	TITLE =			Release build
	BUILD_DIR =		$(ROOT_BUILD_DIR)/release
	TARGET =		$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=			$(shell find $(SRC_DIRS) -name *$(SOURCE_EXT)) $(MAIN_SRC)
	CXX_CMD_ARGS =		$(RELEASE_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(RELEASE_FLAGS) $(RELEASE_LD_FLAGS) $(LD_FLAGS) $(addprefix -l,$(RELEASE_LIBS))
else ifeq ($(MAKECMDGOALS), static)
	TITLE =			Static build
	BUILD_DIR =		$(ROOT_BUILD_DIR)/static
	TARGET =		$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=			$(shell find $(SRC_DIRS) -name *$(SOURCE_EXT)) $(MAIN_SRC)
	CXX_CMD_ARGS =		$(STATIC_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(STATIC_FLAGS) $(STATIC_LD_FLAGS) $(LD_FLAGS) $(addprefix -l,$(STATIC_LIBS))
else ifeq ($(MAKECMDGOALS), debug)
	TITLE =			Debug build
	BUILD_DIR =		$(ROOT_BUILD_DIR)/debug
	TARGET =		$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=			$(shell find $(SRC_DIRS) -name *$(SOURCE_EXT)) $(MAIN_SRC)
	CXX_CMD_ARGS =		$(DEBUG_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(DEBUG_FLAGS) $(DEBUG_LD_FLAGS) $(LD_FLAGS) $(addprefix -l,$(DEBUG_LIBS))
else ifeq ($(MAKECMDGOALS), profile)
	TITLE =			Profile build
	BUILD_DIR =		$(ROOT_BUILD_DIR)/profile
	TARGET =		$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=			$(shell find $(SRC_DIRS) -name *$(SOURCE_EXT)) $(MAIN_SRC)
	CXX_CMD_ARGS =		$(PROFILE_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(PROFILE_FLAGS) $(PROFILE_LD_FLAGS) $(LD_FLAGS) $(addprefix -l,$(PROFILE_LIBS))
else ifeq ($(MAKECMDGOALS), test)
	TITLE =			Test build
	BUILD_DIR =		$(ROOT_BUILD_DIR)/test
	TARGET =		$(BUILD_DIR)/$(TARGET_EXEC)
	SRCS :=			$(shell find $(SRC_DIRS) $(TEST_SRC_DIRS) -name *$(SOURCE_EXT))
	CXX_CMD_ARGS =		$(TEST_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(TEST_FLAGS) $(TEST_LD_FLAGS) $(LD_FLAGS) $(addprefix -l,$(TEST_LIBS))
else ifeq ($(MAKECMDGOALS), library)
	TITLE =			Lib build
	BUILD_DIR =		$(ROOT_BUILD_DIR)/lib
	TARGET =		$(BUILD_DIR)/$(TARGET_LIB)
	SRCS :=			$(shell find $(SRC_DIRS) -name *$(SOURCE_EXT))
	CXX_CMD_ARGS =		$(LIBRARY_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS)
	LD_CMD_ARGS =		$(LIBRARY_FLAGS) $(LIBRARY_LD_FLAGS) $(LD_FLAGS) $(addprefix -l,$(LIBRARY_LIBS))
else ifeq ($(MAKECMDGOALS), clean)
	TITLE =			Clean
endif



# obtaining list of objs and deps to produce
OBJS :=				$(SRCS:%=$(BUILD_DIR)/%.o)
DEPS :=				$(OBJS:.o=.d)



################################################################################
# platform specifics

ifeq ("$(PROGRAMFILES)$(ProgramFiles)","")
# linux
	SEPCHR =		/
# common binaries
	MKDIR =			mkdir -p
	RM =			rm -f
	RMALL =			rm -rf
# colors and terminal details
# https://wiki.archlinux.org/index.php/Bash/Prompt_customization#Escapes_between_command_input_and_output
# https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
	NORMAL =		"\\033[0;39m"
	GREEN =			"\\033[1;32m"
	WHITE =			"\\033[1;37m"
	YELLOW =		"\\033[1;33m"
	RED =			"\\033[1;31m"
	BLUE =			"\\033[1;34m"
else
	SEPCHR =		\\\\
ifneq (,$(findstring /cygdrive,$(PATH)))
# cygwin
	MKDIR =			mkdir -p
	RM =			rm -f
	RMALL =			rm -rf
else ifneq (,$(findstring /mingw,$(PATH)))
# mingw
	MKDIR =			mkdir -p
	RM =			rm -f
	RMALL =			rm -rf
else
# windows
	MKDIR =			md
	RM =			del
	RMALL =			rd /s /q
endif
endif

# check command results
CHECK =				&& echo $(GREEN)"DONE"$(NORMAL) || (echo $(RED)"==== ERROR ===="$(NORMAL); exit 1)



################################################################################
# defining targets

# multiple targets
$(TARGETS):			| INFO_TRG INFO_CMDS FORMAT $(TARGET)                                   # the targets after a | are executed in order even if parallel build (e.g. -j 10) is used

# include dependencies that CXX will generate
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS_TARGETS))))
-include $(DEPS)											# include dependencies
endif

clean:				INFO_TRG
	@echo Removing $(ROOT_BUILD_DIR) folder
	$(if $(ROOT_BUILD_DIR),($(RMALL) $(ROOT_BUILD_DIR)),)

.PHONY:				$(TARGETS) clean

# code style format target
FORMAT:
	which astyle > /dev/null \
&& astyle --options=none --quiet --style=allman \
--indent=spaces=4 --lineend=linux --align-pointer=middle \
--pad-oper --pad-comma --unpad-paren \
--add-brackets --convert-tabs --max-code-length=80 \
--suffix=none --recursive \
$(FORMAT_INC_DIRS:%='%/*$(HEADER_EXT)') \
$(FORMAT_SRC_DIRS:%='%/*$(SOURCE_EXT)') \
|| true													# TODO check if FORMAT DIRS are empty



# info targets

INFO_TRG:
	@echo $(YELLOW)= $(TITLE) =$(NORMAL)

INFO_CMDS:
	@echo
	@echo $(YELLOW)"Command arguments used to"$(NORMAL)
	@echo $(YELLOW)" compile: "$(BLUE)$(CXX)" <FILE> "$(CXX_CMD_ARGS)" -c"$(NORMAL)
	@echo $(YELLOW)" link:    "$(BLUE)$(LD)" <FILE> "$(LD_CMD_ARGS)$(NORMAL)



################################################################################
# redefine the implicit rule to compile

# delete the built-in rules for building object files from $(SOURCE_EXT) files
%.o:							%$(SOURCE_EXT)

# create a pattern rule with an empty recipe, so that make won’t fail if the dependency file doesn’t exist
$(BUILD_DIR)/%$(SOURCE_EXT).d: ;

# rule for .cpp (or any else defined in SOURCE_EXT var) source files
$(BUILD_DIR)/%$(SOURCE_EXT).o:	%$(SOURCE_EXT) $(BUILD_DIR)/%$(SOURCE_EXT).d
	@echo -n $(GREEN)"   + "$(WHITE)$<$(NORMAL)" -o "$@" ... "
	$(MKDIR) $(dir $@)
	$(CXX) -c $< $(CXX_CMD_ARGS) -o $@ $(CHECK)

# rule for the executable target
$(BUILD_DIR)/$(TARGET_EXEC):	$(OBJS)
	@echo -n $(GREEN)"   + "$(WHITE)$(BUILD_DIR)" ... *.o"$(NORMAL)" -o "$@" ... "
	$(LD) $(OBJS) $(LD_CMD_ARGS) -o $@ $(CHECK)

# rule for the lib target
$(BUILD_DIR)/$(TARGET_LIB):	$(OBJS)
	@echo -n $(GREEN)"   + "$(WHITE)$(BUILD_DIR)" ... *.o"$(NORMAL)" -o "$@" ... "
	$(LD) $(OBJS) $(LD_CMD_ARGS) -o $@ $(CHECK)



# End
