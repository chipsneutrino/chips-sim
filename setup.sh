#! /bin/bash

# Set the chips-sim directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export WCSIMHOME=$DIR
export LD_LIBRARY_PATH=$DIR:$LD_LIBRARY_PATH
export CPLUS_INCLUDE_PATH=$DIR/include:$CPLUS_INCLUDE_PATH
export PATH=$DIR:$PATH
