file(GLOB SOURCES CONFIGURE_DEPENDS *.h *.cpp)
add_library(stb ${SOURCES} )

target_include_directories(stb PUBLIC ./)