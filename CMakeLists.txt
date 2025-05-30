cmake_minimum_required(VERSION 3.10)
project(mavlink_udp_client C)

set(CMAKE_C_STANDARD 99)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/mavlink
)

add_executable(mavlink_udp_client
    src/main.c
    src/serial.c
    src/udp.c
    src/option.c
)

