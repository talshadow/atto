cmake_minimum_required(VERSION 3.30)

project(filter_service)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(${PROJECT_NAME}_SRC
  main.cpp
  tcpsender.cpp
  tcpsender.hpp
  udpreciver.cpp
  udpreciver.hpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
target_link_libraries(${PROJECT_NAME} atto_common_data)

