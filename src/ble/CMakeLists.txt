target_include_directories(app PRIVATE .)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app_ble.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/services/mysensor.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/services/mycommand.c)