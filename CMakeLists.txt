cmake_minimum_required(VERSION 3.10)
project(Gomoku VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 根据操作系统设置特定选项
if(WIN32)
    add_definitions(-DWINDOWS)
    # Windows下清除屏幕命令不同
else()
    add_definitions(-DLINUX)
    find_package(Threads REQUIRED)
endif()

# 可执行文件
add_executable(gomoku
    main.cpp
    gomoku.cpp
)

# 链接线程库
if(UNIX AND NOT APPLE)
    target_link_libraries(gomoku pthread)
endif()

# 设置输出目录
set_target_properties(gomoku PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装目标
install(TARGETS gomoku
    RUNTIME DESTINATION bin
)