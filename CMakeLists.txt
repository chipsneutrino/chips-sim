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