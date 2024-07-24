cmake_minimum_required(VERSION 3.12)
project(luapy)

set(CMAKE_CXX_STANDARD 11)

find_package(pybind11 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua5.4)

include_directories(${LUA_INCLUDE_DIRS})
link_directories(${LUA_LIBRARY_DIRS})

pybind11_add_module(luapy src/main.cpp)
target_link_libraries(luapy PRIVATE ${LUA_LIBRARIES})
