#! /bin/bash

CURRENTDIR=$(pwd)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -f "$DIR/WCSim" ]
then
    echo "${C_GREEN}chips-sim built${C_RESET}"
else
    echo "${C_RED}building chips-sim${C_RESET}"
    NB_CORES=$(grep -c '^processor' /proc/cpuinfo)
    export MAKEFLAGS="-j$((NB_CORES+1)) -l${NB_CORES}"
    cd $DIR
    cmake .
    make
    cd $CURRENTDIR
    echo "${C_GREEN}chips-sim built${C_RESET}"
fi

export G4VIS_USE=1
export G4VIS_USE_OPENGLQT=1

export CHIPSSIM=$DIR
export LD_LIBRARY_PATH=$DIR:$LD_LIBRARY_PATH
export CPLUS_INCLUDE_PATH=$DIR/include:$CPLUS_INCLUDE_PATH
export PATH=$DIR:$PATH
echo "${C_GREEN}chips-sim setup done${C_RESET}"