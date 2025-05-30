cmake_minimum_required(VERSION 3.10)

project(mavlink_udp_client C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
    src/main.c
    src/serial.c
    src/udp.c
    src/option.c
)

# target_include_directories(${PROJECT_NAME} PRIVATE mavlink/include/mavlink/v2.0)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include mavlink/include/mavlink/v2.0)
