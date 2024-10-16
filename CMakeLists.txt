cmake_minimum_required(VERSION 3.22.1)

project(DistributedSystem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system filesystem serialization)

include_directories(src/include)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(Client/src)
add_subdirectory(Server/src)