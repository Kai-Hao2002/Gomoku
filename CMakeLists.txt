cmake_minimum_required(VERSION 3.10)
project(GomokuProject)

set(CMAKE_CXX_STANDARD 17)

# Include header files
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp" "main.cpp")

# 可加上 -pthread 開啟平行運算支援
add_executable(main ${SOURCES})

# 如果之後加 UI (SFML)
# find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
# target_link_libraries(main sfml-graphics sfml-window sfml-system)
