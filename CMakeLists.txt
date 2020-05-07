#---Setup the project
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(chips-sim CXX)

#---Build Configuration
set(CMAKE_CXX_STANDARD 11)
set(BUILD_SHARED_LIBS YES CACHE BOOL "")
set(BUILD_STATIC_LIBS NO CACHE BOOL "")
set(CMAKE_VERBOSE_MAKEFILE off)

#---Default to a 'release' build with O2 optimisations
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

#---Find ROOT and define it's cmake macros
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT CONFIG REQUIRED COMPONENTS)
if(NOT ROOT_FOUND)
  message(STATUS "ROOT package not found.")  
  return()
endif()
include(${ROOT_USE_FILE})

#---Locate sources and headers for this project
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})

#---WCSimRootDict.cxx and WCSimRootDict.pcm generation by rootcling
ROOT_GENERATE_DICTIONARY(WCSimRootDict 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootEvent.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootGeom.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimPmtInfo.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimCHIPSPMT.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimSK1pePMT.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimTOTPMT.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimPMTManager.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimPMTConfig.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimLCManager.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimLCConfig.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimTruthSummary.hh 
                         ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimEvDisplay.hh 
                         LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/WCSimRootLinkDef.hh)

#---Make shared object library, crucial for reading ROOT classes
add_library(WCSimRoot SHARED 
            ./src/base/WCSimRootEvent.cc 
            ./src/base/WCSimRootGeom.cc 
            ./src/base/WCSimPmtInfo.cc 
            ./src/base/WCSimCHIPSPMT.cc 
            ./src/base/WCSimSK1pePMT.cc 
            ./src/base/WCSimTOTPMT.cc 
            ./src/base/WCSimPMTManager.cc 
            ./src/base/WCSimPMTConfig.cc 
            ./src/base/WCSimLCManager.cc 
            ./src/base/WCSimLCConfig.cc 
            ./src/base/WCSimTruthSummary.cc 
            ./src/base/WCSimEvDisplay.cc
            WCSimRootDict.cxx)
target_link_libraries(WCSimRoot ${ROOT_LIBRARIES})

#---Find Geant4 and activate all UI and VIS drivers by default
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})

#---Locate sources and headers for this project
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/base/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#---Add the WCSim executable and link to all the required libraries
add_executable(WCSim src/apps/WCSim.cc ${sources} ${headers} WCSimRootDict.cxx)
target_link_libraries(WCSim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Gui EG WCSimRoot Tree)

#---Add the geometryHelper executable and link to all the required libraries
add_executable(geometryHelper src/apps/geometryHelper.cc ${sources} ${headers})
target_link_libraries(geometryHelper ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Gui EG WCSimRoot Tree)

#---Add the simDisplay executable and link to all the required libraries
add_executable(simDisplay src/apps/simDisplay.cc ${sources} ${headers})
target_link_libraries(simDisplay ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Gui EG WCSimRoot Tree)

#---Download large data files to the config directory
if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4NDL4.5)
  message(STATUS "Already have G4NDL4.5")
else()
  message(STATUS "Downloading G4NDL4.5")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4NDL.4.5.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4NDL.4.5.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4NDL.4.5.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4NDL4.5 $ENV{CHIPSSIM}/config/geant4/G4NDL4.5)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4NDL.4.5.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4EMLOW7.7)
  message(STATUS "Already have G4EMLOW7.7")
else()
  message(STATUS "Downloading G4EMLOW7.7")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4EMLOW.7.7.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4EMLOW.7.7.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4EMLOW.7.7.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4EMLOW7.7 $ENV{CHIPSSIM}/config/geant4/G4EMLOW7.7)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4EMLOW.7.7.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4PhotonEvaporation5.3)
  message(STATUS "Already have G4PhotonEvaporation5.3")
else()
  message(STATUS "Downloading G4PhotonEvaporation5.3")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4PhotonEvaporation.5.3.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4PhotonEvaporation.5.3.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4PhotonEvaporation.5.3.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename PhotonEvaporation5.3 $ENV{CHIPSSIM}/config/geant4/PhotonEvaporation5.3)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4PhotonEvaporation.5.3.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4RadioactiveDecay5.3)
  message(STATUS "Already have G4RadioactiveDecay5.3")
