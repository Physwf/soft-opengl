file(GLOB_RECURSE DIR_SRC
    "./*.cpp"
    "./*.h"
)
add_library(libsoftgl SHARED ${DIR_SRC})
set_target_properties(libsoftgl PROPERTIES FOLDER "Libraries")
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


include(GenerateExportHeader)
generate_export_header(libsoftgl 
						BASE_NAME SGL
						EXPORT_MACRO_NAME SGL_API
						EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/src/SGL.h
						)