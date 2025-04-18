cmake_minimum_required(VERSION 3.30)

project(atto_common_data CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

set(${PROJECT_NAME}_SRC types.cpp threadpool.cpp udpsocket.cpp tcpsocket.cpp)
set(${PROJECT_NAME}_HDR threadpool.hpp types.hpp tcpsocket.hpp udpsocket.hpp messagesstore.hpp sharedmutex.hpp)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_HDR} ${${PROJECT_NAME}_SRC})
target_link_libraries(${PROJECT_NAME}
  Boost::boost
  Boost::system
  Threads::Threads
  )
