cmake_minimum_required(VERSION 3.0.0)
project(sdltris VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)

add_executable(sdltris main.c)
set_property(TARGET sdltris PROPERTY C_STANDARD 23)


target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
