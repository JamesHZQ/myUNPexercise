cmake_minimum_required(VERSION 3.10)
project(testOOB C)

set(CMAKE_C_STANDARD 99)

INCLUDE_DIRECTORIES(usr/local/include)
link_directories(/usr/local/lib)

add_executable(testOOB tcpserv2.c heartbeatcli.c heartbeatserv.c)

target_link_libraries(${PROJECT_NAME} unp pthread)
