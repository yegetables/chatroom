# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.20.2)

# 项目信息
project (server.out)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(rely/func RELY_FUNC_SRCS)
aux_source_directory(rely/includec RELY_INCLUDEC_SRCS)
aux_source_directory(server SERVER_SRCS)

# 添加 math 子目录
# add_subdirectory(math)

# 指定生成目标 
add_executable(server.out ${SERVER_SRCS} ${RELY_INCLUDEC_SRCS} ${RELY_FUNC_SRCS} )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -fsanitize=address -fno-omit-frame-pointer ")
# SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O2  -Wall")
# SET(CMAKE_BUILE_TYPE "Debug")

# 
# add_compile_options(-Wall -fsanitize=address -fno-omit-frame-pointer -g)
# add_compile_options(-Wall  -g)

# 添加链接库
target_link_libraries(server.out -lcjson -lpthread -lzlog )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)