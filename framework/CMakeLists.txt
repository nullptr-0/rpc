cmake_minimum_required(VERSION 3.10)

project(rpc
    DESCRIPTION "rpc framework & examples")

set(CMAKE_CXX_STANDARD 23)

#-----------------------------------------------------------------------------
# Include cmake modules
#-----------------------------------------------------------------------------
include(rpc_sources.cmake)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release")
endif()

#-----------------------------------------------------------------------------
# Project targets
#-----------------------------------------------------------------------------
add_executable(rpc-server
    ${RPC_SERVER_SOURCES}
    ${RPC_SERVER_HEADERS}
)

add_executable(rpc-client
    ${RPC_CLIENT_SOURCES}
    ${RPC_CLIENT_HEADERS}
)
