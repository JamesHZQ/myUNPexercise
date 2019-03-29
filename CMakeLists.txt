cmake_minimum_required(VERSION 3.10)
project(myUNPexercise C)

set(CMAKE_C_STANDARD 99)

include_directories(usr/local/include)
include_directories(include)
link_directories(/usr/local/lib)

aux_source_directory(src DIR_SRCS)

add_executable(myUNPexercise ${DIR_SRCS})

target_link_libraries(${PROJECT_NAME} unp pthread)
