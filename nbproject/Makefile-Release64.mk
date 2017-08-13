#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Release64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Stackful/Stackful.o \
	${OBJECTDIR}/Stackful/src/sfatoms.o \
	${OBJECTDIR}/Stackful/src/sfbuiltins.o \
	${OBJECTDIR}/Stackful/src/sfdebug.o \
	${OBJECTDIR}/Stackful/src/sfextypes.o \
	${OBJECTDIR}/Stackful/src/sffndef.o \
	${OBJECTDIR}/Stackful/src/sfinterp.o \
	${OBJECTDIR}/Stackful/src/sfparser.o \
	${OBJECTDIR}/Stackful/src/sfshared.o \
	${OBJECTDIR}/Stackful/src/sftests.o \
	${OBJECTDIR}/Stackful/src/sftypes.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 -Wall -Wno-unused-parameter -Wextra
CXXFLAGS=-m64 -Wall -Wno-unused-parameter -Wextra

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/stackful.exe

${CND_DISTDIR}/${CND_CONF}/stackful.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/stackful ${OBJECTFILES} ${LDLIBSOPTIONS} -flto

${OBJECTDIR}/Stackful/Stackful.o: Stackful/Stackful.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/Stackful.o Stackful/Stackful.cpp

${OBJECTDIR}/Stackful/src/sfatoms.o: Stackful/src/sfatoms.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfatoms.o Stackful/src/sfatoms.cpp

${OBJECTDIR}/Stackful/src/sfbuiltins.o: Stackful/src/sfbuiltins.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfbuiltins.o Stackful/src/sfbuiltins.cpp

${OBJECTDIR}/Stackful/src/sfdebug.o: Stackful/src/sfdebug.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfdebug.o Stackful/src/sfdebug.cpp

${OBJECTDIR}/Stackful/src/sfextypes.o: Stackful/src/sfextypes.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfextypes.o Stackful/src/sfextypes.cpp

${OBJECTDIR}/Stackful/src/sffndef.o: Stackful/src/sffndef.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sffndef.o Stackful/src/sffndef.cpp

${OBJECTDIR}/Stackful/src/sfinterp.o: Stackful/src/sfinterp.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfinterp.o Stackful/src/sfinterp.cpp

${OBJECTDIR}/Stackful/src/sfparser.o: Stackful/src/sfparser.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfparser.o Stackful/src/sfparser.cpp

${OBJECTDIR}/Stackful/src/sfshared.o: Stackful/src/sfshared.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sfshared.o Stackful/src/sfshared.cpp

${OBJECTDIR}/Stackful/src/sftests.o: Stackful/src/sftests.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sftests.o Stackful/src/sftests.cpp

${OBJECTDIR}/Stackful/src/sftypes.o: Stackful/src/sftypes.cpp
	${MKDIR} -p ${OBJECTDIR}/Stackful/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DARCH64 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Stackful/src/sftypes.o Stackful/src/sftypes.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
