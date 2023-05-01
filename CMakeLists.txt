cmake_minimum_required(VERSION 3.13)

project(interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_FLAGS "-std=c++17")

add_executable(${PROJECT_NAME} 
  src/main.cpp
  src/fs.cpp
  src/node.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++17 -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wunused-variable -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fstack-protector  -DDEBUG" `pkg-config fuse --cflags --libs`)

# generate compiler commands to help clangd find include paths
# even if not set, there is no error in compilation and linking but neovim diagnostics shows error
set(DCMAKE_EXPORT_COMPILE_COMMANDS 1 CACHE INTERNAL "")
set(DCMAKE_BUILD_TYPE Debug)

