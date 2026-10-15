# PACSCORDER development tests (not the canonical hardware tests of docs/TESTING.md).

add_executable(test_h264_level test_h264_level.c)
target_link_libraries(test_h264_level PRIVATE pacs_core)
add_test(NAME h264_level COMMAND test_h264_level)

# Stream keys, user names, passwords and query tokens never reach the log.
add_executable(test_url_redact test_url_redact.c)
target_link_libraries(test_url_redact PRIVATE pacs_core)
add_test(NAME url_redact COMMAND test_url_redact)

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

# Drive failure and return (slice C5): a drive missing at start that returns,
# and (macOS only) a RAM disk that fills up during the recording.
add_test(NAME smoke_drives_dev
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/smoke_drives.sh $<TARGET_FILE:pacscorder>)
set_tests_properties(smoke_drives_dev PROPERTIES TIMEOUT 180)

# The mirror comparison used by the tests above (tests/mp4_same.py): two muxes
# of the same frames, a second apart, must compare as the same recording, and a
# changed media byte must not. Skipped (exit 77) without gst-launch-1.0 or python3.
add_test(NAME mp4_same_dev
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mp4_same_check.sh)
set_tests_properties(mp4_same_dev PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77)
