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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/functions.o \
	${OBJECTDIR}/lang.o \
	${OBJECTDIR}/lenv.o \
	${OBJECTDIR}/lib/mpc/mpc.o \
	${OBJECTDIR}/lval.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/util.o


# C Compiler Flags
CFLAGS=-O0

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs libedit` -lm   

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/klisp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/klisp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/klisp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/functions.o: functions.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/functions.o functions.c

${OBJECTDIR}/lang.o: lang.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lang.o lang.c

${OBJECTDIR}/lenv.o: lenv.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lenv.o lenv.c

${OBJECTDIR}/lib/mpc/mpc.o: lib/mpc/mpc.c 
	${MKDIR} -p ${OBJECTDIR}/lib/mpc
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/mpc/mpc.o lib/mpc/mpc.c

${OBJECTDIR}/lval.o: lval.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lval.o lval.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/util.o: util.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -Ilib/mpc `pkg-config --cflags libedit` -std=c99  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/util.o util.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/klisp

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
