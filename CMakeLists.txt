cmake_minimum_required(VERSION 2.6)
project("Cesium Terrain Builder")

# The version number
set(CTB_VERSION_MAJOR 0)
set(CTB_VERSION_MINOR 4)
set(CTB_VERSION_PATCH 1)

# Set C++ standard - default to C++17 for modern GDAL compatibility
# This will be verified against GDAL requirements after GDAL detection
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure we have proper C++ compiler support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(NOT MSVC)
  if(NOT COMPILER_SUPPORTS_CXX17)
    message(WARNING "C++17 not supported, falling back to C++11. Some GDAL versions may not be compatible.")
    if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_STANDARD 11)
    else()
      message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 or C++17 support. Please use a different C++ compiler.")
    endif()
  endif()
endif()

# We need g++ >= version 4.7 (see
# http://stackoverflow.com/questions/4058565/check-gcc-minor-in-cmake)
if (CMAKE_COMPILER_IS_GNUCXX)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCXX_VERSION)
  if (GCXX_VERSION VERSION_LESS 4.7)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} is version ${GCXX_VERSION} which is not supported. Please use version 4.7 or greater.")
  endif()
endif()

# The tile size (width and height). According to the spec this should always be
# 65
set(TERRAIN_TILE_SIZE 65)
# The tile water mask size (width and height). According to the spec this
# should always be 256
set(TERRAIN_MASK_SIZE 256)

# Configure a header file to pass some of the CMake settings to the source code
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
  "${PROJECT_BINARY_DIR}/config.hpp"
  )

# Add the binary tree to the search path for include files so that we will find
# `config.hpp`
include_directories("${PROJECT_BINARY_DIR}")

# Ensure the configuration is installed
install(FILES ${PROJECT_BINARY_DIR}/config.hpp DESTINATION include/ctb)

# Perform as many checks as possible on debug builds:
#    cmake -DCMAKE_BUILD_TYPE=Debug ..
if(NOT MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra --pedantic")
  #set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra --pedantic")
endif()

FIND_PACKAGE (Threads)

# We need GDAL
find_package(GDAL REQUIRED)
if(NOT GDAL_FOUND)
  message(FATAL_ERROR "The GDAL library cannot be found on the system")
endif()

# Display GDAL version information
message(STATUS "Found GDAL version: ${GDAL_VERSION}")
message(STATUS "GDAL include dir: ${GDAL_INCLUDE_DIR}")
message(STATUS "GDAL libraries: ${GDAL_LIBRARIES}")

# Extract GDAL version components for compatibility checks
if(GDAL_VERSION)
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" GDAL_VERSION_MATCH ${GDAL_VERSION})
  set(GDAL_VERSION_MAJOR ${CMAKE_MATCH_1})
  set(GDAL_VERSION_MINOR ${CMAKE_MATCH_2})
  message(STATUS "GDAL major version: ${GDAL_VERSION_MAJOR}, minor version: ${GDAL_VERSION_MINOR}")
  
  # Check for supported GDAL versions
  if(GDAL_VERSION VERSION_LESS "3.1.0")
    message(WARNING "GDAL version ${GDAL_VERSION} is older than recommended minimum 3.1.0")
  endif()
  
  if(GDAL_VERSION VERSION_GREATER_EQUAL "4.0.0")
    message(WARNING "GDAL version ${GDAL_VERSION} is 4.x - compatibility review may be needed")
  endif()
  
  # Verify C++ standard compatibility with GDAL version
  if(GDAL_VERSION VERSION_GREATER_EQUAL "3.9.0" AND CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "GDAL ${GDAL_VERSION} requires C++17, but C++${CMAKE_CXX_STANDARD} is configured. Please ensure your compiler supports C++17.")
  endif()
  
  message(STATUS "Using C++${CMAKE_CXX_STANDARD} standard (compatible with GDAL ${GDAL_VERSION})")
endif()

include_directories(${GDAL_INCLUDE_DIR})

# Build and install libctb
include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

# Build and install libcommander
include_directories("${PROJECT_SOURCE_DIR}/deps")
add_subdirectory(deps)

# Build and install the tools
add_subdirectory(tools)
