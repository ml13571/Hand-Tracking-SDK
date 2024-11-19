cmake_minimum_required(VERSION 3.10.2)

project(test CXX)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(
    ${SRC_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

add_library(libHand
    STATIC 
    IMPORTED)
set_target_properties(libHand PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libHand.a)

add_executable(${PROJECT_NAME}
    ${SRC_DIR}/test.cpp)
    
target_link_libraries(${PROJECT_NAME}
    libHand
    ${OpenCV_LIBS}
    -pthread)

