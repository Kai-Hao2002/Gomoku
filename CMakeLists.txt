cmake_minimum_required(VERSION 3.10)
project(GomokuProject)

set(CMAKE_CXX_STANDARD 17)

# 包含頭文件
include_directories(include)

# 添加源文件
file(GLOB SOURCES "src/*.cpp" "main_gui.cpp")

# 設置可執行檔案
add_executable(main ${SOURCES})

# 查找 SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# 將 SFML 與你的項目鏈接
target_link_libraries(main sfml-graphics sfml-window sfml-system)
