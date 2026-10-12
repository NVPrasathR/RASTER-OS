# PACSCORDER application (ADR-007 GStreamer, ADR-013 C).

# Pure-C helpers with no GStreamer dependency, unit-tested on any machine.
add_library(pacs_core STATIC src/h264_level.c)
target_include_directories(pacs_core PUBLIC src)
target_compile_options(pacs_core PRIVATE -Wall -Wextra)

add_executable(pacscorder src/main.c src/config.c src/pipeline.c)
target_link_libraries(pacscorder PRIVATE pacs_core PkgConfig::GST)
target_compile_options(pacscorder PRIVATE -Wall -Wextra)

install(TARGETS pacscorder RUNTIME DESTINATION bin)
