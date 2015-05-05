#! /bin/bash

#WCSim Home Directory

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. && pwd )"
DIR=${DIR#setupScripts/}
export WCSIMHOME=${DIR}

source /unix/lartpc/software/root/setup.sh
echo "Root setup complete"
source /unix/lartpc/software/geant4/setup.sh
echo "geant4 setup complete"
source /unix/lartpc/software/genie/setup.sh
echo "genie setup complete"
