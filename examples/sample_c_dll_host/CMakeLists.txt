
cmake_minimum_required(VERSION 3.10)

project (sample_c_dll_host LANGUAGES C CXX)

file(GLOB SOURCE_FILES *.c *.cc *.h)

add_executable(sample_c_dll_host ${SOURCE_FILES})

target_link_libraries(sample_c_dll_host PUBLIC 
  logger_host
)

target_compile_definitions(sample_c_dll_host PRIVATE LOG_USE_DLL=0 LOG_THIS_IS_DLL=0)
