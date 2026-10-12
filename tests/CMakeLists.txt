# PACSCORDER development tests (not the canonical hardware tests of docs/TESTING.md).

add_executable(test_h264_level test_h264_level.c)
target_link_libraries(test_h264_level PRIVATE pacs_core)
add_test(NAME h264_level COMMAND test_h264_level)

# End-to-end run of the recorder with the dev backend (test source + software
# encoder). Proves the pipeline logic only; nothing about the CM4.
add_test(NAME smoke_record_dev
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/smoke_record.sh $<TARGET_FILE:pacscorder>)
set_tests_properties(smoke_record_dev PROPERTIES TIMEOUT 180)

# Live publishing (slice C2) through a private MediaMTX on localhost; skipped
# (exit 77) when mediamtx or ffprobe is not installed.
add_test(NAME smoke_publish_dev
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/smoke_publish.sh $<TARGET_FILE:pacscorder>)
set_tests_properties(smoke_publish_dev PROPERTIES TIMEOUT 180 SKIP_RETURN_CODE 77)
