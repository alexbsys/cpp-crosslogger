
cmake_minimum_required(VERSION 3.10)

project(logger_tests VERSION 0.0.1.1 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "v")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #Optional

set (SOURCE_FILES
  utils/cprintf.c
  utils/mem_checker.cc
  logger_test_log.cc
  test_common.cc
)

set (HEADER_FILES
  logger_tests_log.h
  utils/cprintf.h
  utils/mem_checker.h
)  
 
include_directories("${CMAKE_CURRENT_LIST_DIR}/../include")

add_executable(logger_tests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(logger_tests gtest_main logger_host)

install(TARGETS logger_tests DESTINATION ../out)
