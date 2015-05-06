#! /bin/bash

#WCSim Home Directory
SOURCE="${BASH_SOURCE[0]}"
# While ${SOURCE} is a symbolic link, resolve it
while [ -h "${SOURCE}" ]; do
     DIR="$( cd -P "$( dirname "${SOURCE}" )" && pwd )"
     SOURCE="$( readlink "${SOURCE}" )"
     # Handle cases where SOURCE is ./something
     [[ ${SOURCE} != /* ]] && SOURCE="$DIR/${SOURCE}"
done
DIR="$( cd -P "$( dirname "${SOURCE}" )" && pwd | sed "s#setupScripts##g")"
export WCSIMHOME=${DIR}

source /unix/lartpc/software/root/setup.sh
echo "Root setup complete"
source /unix/lartpc/software/geant4/setup.sh
echo "geant4 setup complete"
source /unix/lartpc/software/genie/setup.sh
echo "genie setup complete"
