cmake_minimum_required (VERSION 2.6)
project (TokenCore)
FILE(GLOB_RECURSE LibFiles "${TokenFighter_SOURCE_DIR}/TokenCore/include/*.hpp")
add_custom_target(headers SOURCES ${LibFiles})
include_directories("${TokenFighter_SOURCE_DIR}/TokenCore/include")
add_executable (TokenCore TokenCore.cpp)
