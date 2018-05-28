cmake_minimum_required(VERSION 3.7)

project(senders)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(${PROJECT_NAME}_SRC
  sender_data1.hpp
  sender_data2.hpp
  sender_data3.hpp
  sender_data4.hpp
  udpsender.hpp
  udpsender.cpp
  main.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
target_link_libraries(${PROJECT_NAME}
  atto_common_data
  Threads::Threads)


