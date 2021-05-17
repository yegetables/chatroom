# CMake 最低版本号要求
cmake_minimum_required (VERSION  3.20.2 )

# 项目信息
project (chatroom)
#CMAKE_SOURCE_DIR	工程顶层目录，即入口CMakeLists文件所在路径
#PROJECT_SOURCE_DIR	同CMAKE_SOURCE_DIR

#CMAKE_BINARY_DIR	工程编译发生的目录，即执行cmake命令进行项目配置的目录，一般为build
#PROJECT_BINARY_DIR	同CMAKE_BINARY_DIR

set( CMAKE_BUILD_TYPE Debug )
#set( CMAKE_BUILD_TYPE Release )

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -fsanitize=address -fno-omit-frame-pointer ")

aux_source_directory(src RELY_FUNC_SRCS)
aux_source_directory(server SERVER_SRCS)
aux_source_directory(client CLIENT_SRCS)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# 指定生成目标 
add_executable(server.out ${SERVER_SRCS}  ${RELY_FUNC_SRCS} )
add_executable(client.out ${CLIENT_SRCS}  ${RELY_FUNC_SRCS} )

# 添加链接库
target_link_libraries(server.out -lcjson -lpthread -lzlog )
target_link_libraries(client.out -lcjson -lpthread -lzlog )
