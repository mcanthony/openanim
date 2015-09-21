include_directories(./)

file(GLOB_RECURSE sources *.cpp Loaders/*.cpp)

add_library(openanim SHARED ${sources})

target_link_libraries(openanim ${LIBS})
