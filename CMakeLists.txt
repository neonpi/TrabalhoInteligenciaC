cmake_minimum_required(VERSION 3.28)
cmake_policy(SET CMP0167 OLD) # change this in the future when cmake 3.30 becomes more ubiquitous
project(trabalho-inteligencia-computacional VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

add_executable(trabalho-inteligencia-computacional
    src/main.cpp
)
target_include_directories(trabalho-inteligencia-computacional PUBLIC "${PROJECT_SOURCE_DIR}/include")
