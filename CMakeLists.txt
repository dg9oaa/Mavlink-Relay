cmake_minimum_required(VERSION 3.10)

project(mavrelayclient C)

set(CMAKE_C_STANDARD 99)


set(SOURCES
    src/main.c
    src/serial.c
    src/udp.c
    src/option.c
    src/logging.c
    cJSON/cJSON.c
)

string(TIMESTAMP VERSION_STRING "%Y%m%d_%H%M%S")
# set(VERSION_HEADER "${CMAKE_BINARY_DIR}/version.h")
set(VERSION_HEADER "src/version.h")
# Generiere eine Header-Datei mit der Versionsinfo
file(WRITE ${VERSION_HEADER} "#pragma once\n")
file(APPEND ${VERSION_HEADER} "#define VERSION_STRING \"${VERSION_STRING}\"\n")


# target_include_directories(${PROJECT_NAME} PRIVATE mavlink/include/mavlink/v2.0)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Os)
target_link_options(${PROJECT_NAME} PRIVATE -s)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include mavlink/include/mavlink/v2.0 ${PROJECT_SOURCE_DIR}/cJSON)
