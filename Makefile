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
MAKEFILE_VER=		0.8.0



# make options
MAKEFLAGS+=			-s



# NOTE: search for 'CUSTOM' text




# CUSTOM output executable file
OUT_FILE=			stsm

# CUSTOM paths
INC_DIR=			inc
SRC_DIR=			src
LIB_DIR=			lib
EXT_DIR=			ext

# CUSTOM libs TODO remove cxxtools and any other to REL_LIBS
REL_LIBS=
STA_LIBS=			$(REL_LIBS)
DEB_LIBS=			$(REL_LIBS)
PRO_LIBS=			$(REL_LIBS)
TES_LIBS=			$(REL_LIBS) -l cxxtools -l cxxtools-unit



# basic path
BUILD_DIR=			BUILD
REL_BUILD_DIR=		$(BUILD_DIR)/release
STA_BUILD_DIR=		$(BUILD_DIR)/static
DEB_BUILD_DIR=		$(BUILD_DIR)/debug
PRO_BUILD_DIR=		$(BUILD_DIR)/profile
TES_BUILD_DIR=		$(BUILD_DIR)/test



# compiler and linker
CC=					g++
LD=					g++

# rm
RM=					rm -f



# flags
CC_FLAGS=			-pipe -std=c++11 -pedantic -Wall -Wextra -I$(INC_DIR) -I$(EXT_DIR)
LD_FLAGS=			-L$(LIB_DIR)

RELEASE_FLAGS=		-O3 -s				# optimization and remove all symbol table
STATIC_LD_FLAGS=	-static -static-libgcc -static-libstdc++ -pthread
DEBUG_FLAGS=		-Og -ggdb -g3 -fno-omit-frame-pointer	# some debug optimization and increase debug level to 3, mantain frame pointer for 'prof' tool
PROFILE_FLAGS=		-g -pg				# add 'gprof' tool info
PROFILE_LD_FLAGS=	-pg $(LD_FLAGS)
TEST_FLAGS=			$(DEBUG_FLAGS)



# commands
ifeq ($(MAKECMDGOALS), clean)
	GOAL=			Clean
else ifeq ($(MAKECMDGOALS), static)
	GOAL=			Static
	OUT_DIR=		$(STA_BUILD_DIR)
	CC_CMD_ARGS=	$(RELEASE_FLAGS) $(CC_FLAGS)
	LD_CMD_ARGS=	$(RELEASE_FLAGS) $(STATIC_LD_FLAGS) $(LD_FLAGS) $(STA_LIBS)
else ifeq ($(MAKECMDGOALS), debug)
	GOAL=			Debug
	OUT_DIR=		$(DEB_BUILD_DIR)
	CC_CMD_ARGS=	$(DEBUG_FLAGS) $(CC_FLAGS)
	LD_CMD_ARGS=	$(DEBUG_FLAGS) $(LD_FLAGS) $(DEB_LIBS)
else ifeq ($(MAKECMDGOALS), profile)
	GOAL=			Profile
	OUT_DIR=		$(PRO_BUILD_DIR)
	CC_CMD_ARGS=	$(PROFILE_FLAGS) $(CC_FLAGS)
	LD_CMD_ARGS=	$(PROFILE_FLAGS) $(PROFILE_LD_FLAGS) $(PRO_LIBS)
else ifeq ($(MAKECMDGOALS), test)
	GOAL=			Test
	OUT_DIR=		$(TES_BUILD_DIR)
	CC_CMD_ARGS=	$(TEST_FLAGS) $(CC_FLAGS)
	LD_CMD_ARGS=	$(TEST_FLAGS) $(LD_FLAGS) $(TES_LIBS)
else
	GOAL=			Release
	OUT_DIR=		$(REL_BUILD_DIR)
	CC_CMD_ARGS=	$(RELEASE_FLAGS) $(CC_FLAGS)
	LD_CMD_ARGS=	$(RELEASE_FLAGS) $(LD_FLAGS) $(REL_LIBS)
