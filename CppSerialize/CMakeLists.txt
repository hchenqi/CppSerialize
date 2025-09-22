add_library(CppSerialize INTERFACE)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(CppSerialize INTERFACE ${PARENT_DIR})
target_compile_features(CppSerialize INTERFACE cxx_std_20)
