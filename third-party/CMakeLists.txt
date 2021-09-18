cmake_minimum_required(VERSION 3.0)

add_definitions("-std=c++11 -pthread")

########################################################################
# RabbitMQ

include(ExternalProject)
set(AMQPCPP_ROOT "${CMAKE_CURRENT_BINARY_DIR}/amqpcpp")
set(AMQPCPP_SSL_CACERTS "${PROJECT_BINARY_DIR}/ssl/ca/cacert.pem" CACHE STRING "")
set(AMQPCPP_SSL_CERTFILE "${PROJECT_BINARY_DIR}/ssl/client/rabbitmq_client.cert.pem" CACHE STRING "")
set(AMQPCPP_SSL_KEYFILE "${PROJECT_BINARY_DIR}/ssl/client/rabbitmq_client.key.pem" CACHE STRING "")
ExternalProject_Add(libamqpcpp
    # URL "https://github.com/CopernicaMarketingSoftware/AMQP-CPP/archive/v4.1.5.tar.gz"
    # URL "${CMAKE_CURRENT_SOURCE_DIR}/libamqpcpp-4.1.5.tar.gz"
    # URL_MD5 "66fa995d432adade65ea6274799f2bed"
    URL "${CMAKE_CURRENT_SOURCE_DIR}/libamqpcpp-4.1.5-ssl-fixed.tar.gz"
    URL_MD5 "50004A5B7CB8CFBBCF2F345BE8449DC4"
    CMAKE_ARGS -Wno-dev 
        -DCMAKE_INSTALL_PREFIX=${AMQPCPP_ROOT}
        -DAMQP-CPP_LINUX_TCP=ON
        -DAMQP-CPP_SSL_CACERTS=${AMQPCPP_SSL_CACERTS}
        -DAMQP-CPP_SSL_CERTFILE=${AMQPCPP_SSL_CERTFILE}
        -DAMQP-CPP_SSL_KEYFILE=${AMQPCPP_SSL_KEYFILE}
)
file(MAKE_DIRECTORY ${AMQPCPP_ROOT}/include)
message(STATUS "Found external dependency AMQP-CPP: " ${AMQPCPP_ROOT}/include)
add_library(amqpcpp-static STATIC IMPORTED GLOBAL)
set_property(TARGET amqpcpp-static PROPERTY IMPORTED_LOCATION ${AMQPCPP_ROOT}/lib/libamqpcpp.a)
set_property(TARGET amqpcpp-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${AMQPCPP_ROOT}/include)

########################################################################


########################################################################
# OpenSSL

set(OpenSSL_ROOT "${CMAKE_CURRENT_BINARY_DIR}/openssl")
ExternalProject_Add(openssl
  # URL "https://www.openssl.org/source/openssl-1.1.0h.tar.gz"
  URL "${CMAKE_CURRENT_SOURCE_DIR}/openssl-1.1.0h.tar.gz"
  URL_MD5 "5271477e4d93f4ea032b665ef095ff24"
  CONFIGURE_COMMAND <SOURCE_DIR>/Configure linux-x86_64 --prefix=<INSTALL_DIR>
    --openssldir=<INSTALL_DIR>/lib/ssl no-weak-ssl-ciphers enable-ec_nistp_64_gcc_128 no-shared
  # BUILD_COMMAND make "-ldl"
  INSTALL_DIR ${OpenSSL_ROOT}
)
file(MAKE_DIRECTORY ${OpenSSL_ROOT}/include)
message(STATUS "Found external dependency OpenSSL: " ${OpenSSL_ROOT}/include)
add_library(openssl-crypto-static STATIC IMPORTED GLOBAL)
set_property(TARGET openssl-crypto-static PROPERTY IMPORTED_LOCATION ${OpenSSL_ROOT}/lib/libcrypto.a)
# set_property(TARGET openssl-crypto-static PROPERTY IMPORTED_INTERFACE_LINK_LIBRARIES pthread)
set_property(TARGET openssl-crypto-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OpenSSL_ROOT}/include)
add_library(openssl-ssl-static STATIC IMPORTED GLOBAL)
set_property(TARGET openssl-ssl-static PROPERTY IMPORTED_LOCATION ${OpenSSL_ROOT}/lib/libssl.a)
# set_property(TARGET openssl-ssl-static PROPERTY IMPORTED_INTERFACE_LINK_LIBRARIES pthread)
set_property(TARGET openssl-ssl-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OpenSSL_ROOT}/include)

########################################################################


########################################################################
# LibEvent

set(Event_ROOT "${CMAKE_CURRENT_BINARY_DIR}/event")
ExternalProject_Add(libevent
  # URL "https://github.com/libevent/libevent/archive/release-2.0.22-stable.tar.gz"
  # URL_MD5 "8913ef56ec329f2c046007bd634c7201"
  URL "${CMAKE_CURRENT_SOURCE_DIR}/libevent-2.1.11-stable.tar.gz"
  URL_MD5 "7f35cfe69b82d879111ec0d7b7b1c531"
  PATCH_COMMAND <SOURCE_DIR>/autogen.sh
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --disable-shared --prefix <INSTALL_DIR>
    OPENSSL_CFLAGS=-I${OpenSSL_ROOT}/include OPENSSL_LIBS="-L${OpenSSL_ROOT}/lib -lssl -lcrypto -ldl"
  INSTALL_DIR ${Event_ROOT}
)
file(MAKE_DIRECTORY ${Event_ROOT}/include)
message(STATUS "Found external dependency Event: " ${Event_ROOT}/include)
add_library(event-static STATIC IMPORTED GLOBAL)
set_property(TARGET event-static PROPERTY IMPORTED_LOCATION ${Event_ROOT}/lib/libevent.a)
set_property(TARGET event-static PROPERTY IMPORTED_INTERFACE_LINK_LIBRARIES pthread crypto)
set_property(TARGET event-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Event_ROOT}/include)

add_library(event-openssl-static STATIC IMPORTED GLOBAL)
set_property(TARGET event-openssl-static PROPERTY IMPORTED_LOCATION ${Event_ROOT}/lib/libevent_openssl.a)
set_property(TARGET event-openssl-static PROPERTY IMPORTED_INTERFACE_LINK_LIBRARIES pthread crypto)
set_property(TARGET event-openssl-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Event_ROOT}/include)

add_library(event-pthreads-static STATIC IMPORTED GLOBAL)
set_property(TARGET event-pthreads-static PROPERTY IMPORTED_LOCATION ${Event_ROOT}/lib/libevent_pthreads.a)
set_property(TARGET event-pthreads-static PROPERTY IMPORTED_INTERFACE_LINK_LIBRARIES pthread crypto)
set_property(TARGET event-pthreads-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Event_ROOT}/include)

########################################################################
