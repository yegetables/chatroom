# CMake 最低版本号要求
cmake_minimum_required (VERSION  3.20.2 )

# 项目信息
project (chatroom)



set( CMAKE_BUILD_TYPE Debug )
#set( CMAKE_BUILD_TYPE Release )

aux_source_directory(src RELY_FUNC_SRCS)
aux_source_directory(server SERVER_SRCS)
aux_source_directory(client CLIENT_SRCS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/server)
include_directories(${CMAKE_SOURCE_DIR}/client)

link_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/cjson)
link_directories(${CMAKE_SOURCE_DIR}/lib/zlog)
link_directories(${CMAKE_SOURCE_DIR}/lib/mysql)

set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#target_compile_definitions(server.out PUBLIC "${CMAKE_SOURCE_DIR}" )

# 指定生成目标 
add_executable(server.out ${SERVER_SRCS}  ${RELY_FUNC_SRCS} )
#add_executable(client.out ${CLIENT_SRCS}  ${RELY_FUNC_SRCS} )


configure_file(${CMAKE_SOURCE_DIR}/rely/config.h.in ${CMAKE_SOURCE_DIR}/server/config.h )
configure_file(${CMAKE_SOURCE_DIR}/rely/config.h.in ${CMAKE_SOURCE_DIR}/src/config.h )
configure_file(${CMAKE_SOURCE_DIR}/rely/config.h.in ${CMAKE_SOURCE_DIR}/client/config.h )

configure_file(${CMAKE_SOURCE_DIR}/rely/s.log.conf.in ${CMAKE_SOURCE_DIR}/log/s.log.conf )
#add_definitions(  -g -Wall -fsanitize=address -fno-omit-frame-pointer )

# 添加链接库
target_link_libraries(server.out cjson pthread zlog )
#target_link_libraries(client.out -lcjson -lpthread -lzlog -lmysqlclient )

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -fsanitize=address -fno-omit-frame-pointer ")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wall -L${CMAKE_SOURCE_DIR}/lib -lmysqlclient -I${CMAKE_SOURCE_DIR}/include/mysql ")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -Wall -L${CMAKE_SOURCE_DIR}/lib -lcjson -I${CMAKE_SOURCE_DIR}/include/")