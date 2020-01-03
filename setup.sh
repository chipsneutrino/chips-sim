#! /bin/bash

# Set the chips-sim directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export WCSIMHOME=$DIR
export G4WORKDIR=$DIR/geant4
export PATH=$PATH:$DIR/geant4/bin/Linux-g++/
export PATH=$PATH:$DIR/bin
