cmake_minimum_required(VERSION 3.15)
project(TestCase4 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc io i18n)

add_executable(TestCase4 test.cpp)

target_link_libraries(TestCase4 PRIVATE nlohmann_json::nlohmann_json ${ICU_LIBRARIES})
target_include_directories(TestCase4 PRIVATE ${ICU_INCLUDE_DIRS})