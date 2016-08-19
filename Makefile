# version of this Makefile
MAKEFILE_VER=		0.4.1


# make options
MAKEFLAGS+=			-s



# NOTE: search for 'CUSTOM' text




# CUSTOM output executable file
OUT_FILE=			gsp

# CUSTOM paths
#INC_DIR=			inc
INC_DIR=			src
SRC_DIR=			src
LIB_DIR=			lib

# CUSTOM libs
LIBS_REL=			-l cxxtools
LIBS_DEB=			$(LIBS_REL)
LIBS_TES=			$(LIBS_REL) -l cxxtools-unit



# basic path
BUILD_DIR=			BUILD
RELEASE_DIR=		$(BUILD_DIR)/release
DEBUG_DIR=			$(BUILD_DIR)/debug
TEST_DIR=			$(BUILD_DIR)/test



# compiler and linker
CC=					g++
LD=					g++

# rm
RM=					rm -f



# flags
RELEASE_FLAGS=		-O3
DEBUG_FLAGS=		-O0 -ggdb -g3
TEST_FLAGS=			$(DEBUG_FLAGS)

CFLAGS=				-pipe -ansi -pedantic -Wall -Wextra
CC_FLAGS=			$(CFLAGS) -I$(INC_DIR)
LD_FLAGS=			-L$(LIB_DIR)



# commands
ifeq ($(MAKECMDGOALS), debug)
	OUT_DIR=		$(DEBUG_DIR)
	CC_COMMAND=		$(CC) $(DEBUG_FLAGS) $(CC_FLAGS)
	LD_COMMAND=		$(LD) $(DEBUG_FLAGS) $(LD_FLAGS) $(LIBS_DEB)
else ifeq ($(MAKECMDGOALS), test)
	OUT_DIR=		$(TEST_DIR)
	CC_COMMAND=		$(CC) $(TEST_FLAGS) $(CC_FLAGS)
	LD_COMMAND=		$(LD) $(TEST_FLAGS) $(LD_FLAGS) $(LIBS_TES)
else
	OUT_DIR=		$(RELEASE_DIR)
	CC_COMMAND=		$(CC) $(RELEASE_FLAGS) $(CC_FLAGS)
	LD_COMMAND=		$(LD) $(RELEASE_FLAGS) $(LD_FLAGS) $(LIBS_REL)
endif


# colors
NORMAL=				"\\033[0;39m"
GREEN=				"\\033[1;32m"
WHITE=				"\\033[1;37m"
YELLOW=				"\\033[1;33m"
RED=				"\\033[1;31m"
BLUE=				"\\033[1;34m"
CYAN=				"\\033[1;36m"
PINK=				"\\033[1;35m"

SPACE=				"\\033[70G"



# output
ECHO_DONE=			$(SPACE)$(GREEN)DONE$(NORMAL)
ECHO_SHIT=			$(SPACE)${RED}SHIT$(NORMAL)
CHECK=				&& echo $(ECHO_DONE) || (echo && echo $(ECHO_SHIT) && exit 1)



################################################################################
#--- main target
all:				release

release:			pre INFO_VARS INFO_CC SOURCES INFO_LD INFO_OUT
	@echo
	@echo "Hint: run with './$(OUT_DIR)/$(OUT_FILE)'"
	@echo

debug:				pre INFO_VARS INFO_CC SOURCES INFO_LD INFO_OUT
	@echo
	@echo "Hint: debug with 'ddd $(OUT_DIR)/$(OUT_FILE)'"
	@echo

test:				pre INFO_VARS INFO_CC SOURCES_TEST INFO_LD INFO_OUT
	@echo
	@echo "Hint: test with './$(OUT_DIR)/$(OUT_FILE)'"
	@echo



################################################################################
#--- CUSTOM dependence files

SOURCES:			MAIN
SOURCES_TEST:		GSPTEST

MAIN:				GSP $(OUT_DIR)/main.o
GSPTEST:			GSP $(INC_DIR)/GSPTest.h $(OUT_DIR)/GSPTest.o
GSP:				SEQUENCE $(INC_DIR)/GSP.h $(OUT_DIR)/GSP.o
SEQUENCE:			ITEM $(INC_DIR)/Sequence.h $(OUT_DIR)/Sequence.o
ITEM:				$(INC_DIR)/Item.h



################################################################################
#--- redefine the implicit rule to compile
$(OUT_DIR)/%.o:	$(SRC_DIR)/%.cpp
	@echo -n ${GREEN}"   + "${WHITE}$(SRC_DIR)/$*.cpp$(NORMAL) -o $(OUT_DIR)/$*.o
	$(CC_COMMAND) -c $(SRC_DIR)/$*.cpp -o $(OUT_DIR)/$*.o $(CHECK)



################################################################################
pre:
	mkdir -p $(RELEASE_DIR)
	mkdir -p $(DEBUG_DIR)
	mkdir -p $(TEST_DIR)



################################################################################
clean:
	@echo
	@echo $(YELLOW)"CLEAN:"$(NORMAL)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(RELEASE_DIR)"/*.o" $(RELEASE_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(RELEASE_DIR)/*.o $(RELEASE_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(DEBUG_DIR)"/*.o" $(DEBUG_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(DEBUG_DIR)/*.o $(DEBUG_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(TEST_DIR)"/*.o" $(TEST_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(TEST_DIR)/*.o $(TEST_DIR)/$(OUT_FILE)$(CHECK)
	@echo



################################################################################
INFO_VARS:
	@echo
	@echo $(YELLOW)"Variables used to build:"$(NORMAL)
	@echo $(BLUE)CC_FLAGS=	$(CC_FLAGS)$(NORMAL)
	@echo $(BLUE)LD_FLAGS=	$(LD_FLAGS)$(NORMAL)

INFO_CC:
	@echo
	@echo $(YELLOW)"Command used to build:"$(NORMAL)
	@echo    $(BLUE)$(CC_COMMAND) -c$(NORMAL)

INFO_LD:
	@echo
	@echo $(YELLOW)"Command used to link:"$(NORMAL)
	@echo    $(BLUE)$(LD_COMMAND)$(NORMAL)

INFO_OUT:
	@echo -n ${GREEN}"   + "${WHITE} $(OUT_DIR)"/*.o"$(NORMAL) -o $(OUT_DIR)/$(OUT_FILE)
	$(LD_COMMAND) $(OUT_DIR)/*.o -o $(OUT_DIR)/$(OUT_FILE) $(CHECK)



# End
