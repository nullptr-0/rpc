project(rpcc
    DESCRIPTION "code generator for rpc framework")

include(rpcc_sources.cmake)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release")
endif()

add_executable(rpcc
    ${RPCC_SOURCES}
)
