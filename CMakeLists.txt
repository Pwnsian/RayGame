cmake_minimum_required(VERSION 3.0.0)
project(RayGame VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)

add_executable(RayGame main.cpp map.cpp render.cpp texture.cpp sprite.cpp)

target_include_directories(RayGame PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(RayGame ${SDL2_LIBRARIES})