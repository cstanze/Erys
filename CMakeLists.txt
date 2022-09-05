cmake_minimum_required(VERSION 3.19.6)

project(
  Erys
  C
  CXX
)

# Don't clobber existing variable values when
# evaluating `option()` declarations.
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

option(ERYS_CROSS_COMPILE "Enable cross-compiling" OFF)
set(ERYS_CROSS_COMPILE_TARGET "arm-linux-gnueabihf" CACHE STRING "Target platform")
set(ERYS_CROSS_COMPILE_SYSTEM "Linux" CACHE STRING "Target system")
set(ERYS_CROSS_COMPILE_PROCESSOR "arm" CACHE STRING "Target processor")
set(ERYS_CROSS_COMPILE_SYSROOT "/usr/arm-linux-gnueabihf" CACHE STRING "Target sysroot")

option(ERYS_DEBUG "Enable debug build" ON)

if(DEFINED ENV{PREFIX_DIR} AND NOT "$ENV{PREFIX_DIR}" STREQUAL "" AND NOT EXISTS "${ERYS_CROSS_COMPILE}")
  set(CMAKE_INSTALL_PREFIX "$ENV{PREFIX_DIR}")
else()
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.erys")
endif()
set(CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX};${CMAKE_BINARY_DIR}")
add_definitions(-DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
message(STATUS "Using prefix: ${CMAKE_INSTALL_PREFIX}")

# Module setup
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/modules")

include(QuarkTargets)
include(CPM)

# macOS RPath fix
set(ORIGIN "$ORIGIN")
if(APPLE)
  set(ORIGIN "@loader_path")
endif()
set(CMAKE_INSTALL_RPATH "${ORIGIN}/../lib;${ORIGIN}/../lib;${ORIGIN}/.")
set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_MACOSX_RPATH ON)

# C++ Standard
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
    "Build type for Erys [Debug, RelWithDebInfo, Release]"
    FORCE)
  message(STATUS "Build type not specified, defaulting to ${CMAKE_BUILD_TYPE}")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES ".*BSD.*")
  add_definitions(-D_WITH_GETLINE)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  message(STATUS "Building with debug info...")
  add_definitions(-DDEBUG)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  message(STATUS "Building with release info...")
  add_definitions(-DRELEASE)
endif()

# Cross-compiling
if("${ERYS_CROSS_COMPILE}")
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "Cross-compiling requires Clang")
  endif()

  message(STATUS "Cross-compiling for ${ERYS_CROSS_COMPILE_TARGET}...")
  message(STATUS "System: ${ERYS_CROSS_COMPILE_SYSTEM}")
  message(STATUS "Processor: ${ERYS_CROSS_COMPILE_PROCESSOR}")
  message(STATUS "Sysroot: ${ERYS_CROSS_COMPILE_SYSROOT}")

  # Set system name, processor, and sysroot
  set(CMAKE_SYSTEM_NAME ${ERYS_CROSS_COMPILE_SYSTEM})
  set(CMAKE_SYSTEM_PROCESSOR ${ERYS_CROSS_COMPILE_PROCESSOR})
  set(CMAKE_SYSROOT ${ERYS_CROSS_COMPILE_SYSROOT})

  # Set compiler and target
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_CXX_COMPILER clang++)
  set(CMAKE_C_COMPILER_TARGET ${ERYS_CROSS_COMPILE_TARGET}) 
  set(CMAKE_CXX_COMPILER_TARGET ${ERYS_CROSS_COMPILE_TARGET})
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Config
option(ERYS_ENABLE_TESTS "Enable tests" OFF)
option(ERYS_ENABLE_COMPILE_COMMANDS "Enable compile_commands.json" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ${ERYS_ENABLE_COMPILE_COMMANDS})

set(ERYS_VERSION "0.0.1")

execute_process(COMMAND date +"%Y-%m-%d" OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)

# Check if the directory is a git repository
execute_process(COMMAND git rev-parse --is-inside-work-tree
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_REPO
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GIT_REPO)
  # Get git revision
  execute_process(COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Get git branch
  execute_process(COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

configure_file(
  ${CMAKE_SOURCE_DIR}/include/Config.hpp.in
  ${CMAKE_SOURCE_DIR}/include/Config.hpp
  @ONLY
)

# Pthread
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THRADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
  message(STATUS "Using thread flags: -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
else()
  message(STATUS "Using thread flags: ${CMAKE_THREAD_LIBS_INIT}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
endif()

# ccache
CPMAddPackage(
  NAME Ccache.cmake
  GITHUB_REPOSITORY TheLartians/Ccache.cmake
  VERSION 1.2
)

CPMAddPackage(
  NAME fmt
  GIT_TAG 6.1.2
  GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
  NAME magic_enum
  GITHUB_REPOSITORY Neargye/magic_enum
  GIT_TAG v0.8.1
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND NOT "${USE_CCACHE}" EQUAL "OFF")
  set(USE_CCACHE ON)
  set(CCACHE_OPTIONS "--show-stats")
endif()

# Install CMake modules
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/modules/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/modules
)

# Install includes
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

# Dependencies
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message(STATUS "LLVM Include Dirs: ${LLVM_INCLUDE_DIRS}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# TODO: Move vendor dependencies to use CPM
add_subdirectory(vendor)

add_subdirectory(lib)
