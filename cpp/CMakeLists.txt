cmake_minimum_required(VERSION 3.12)
project(stereoMatch)

set(CMAKE_BUILD_TYPE Debug)
set(SOURCE_DIR "src")
set(INCLU_DIR "include")

set(TEST_SOURCE_FILES
    #googleTest/blockMatchTest.cpp
    googleTest/imageTest.cpp
)

set(TEST_TARGET_NAME imageTest)
# 查找 OpenCV 包
find_package(OpenCV REQUIRED)

file(GLOB HEADE_FILES ${INCLU_DIR}/*.h)
file(GLOB SOURC_FILES ${SOURCE_DIR}/*.cpp)

# 添加可执行文件
#add_executable(sliding_window_test main.cpp ${SOURC_FILES})
add_executable(stereoMatch ${TEST_SOURCE_FILES} ${SOURC_FILES})

target_include_directories(stereoMatch PUBLIC ${INCLU_DIR})
# 链接 OpenCV 库
target_link_libraries(stereoMatch ${OpenCV_LIBS} gtest gtest_main)

add_test(NAME ${TEST_TARGET_NAME} COMMAND ${TEST_TARGET_NAME})