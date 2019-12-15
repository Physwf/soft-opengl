file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)

add_executable(samples ${DIR_SRC})

target_link_libraries(samples libsoftgl)
add_dependencies(samples libsoftgl)
include_directories(../src)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")