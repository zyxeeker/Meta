cmake_minimum_required(VERSION 3.10)
project(Meta)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
# YAML
include_directories(${CMAKE_SOURCE_DIR}/3rd/yaml/include)

# YAML
link_libraries(${CMAKE_SOURCE_DIR}/3rd/yaml/lib/libyaml-cpp.a)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(3rd/googletest)