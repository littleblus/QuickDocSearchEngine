cmake_minimum_required(VERSION 3.27)
project(search_engined)

add_executable(parser src/parser/parser.cpp src/parser/html_util.cpp)
add_executable(http_server src/http_server.cpp)

# 对全部文件使用c++20标准
target_compile_features(parser PRIVATE cxx_std_20)
target_compile_features(http_server PRIVATE cxx_std_20)

# 链接库
target_link_libraries(parser PRIVATE stdc++fs)
target_link_libraries(http_server PRIVATE pthread jsoncpp stdc++fs)