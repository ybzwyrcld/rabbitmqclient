cmake_minimum_required(VERSION 3.0)

project(rabbitmqclient)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 9)
set (VERSION_PATCH 0)

option(RABBITMQCLIENT_BUILD_EXAMPLES "Build rabbitmqclient examples" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("-- Using default build type: Debug.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O2 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_INCLUDE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/include)

set(rabbitmqclient_include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(SYSTEM ${rabbitmqclient_include_dirs})

add_subdirectory(third-party)
aux_source_directory(src DIR_SRCS)

add_library(${PROJECT_NAME}
  ${DIR_SRCS}
)

add_dependencies(${PROJECT_NAME} libamqpcpp openssl libevent)

target_link_libraries(${PROJECT_NAME} PRIVATE
  openssl-ssl-static
  openssl-crypto-static
  amqpcpp-static
  pthread
  event-static
  event-openssl-static
  event-pthreads-static
  dl
)

# Install options.

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Config ARCHIVE DESTINATION lib
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/rabbitmq_client/rabbitmq_client.h
  DESTINATION include
)

install(EXPORT ${PROJECT_NAME}Config DESTINATION cmake)
export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)

set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
  $<BUILD_INTERFACE:${rabbitmqclient_include_dirs}>
  $<INSTALL_INTERFACE:${CMAKE_OUTPUT_INCLUDE_DIR}>
)

if (RABBITMQCLIENT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (RABBITMQCLIENT_BUILD_EXAMPLES)
