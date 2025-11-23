cmake_minimum_required(VERSION 3.10)
project(DitheringModule CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(pybind11 REQUIRED)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV encontrado. Versión: ${OpenCV_VERSION}")

pybind11_add_module(
    dither_cpp                
    SHARED
    ordered.cpp             
)

target_link_libraries(dither_cpp 
    PRIVATE 
       
        ${OpenCV_LIBS}         
        pybind11::module       
)