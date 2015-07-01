include_directories(./)

file(GLOB sources *.cpp)

add_library(openanim SHARED ${sources})

target_link_libraries(openanim ${LIBS})
