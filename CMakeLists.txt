cmake_minimum_required(VERSION 3.0.0)
project(win_services VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

include_directories(./src/include)
file (GLOB srcCode 
    "./src/main.cpp"
)
add_executable(win_services ${srcCode})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)




