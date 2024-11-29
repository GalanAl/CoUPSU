#!/bin/bash -
# libcoupsu auto installer
# Copyright(c) 2024

### By default compile with 4 threads
MAKETHREADS=4
if [ ! -z "$1" ]
then
  MAKETHREADS=$1
fi


### COUPSU git sources and commit ###
COUPSU_LOCDIR=CoUPSU
COUPSU_GITDIR=https://github.com/GalanAl/CoUPSU.git
COUPSU_COMMIT=v1.0

### HELib git sources and commit ###
HELib_GITDIR=https://github.com/homenc/HElib.git
HELib_COMMIT=3e337a6

### HEXL git sources and branch ###
HEXL_GITDIR=https://github.com/jgdumas/hexl.git
HEXL_BRANCH=1.2.1
HEXL_COMMIT=v1.2.1

###  ###
LOGFILE=libcoupsu-auto-install.log


### die/cool ###
DONE="\033[0;36m done !\033[0m"
BEG="\033[1;32m * \033[0m"

die() {
    echo -ne "\n\033[1;31m * \033[0mfailed" ;
    if [[ -n $1 ]] ; then
	echo " ($1)"
    else
	echo "."
    fi
    exit -1 ;
}

cool() {
    echo -e $DONE
}

### Extract sources ###
extract() {
    LIB_NAME=$1
    LIB_GITDIR=$2
    LIB_COMMIT=$3
    if [ ! -z "$4" ]
    then 
	LIB_GITDIR="$2 --branch $4"
    fi
    echo -en "${BEG}fetching $1 ..."| tee -a ${LOGFILE}
    OK=0; git clone ${LIB_GITDIR} 2>&1 >/dev/null && OK=1
    [ "$OK" = "1" ] && cool | tee -a ${LOGFILE} || die
    cd $1
    OK=0; git reset --hard ${LIB_COMMIT} 2>&1 >/dev/null && OK=1
    [ "$OK" = "1" ] && cool | tee -a ../${LOGFILE} || die
    cd ..
}




### Create local directory ###
echo "mkdir libcoupsu && cd libcoupsu"
mkdir libcoupsu && cd libcoupsu


### Extract HEXL sources ###
extract "hexl" "${HEXL_GITDIR}" "${HEXL_COMMIT}" "${HEXL_BRANCH}"


### Extract HELib sources ###
extract "HElib" "${HELib_GITDIR}" "${HELib_COMMIT}"


### Extract COUPSU sources ###
extract "${COUPSU_LOCDIR}" "${COUPSU_GITDIR}" "${COUPSU_COMMIT}"


### Build HEXL (not mandatory) ###
echo -e "${BEG}building HEXL ..."| tee -a ${LOGFILE}
cd hexl
cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX="../" | tee -a ../${LOGFILE}
cmake --build build -j ${MAKETHREADS} | tee -a ../${LOGFILE}
OK=0; cmake --install build | tee -a ../../${LOGFILE} && OK=1
[ "$OK" = "1" ] && cool | tee -a ../../${LOGFILE} 
cd ..

### Build HELib ###
echo -e "${BEG}building HELib ..."| tee -a ${LOGFILE}
cd HElib
mkdir build && cd build
# cmake -DCMAKE_INSTALL_PREFIX=../ -DUSE_INTEL_HEXL=ON -DHEXL_DIR="../lib/cmake/hexl-1.2.1" .. | tee -a ../${LOGFILE}
cmake -DCMAKE_INSTALL_PREFIX=../ .. | tee -a ../${LOGFILE}
make -j ${MAKETHREADS} | tee -a ../${LOGFILE}
OK=0; make install | tee -a ../../${LOGFILE} || die && OK=1
[ "$OK" = "1" ] && cool | tee -a ../../${LOGFILE} || die
cd ../..


### Build COUPSU ###
echo -e "${BEG}building COUPSU ..." | tee -a ${LOGFILE}
cd ${COUPSU_LOCDIR}
OK=0; make -j ${MAKETHREADS} HELIB_DIR=../HElib | tee -a ../${LOGFILE} || die && OK=1
[ "$OK" = "1" ] && cool | tee -a ../${LOGFILE} || die
cd ..

### Done. ###
cd ..
