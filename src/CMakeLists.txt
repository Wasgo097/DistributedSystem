cmake_minimum_required(VERSION 3.25.1)

# Set the project name
project(DistributedSystem)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find all .cpp files in the current directory
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/*.h")

# Add the executable target
add_executable(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})