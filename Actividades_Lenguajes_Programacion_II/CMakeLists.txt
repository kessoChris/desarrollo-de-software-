cmake_minimum_required(VERSION 3.21)
project(empleados_odbc LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Este proyecto usa el administrador ODBC de Windows.")
endif()

add_executable(empleados_odbc src/main.cpp)
target_compile_features(empleados_odbc PRIVATE cxx_std_17)
target_compile_definitions(empleados_odbc PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(empleados_odbc PRIVATE odbc32)

