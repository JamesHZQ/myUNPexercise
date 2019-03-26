cmake_minimum_required(VERSION 3.10)
project(myUNPexercise C)

set(CMAKE_C_STANDARD 99)

INCLUDE_DIRECTORIES(usr/local/include)
link_directories(/usr/local/lib)

add_executable(myUNPexercise main.c)

target_link_libraries(${PROJECT_NAME} unp pthread)
