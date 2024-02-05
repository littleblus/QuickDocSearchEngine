cmake_minimum_required(VERSION 3.27)
project(parser)

add_executable(parser parser.cpp html_util.cpp)

target_compile_features(parser PRIVATE cxx_std_20)
target_link_libraries(parser PRIVATE stdc++fs)
