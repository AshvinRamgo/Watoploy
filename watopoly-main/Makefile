# Makefile for Watopoly project
#
# Use gcc flag -MMD to generate dependencies among source files.

########## Variables ##########

CXX = g++-11                             # compiler
CXXFLAGS = -std=c++20 -g -Wall -MMD      # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}  # makefile name

SOURCES = $(wildcard *.cc)               # source files (*.cc)
OBJECTS = ${SOURCES:.cc=.o}              # object files forming executable
DEPENDS = ${OBJECTS:.o=.d}               # substitute ".o" with ".d"
EXEC = watopoly                          # executable name

########## Targets ##########

.PHONY : clean run                       # not file names

${EXEC} : ${OBJECTS}                     # link step
	${CXX} ${CXXFLAGS} $^ -o $@

${OBJECTS} : ${MAKEFILE_NAME}            # changes to this file => recompile

# make implicitly generates rules to compile C++ files that generate .o files

run: ${EXEC}                             # run the executable
	./${EXEC}

-include ${DEPENDS}                      # include *.d files containing program dependences

clean :                                  # remove files that can be regenerated
	rm -f ${DEPENDS} ${OBJECTS} ${EXEC}
