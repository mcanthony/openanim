project (animation_twist)

# set(CMAKE_CXX_COMPILER "clang")

cmake_minimum_required (VERSION 2.8.4)
# set(CMAKE_VERBOSE_MAKEFILE true)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/")

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(LIBS "")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

add_definitions(-Wall -Werror -std=c++11)

###########################################################
# DEPENDENCIES

# looking for boost
find_package(Boost REQUIRED COMPONENTS filesystem system serialization unit_test_framework program_options regex)
include_directories(${Boost_INCLUDE_DIR})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

# looking for OpenGL libraries
# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIR})
# set(LIBS ${LIBS} ${OPENGL_LIBRARY})

# looking for glut
# find_package(GLUT REQUIRED)
# include_directories(${GLUT_INCLUDE_DIR})
# set(LIBS ${LIBS} ${GLUT_LIBRARY})

# looking for GLEW library
# find_path(GLEW_INCLUDE_DIR GL/glew.h ${LIBS_PATH}/glew/include /usr/include /usr/local/include /mingw/include C:/MinGW/include)
# find_library(GLEW_LIBRARY NAMES GLEW GLEW32 glew32 PATHS ${LIBS_PATH}/glew/lib /usr/lib /usr/local/lib)
# if (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
#   set (GLEW_FOUND TRUE)
#   message (STATUS "GLEW library found (${GLEW_INCLUDE_DIR} and ${GLEW_LIBRARY}).")
# else (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
#   message (FATAL_ERROR "GLEW library not found (${GLEW_INCLUDE_DIR} and ${GLEW_LIBRARY}).")
# endif (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
# include_directories(${GLEW_INCLUDE_DIR})
# set(LIBS ${LIBS} ${GLEW_LIBRARY})

# Qt fun
# SET(QT_USE_QTOPENGL TRUE)
# FIND_PACKAGE(Qt4 REQUIRED)
# include(${QT_USE_FILE})
# include_directories(${QT_INCLUDE_DIR})
# set(LIBS ${LIBS} ${QT_LIBRARIES})
# message(STATUS ${QT_LIBRARIES})

# looking for openexr libs
SET(OPENEXR_LIBRARY_PATHS /usr/lib /usr/local/lib /sw/lib /opt/local/lib $ENV{PROGRAM_FILES}/OpenEXR/lib/static)
FIND_PATH(OPENEXR_INCLUDE_DIR ImfRgbaFile.h PATH_SUFFIXES OpenEXR /usr/include /usr/local/include /sw/include /opt/local/include)
FIND_LIBRARY(OPENEXR_HALF_LIBRARY NAMES Half PATHS ${OPENEXR_LIBRARY_PATHS})
FIND_LIBRARY(OPENEXR_IEX_LIBRARY NAMES Iex PATHS ${OPENEXR_LIBRARY_PATHS})
if (OPENEXR_INCLUDE_DIR AND OPENEXR_HALF_LIBRARY AND OPENEXR_IEX_LIBRARY)
  set (HDF5_FOUND TRUE)
  message (STATUS "OpenEXR library found (${OPENEXR_INCLUDE_DIR}, ${OPENEXR_IEX_LIBRARY} and ${OPENEXR_HALF_LIBRARY}).")
else (OPENEXR_INCLUDE_DIR AND OPENEXR_HALF_LIBRARY AND OPENEXR_IEX_LIBRARY)
  message (FATAL_ERROR "OpenEXR library not found (${OPENEXR_INCLUDE_DIR}, ${OPENEXR_IEX_LIBRARY} and ${OPENEXR_HALF_LIBRARY}).")
endif (OPENEXR_INCLUDE_DIR AND OPENEXR_HALF_LIBRARY AND OPENEXR_IEX_LIBRARY)
include_directories(${OPENEXR_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENEXR_HALF_LIBRARY} ${OPENEXR_IEX_LIBRARY})

# blindly adding tbb
# set(LIBS ${LIBS} tbb)

###########################################################
# BUILD

include_directories(src)
add_subdirectory (src bin/demo)
