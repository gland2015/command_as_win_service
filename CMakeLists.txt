cmake_minimum_required(VERSION 3.0.0)
project(win_services_project VERSION 0.1.0)

if (MSVC) 
    add_compile_options(/utf-8)
endif()

# base - 设置c++标准为c17
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

# 包含自己的头文件
include_directories(./src/include)
# 添加源文件
file (GLOB srcCode 
    "./src/main.cpp"
    "./src/utils.cpp"
    "./src/args.cpp"
)
add_executable(win_services ${srcCode})

# 添加gflags命令行解析工具
find_package(gflags CONFIG REQUIRED)
target_link_libraries(win_services PRIVATE gflags::gflags)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
