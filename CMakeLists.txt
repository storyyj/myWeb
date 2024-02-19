cmake_minimum_required(VERSION 3.5)
project(myServer)
# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/src)
# 获取main代码的cpp文件
file(GLOB SRC_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp)
file(GLOB SRC_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)
# 设置动态库路径
set(LIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 链接子节点
add_subdirectory(src)
# 包含动态库路径
link_directories(${LIBPATH})
# 链接静态库
# link_libraries(calc)
add_executable(myserver ${SRC_SERVER})
add_executable(myclient ${SRC_CLIENT})
target_link_libraries(myserver mynetku)
target_link_libraries(myclient mynetku)
