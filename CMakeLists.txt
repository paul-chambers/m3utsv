cmake_minimum_required(VERSION 3.5)
project(m3upipe C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable( m3upipe m3upipe.c m3upipe.h )

install( TARGETS m3upipe RUNTIME DESTINATION /usr/bin )

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Paul-Chambers") #required
INCLUDE(CPack)
