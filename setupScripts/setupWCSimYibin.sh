#! /bin/bash

#WCSim Home Directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. && pwd )"
DIR=${DIR#setupScripts/}
export WCSIMHOME=${DIR}

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR/
doG4
doRoot
