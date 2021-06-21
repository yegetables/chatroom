aux_source_directory(${CMAKE_SOURCE_DIR}/src RELY_FUNC_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/client CLIENT_SRCS)


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/client)


link_directories(${CMAKE_SOURCE_DIR}/lib)


set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


add_executable(client.out  ${CLIENT_SRCS}  ${RELY_FUNC_SRCS} )

configure_file(${CMAKE_SOURCE_DIR}/rely/config.h.in ${CMAKE_SOURCE_DIR}/client/config.h )

add_definitions(   -DDEBUG -DPROJECT_CLIENT )
add_compile_options(-g -Wall  -std=c17 )



target_link_libraries(client.out cjson pthread zlog  )
