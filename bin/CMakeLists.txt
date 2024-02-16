add_executable(Bruh_Archiver main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Archiver InputParser HemmingCode)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
