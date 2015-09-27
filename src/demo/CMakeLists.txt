include_directories(./)
include_directories(../)
include_directories(../common)

# Qt fun
SET(QT_USE_QTOPENGL TRUE)
FIND_PACKAGE(Qt4 REQUIRED)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})
set(LIBS ${LIBS} ${QT_LIBRARIES})
message(STATUS ${QT_LIBRARIES})

# looking for OpenGL libraries
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARY})

# looking for glut
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
set(LIBS ${LIBS} ${GLUT_LIBRARY})


file(GLOB headers_demo *.h ../common/viewport.h ../common/bind.h)
file(GLOB sources_demo *.cpp ../common/viewport.cpp ../common/bind.cpp)

set(headers ${headers_demo})
set(sources ${sources_demo})

QT4_WRAP_CPP(headers_moc ${headers})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(demo ${sources} ${headers_moc})

target_link_libraries(demo ${LIBS} openanim)
