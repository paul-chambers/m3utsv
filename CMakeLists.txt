cmake_minimum_required(VERSION 3.5)
project(m3u2json C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable( m3u2json
                m3u2json.c      m3u2json.h
)

target_link_libraries( m3u2json cjson )
