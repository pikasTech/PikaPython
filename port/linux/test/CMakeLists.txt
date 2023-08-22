# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)

set(BINARY ${CMAKE_PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp *.c)
set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

# if(supported) 
#  message(STATUS "IPO / LTO enabled") 
#  set_property(TARGET ${BINARY} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# else() 
#  message(STATUS "IPO / LTO not supported: <${error}>")
# endif()




# 链接src生成的lib库和gtest库
target_link_libraries(${BINARY} PUBLIC 
        -Wl,--whole-archive ${CMAKE_PROJECT_NAME}-core -Wl,--no-whole-archive
        gtest)
