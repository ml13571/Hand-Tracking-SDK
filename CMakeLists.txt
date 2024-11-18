cmake_minimum_required(VERSION 3.10.2)

project(Hand CXX)
set(CMAKE_CXX_STANDARD 14)

set(3RD_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/source)

add_subdirectory(${3RD_DIR}/MNN-2.3.0)

find_package(OpenCV REQUIRED)
include_directories(
    ${SRC_DIR}
    ${3RD_DIR}/MNN-2.3.0/include
    ${OpenCV_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
    STATIC 
    ${SRC_DIR}/PalmDetector.cpp
    ${SRC_DIR}/PalmLandmarkDetector.cpp
    ${SRC_DIR}/Utils.cpp
    ${SRC_DIR}/hand.cpp)

target_link_libraries(${PROJECT_NAME} MNN ${OpenCV_LIBS})

add_executable(test 
    examples/test.cpp)
    
target_link_libraries(test ${PROJECT_NAME})

