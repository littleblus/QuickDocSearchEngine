cmake_minimum_required(VERSION 3.27)
project(search_engined)

add_executable(parser src/parser/parser.cpp src/parser/html_util.cpp)
add_executable(test src/server.cpp)

# 对全部文件使用c++20标准
target_compile_features(parser PRIVATE cxx_std_20)
target_compile_features(test PRIVATE cxx_std_20)

# 链接库
target_link_libraries(test PRIVATE stdc++fs jsoncpp)
target_link_libraries(parser PRIVATE stdc++fs)