endif


# colors
# https://wiki.archlinux.org/index.php/Bash/Prompt_customization#Escapes_between_command_input_and_output
# https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
NORMAL=				"\\033[0;39m"
GREEN=				"\\033[1;32m"
WHITE=				"\\033[1;37m"
YELLOW=				"\\033[1;33m"
RED=				"\\033[1;31m"
BLUE=				"\\033[1;34m"
CYAN=				"\\033[1;36m"
PINK=				"\\033[1;35m"

SPACE=				"\\033[70G"
SPACE="\t\t"
# note this way to obtain actual column
# echo -en "\E[6n"; read -sdR CURPOS; CURPOS=`echo ${CURPOS} | cut -d '[' -f 2 | cut -d ';' -f 1`; echo -en "\\033[${CURPOS};70f"
SAVE_CUR_POS=		"\\033[s"
RESTORE_CUR_POS=	"\\033[u"



# output
ECHO_DONE=			$(SPACE)$(GREEN)DONE$(NORMAL)
ECHO_SHIT=			$(SPACE)${RED}SHIT$(NORMAL)
CHECK=				&& echo $(ECHO_DONE) || (echo && echo $(ECHO_SHIT) && exit 1)



################################################################################
#--- main target
all:				release

release:			INFO_TRG CMD_PREREQ INFO_VARS INFO_CC CMD_COMPILE INFO_LD CMD_LINK
	@echo
	@echo "Hint: run with './$(OUT_DIR)/$(OUT_FILE)'"
	@echo

static:				INFO_TRG CMD_PREREQ INFO_VARS INFO_CC CMD_COMPILE INFO_LD CMD_LINK
	@echo
	@echo "Hint: run with './$(OUT_DIR)/$(OUT_FILE)'"
	@echo

debug:				INFO_TRG CMD_PREREQ INFO_VARS INFO_CC CMD_COMPILE INFO_LD CMD_LINK
	@echo
	@echo "Hint: debug with 'ddd $(OUT_DIR)/$(OUT_FILE)'"
	@echo

profile:			INFO_TRG CMD_PREREQ INFO_VARS INFO_CC CMD_COMPILE INFO_LD CMD_LINK
	@echo
	@echo "Hint: profile with 'gprof $(OUT_DIR)/$(OUT_FILE)'"
	@echo

test:				INFO_TRG CMD_PREREQ INFO_VARS INFO_CC CMD_COMPILE_TEST INFO_LD CMD_LINK
	@echo
	@echo "Hint: test with './$(OUT_DIR)/$(OUT_FILE)'"
	@echo



################################################################################
#--- CUSTOM dependence files

CMD_COMPILE:		STSM $(OUT_DIR)/main.o
CMD_COMPILE_TEST:	STSMTest

STSM:				OCCURRMATRIX POSITION DATABASE SERIE SEQUENCEBLOCK RANGEDSEQUENCE KERNEL SEQUENCE CANDIDATE $(INC_DIR)/STSM.h $(OUT_DIR)/STSM.o
STSMTest:			STSM $(INC_DIR)/STSMTest.h $(OUT_DIR)/STSMTest.o
CANDIDATE:			SEQUENCE KERNEL RANGE $(INC_DIR)/Candidate.h $(OUT_DIR)/Candidate.o
SEQUENCE:			ITEM SIZE $(INC_DIR)/Sequence.h $(OUT_DIR)/Sequence.o
ITEM:				$(INC_DIR)/Item.h
KERNEL:				SEGMENT RANGE SUPPORT FREQUENCY $(INC_DIR)/Kernel.h $(OUT_DIR)/Kernel.o
RANGEDSEQUENCE:		SEQUENCE SIZE RANGE FREQUENCY $(INC_DIR)/RangedSequence.h $(OUT_DIR)/RangedSequence.o
SEQUENCEBLOCK:      SEQUENCE SIZE INTERVAL RANGE FREQUENCY $(INC_DIR)/SequenceBlock.h $(OUT_DIR)/SequenceBlock.o
OCCURRMATRIX:		RANGE INTERVAL $(INC_DIR)/OccurrenceMatrix.h $(OUT_DIR)/OccurrenceMatrix.o
RANGE:				SEGMENT $(INC_DIR)/Range.h $(OUT_DIR)/Range.o
INTERVAL:			SEGMENT $(INC_DIR)/Interval.h $(OUT_DIR)/Interval.o
SEGMENT:			POINT SIZE $(INC_DIR)/Segment.h $(OUT_DIR)/Segment.o
SIZE:				$(INC_DIR)/Size.h
POINT:				$(INC_DIR)/Point.h
POSITION:			POINT $(INC_DIR)/Position.h
SUPPORT:			$(INC_DIR)/Support.h
FREQUENCY:			$(INC_DIR)/Frequency.h
DATABASE:			SERIE $(INC_DIR)/Database.h
SERIE:				$(INC_DIR)/Serie.h



