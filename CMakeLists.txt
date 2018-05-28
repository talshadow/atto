cmake_minimum_required(VERSION 3.7)

project(atto_io_system)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

add_subdirectory(common)
add_subdirectory(filter_service)
add_subdirectory(reciver)
add_subdirectory(senders)
add_subdirectory(data_generator)

add_dependencies(filter_service atto_common_data)
add_dependencies(reciver atto_common_data)
add_dependencies(senders atto_common_data)

