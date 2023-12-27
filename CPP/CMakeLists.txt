cmake_minimum_required(VERSION 3.12)
project(sliding_window_test)

set(CMAKE_BUILD_TYPE Debug)
set(SOURCE_DIR "src")
set(INCLU_DIR "include")


# 查找 OpenCV 包
find_package(OpenCV REQUIRED)

file(GLOB HEADE_FILES ${INCLU_DIR}/*.h)
file(GLOB SOURC_FILES ${SOURCE_DIR}/*.cpp)

# 添加可执行文件
add_executable(sliding_window_test main.cpp ${SOURC_FILES})

target_include_directories(sliding_window_test PUBLIC ${INCLU_DIR})
# 链接 OpenCV 库
target_link_libraries(sliding_window_test ${OpenCV_LIBS})