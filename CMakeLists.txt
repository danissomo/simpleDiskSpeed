cmake_minimum_required(VERSION 3.10)
project(benchmark)
set(BUILD_SHARED_LIBS true)
if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()

add_library(app_lib SHARED src/app.cpp src/benchmark.cpp src/semaphore.cpp)
target_include_directories(app_lib PUBLIC include)
add_executable(main_target src/main.cpp )

target_link_libraries(main_target PUBLIC app_lib user32)
target_link_options(main_target PRIVATE "/SUBSYSTEM:WINDOWS")