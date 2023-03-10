# ----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.21)
project(MdmSim)

# ----------------------------------------------------------------------------
# Setup the compiler
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_BUILD_TYPE "debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall")
enable_language(Fortran)
set(CMAKE_Fortran_FLAGS "-fPIC -finit-local-zero -falign-commons -fno-automatic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# ----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Geant4 simulation for TexPPAC" ON)

if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

# ----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
    ${Geant4_INCLUDE_DIR})

# ----------------------------------------------------------------------------
# Find ROOT
find_package(ROOT CONFIG REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
include_directories(${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

# ----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.F)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# ----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(MdmSim MdmSim.cc ${sources} ${headers})
target_link_libraries(MdmSim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(plotFirstMultipoleField PlotFirstMultipoleField.cc ${PROJECT_SOURCE_DIR}/include/FirstMultipoleField.hh ${PROJECT_SOURCE_DIR}/src/FirstMultipoleField.cc)
target_link_libraries(plotFirstMultipoleField ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(plotDipoleField PlotDipoleField.cc ${PROJECT_SOURCE_DIR}/include/DipoleField.hh ${PROJECT_SOURCE_DIR}/src/DipoleField.cc)
target_link_libraries(plotDipoleField ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# ----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(MdmSim_SCRIPTS
    init_vis.mac
    run.mac
    vis.mac
    rayin.dat
)

foreach(_script ${MdmSim_SCRIPTS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
    )
endforeach()
