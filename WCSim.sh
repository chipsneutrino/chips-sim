#!/bin/bash
cd /home/ajperch/MINOS/waterCherenkov/WCSim/
source ../setupWCSim.sh
make
WCSim vectors.mac
