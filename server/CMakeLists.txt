aux_source_directory(${CMAKE_SOURCE_DIR}/src RELY_FUNC_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/server SERVER_SRCS)


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/server)


link_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/cjson)
link_directories(${CMAKE_SOURCE_DIR}/lib/zlog)


set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


add_executable(server.out ${SERVER_SRCS}  ${RELY_FUNC_SRCS} )


configure_file(${CMAKE_SOURCE_DIR}/rely/config.h.in ${CMAKE_SOURCE_DIR}/server/config.h )


add_definitions(  -g -Wall -DDEBUG -Werror  )
# -fsanitize=address -fno-omit-frame-pointer



target_link_libraries(server.out cjson pthread zlog  mysqlclient)