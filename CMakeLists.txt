cmake_minimum_required(VERSION 3.20)
project(SDL)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-std=c++17 -g -Wall -Wextra)

# set(SDL2_INCLUDES "C:/Users/jaxro/OneDrive/SophmoreYear/Game_Dev/code-organization/SDL-SDL2/SDL-SDL2/include")
# set(SDL2_LIBRARIES "C:/Users/jaxro/OneDrive/SophmoreYear/Game_Dev/code-organization/SDL2-2.26.2-win32-x64/SDL2.dll")

find_package(SDL2 REQUIRED)

add_library(gamelib
  graphics.cpp
  player.cpp
  world.cpp
  vec.cpp
 # camera.cpp
  tilemap.cpp
)



target_include_directories(gamelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} $ {SDL2_INCLUDES})
target_link_libraries(gamelib PUBLIC SDL2::SDL2)


add_executable(main main.cpp)
target_link_libraries(main PUBLIC gamelib)