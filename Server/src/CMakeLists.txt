cmake_minimum_required(VERSION 3.22.1)

# Set the project name
project(ServerApp)

# Specify the C++ standard
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find all .cpp files in the current directory
#file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
#file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/*.h")

# Add the executable target
add_executable(${PROJECT_NAME} ServerApp.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)
target_link_libraries(${PROJECT_NAME}  ${Boost_LIBRARIES})