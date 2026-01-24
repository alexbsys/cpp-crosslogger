
cmake_minimum_required(VERSION 3.10)
project(cpp-crosslogger)

option(CPPCROSSLOGGER_ENABLE_TESTS "Build tests" ON)
option(CPPCROSSLOGGER_ENABLE_SAMPLES "Build examples" ON)
option(CPPCROSSLOGGER_ENABLE_SHARED "Shared library support" ON)

add_library(logger_interface INTERFACE)
target_include_directories(logger_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src/logger_host)

if (CPPCROSSLOGGER_ENABLE_SHARED)
  add_subdirectory(src/logger_dll)
endif()

if (CPPCROSSLOGGER_ENABLE_SAMPLES)
  add_subdirectory(examples)
endif()

if (CPPCROSSLOGGER_ENABLE_TESTS)
  add_subdirectory(tests)
endif()

