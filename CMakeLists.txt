cmake_minimum_required(VERSION 3.5)
project(m3utsv C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable( m3utsv
        m3utsv.c m3utsv.h
        btree/btree.c btree/btree.h
        hashmap/hashmap.c hashmap/hashmap.h)

install( TARGETS m3utsv RUNTIME DESTINATION /usr/bin )

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Paul-Chambers") #required
INCLUDE(CPack)
