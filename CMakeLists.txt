cmake_minimum_required(VERSION 3.12)

project(IndividualHomeWork)

add_library(default errors.cpp)

if(MSVC)
  target_compile_options(default PUBLIC /W4 /WX)
else()
  target_compile_options(default PUBLIC -Wall -Wextra -Wpedantic -Werror)
endif()
target_compile_features(default PUBLIC cxx_std_20)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC default)