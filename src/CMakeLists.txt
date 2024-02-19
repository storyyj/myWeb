# 将网络库的源码制作成一个动态库，供基于该网络库的各项上层开发调用
cmake_minimum_required(VERSION 3.5)
project(myNetKu)
include_directories(${PROJECT_SOURCE_DIR})
# 搜索指定目录下源文件
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(mynetku SHARED ${SRC_LIST})