################################################################################
#--- redefine the implicit rule to compile
$(OUT_DIR)/%.o:		$(SRC_DIR)/%.cpp
	@echo -n ${GREEN}"   + "${WHITE}$(SRC_DIR)/$*.cpp$(NORMAL) -o $(OUT_DIR)/$*.o "... "
	$(CC) -c $(SRC_DIR)/$*.cpp $(CC_CMD_ARGS) -o $(OUT_DIR)/$*.o $(CHECK)

CMD_LINK:
	@echo -n ${GREEN}"   + "${WHITE} $(OUT_DIR)"/*.o"$(NORMAL) -o $(OUT_DIR)/$(OUT_FILE) "... "
	$(LD) $(OUT_DIR)/*.o $(LD_CMD_ARGS) -o $(OUT_DIR)/$(OUT_FILE) $(CHECK)



################################################################################
CMD_PREREQ:
	mkdir -p $(REL_BUILD_DIR)
	mkdir -p $(STA_BUILD_DIR)
	mkdir -p $(DEB_BUILD_DIR)
	mkdir -p $(PRO_BUILD_DIR)
	mkdir -p $(TES_BUILD_DIR)



################################################################################
clean:				INFO_TRG
	@echo
	@echo -n ${RED}"   - "${WHITE}$(RM) $(REL_BUILD_DIR)"/*.o" $(REL_BUILD_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(REL_BUILD_DIR)/*.o $(REL_BUILD_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(STA_BUILD_DIR)"/*.o" $(STA_BUILD_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(STA_BUILD_DIR)/*.o $(STA_BUILD_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(DEB_BUILD_DIR)"/*.o" $(DEB_BUILD_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(DEB_BUILD_DIR)/*.o $(DEB_BUILD_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(PRO_BUILD_DIR)"/*.o" $(PRO_BUILD_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(PRO_BUILD_DIR)/*.o $(PRO_BUILD_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(TES_BUILD_DIR)"/*.o" $(TES_BUILD_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(TES_BUILD_DIR)/*.o $(TES_BUILD_DIR)/$(OUT_FILE)$(CHECK)
	@echo



################################################################################
INFO_TRG:
	@echo $(YELLOW)"= "$(GOAL)" ="$(NORMAL)

INFO_VARS:
	@echo
	@echo $(YELLOW)"Variables used to build:"$(NORMAL)
	@echo $(BLUE)CC_FLAGS=	$(CC_FLAGS)$(NORMAL)
	@echo $(BLUE)LD_FLAGS=	$(LD_FLAGS)$(NORMAL)

INFO_CC:
	@echo
	@echo $(YELLOW)"Command used to build:"$(NORMAL)
	@echo    $(BLUE)$(CC)" <FILE> "$(CC_CMD_ARGS) -c$(NORMAL)

INFO_LD:
	@echo
	@echo $(YELLOW)"Command used to link:"$(NORMAL)
	@echo    $(BLUE)$(LD)" <FILE> "$(LD_CMD_ARGS)$(NORMAL)



# End
