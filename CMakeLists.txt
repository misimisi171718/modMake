cmake_minimum_required(VERSION 3.4)

project(modMake)

add_subdirectory(lib/miniz)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/yaml-cpp)

file(GLOB_RECURSE modMakeSrc src/*.cpp)
add_executable(modMake ${modMakeSrc})
set_target_properties(modMake PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(modMake yaml-cpp spdlog miniz)
target_include_directories(modMake PRIVATE lib/tclap/include)
target_compile_options(modMake PRIVATE -Wall -Wextra -Wpedantic)