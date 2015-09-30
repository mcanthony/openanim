include_directories(./)
include_directories(../)

file(GLOB sources *.cpp)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(tests ${sources})

target_link_libraries(tests ${LIBS} openanim)