else()
  message(STATUS "Downloading G4RadioactiveDecay5.3")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4RadioactiveDecay.5.3.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4RadioactiveDecay.5.3.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4RadioactiveDecay.5.3.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename RadioactiveDecay5.3 $ENV{CHIPSSIM}/config/geant4/RadioactiveDecay5.3)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4RadioactiveDecay.5.3.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4SAIDDATA2.0)
  message(STATUS "Already have G4SAIDDATA2.0")
else()
  message(STATUS "Downloading G4SAIDDATA2.0")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4SAIDDATA.2.0.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4SAIDDATA.2.0.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4SAIDDATA.2.0.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4SAIDDATA2.0 $ENV{CHIPSSIM}/config/geant4/G4SAIDDATA2.0)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4SAIDDATA.2.0.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4PARTICLEXS1.1)
  message(STATUS "Already have G4PARTICLEXS1.1")
else()
  message(STATUS "Downloading G4PARTICLEXS1.1")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4PARTICLEXS.1.1.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4PARTICLEXS.1.1.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4PARTICLEXS.1.1.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4PARTICLEXS1.1 $ENV{CHIPSSIM}/config/geant4/G4PARTICLEXS1.1)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4PARTICLEXS.1.1.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4ABLA3.1)
  message(STATUS "Already have G4ABLA3.1")
else()
  message(STATUS "Downloading G4ABLA3.1")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4ABLA.3.1.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4ABLA.3.1.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4ABLA.3.1.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4ABLA3.1 $ENV{CHIPSSIM}/config/geant4/G4ABLA3.1)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4ABLA.3.1.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4INCL1.0)
  message(STATUS "Already have G4INCL1.0")
else()
  message(STATUS "Downloading G4INCL1.0")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4INCL.1.0.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4INCL.1.0.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4INCL.1.0.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4INCL1.0 $ENV{CHIPSSIM}/config/geant4/G4INCL1.0)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4INCL.1.0.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4PII1.3)
  message(STATUS "Already have G4PII1.3")
else()
  message(STATUS "Downloading G4PII1.3")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4PII.1.3.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4PII.1.3.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4PII.1.3.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4PII1.3 $ENV{CHIPSSIM}/config/geant4/G4PII1.3)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4PII.1.3.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4ENSDFSTATE2.2)
  message(STATUS "Already have G4ENSDFSTATE2.2")
else()
  message(STATUS "Downloading G4ENSDFSTATE2.2")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4ENSDFSTATE.2.2.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4ENSDFSTATE.2.2.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4ENSDFSTATE.2.2.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4ENSDFSTATE2.2 $ENV{CHIPSSIM}/config/geant4/G4ENSDFSTATE2.2)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4ENSDFSTATE.2.2.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4RealSurface2.1.1)
  message(STATUS "Already have G4RealSurface2.1.1")
else()
  message(STATUS "Downloading G4RealSurface2.1.1")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4RealSurface.2.1.1.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4RealSurface.2.1.1.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4RealSurface.2.1.1.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename RealSurface2.1.1 $ENV{CHIPSSIM}/config/geant4/RealSurface2.1.1)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4RealSurface.2.1.1.tar.gz)
endif()

if(EXISTS $ENV{CHIPSSIM}/config/geant4/G4TENDL1.3.2)
  message(STATUS "Already have G4TENDL1.3.2")
else()
  message(STATUS "Downloading G4TENDL1.3.2")
  file(DOWNLOAD http://geant4-data.web.cern.ch/geant4-data/datasets/G4TENDL.1.3.2.tar.gz 
    $ENV{CHIPSSIM}/config/geant4/G4TENDL.1.3.2.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    tar xzf $ENV{CHIPSSIM}/config/geant4/G4TENDL.1.3.2.tar.gz)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rename G4TENDL1.3.2 $ENV{CHIPSSIM}/config/geant4/G4TENDL1.3.2)
  execute_process(COMMAND ${CMAKE_COMMAND} -E 
    rm $ENV{CHIPSSIM}/config/geant4/G4TENDL.1.3.2.tar.gz)
endif()


