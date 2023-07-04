cmake_minimum_required(VERSION 3.23)

project(idl-grammar VERSION 1.0.0
    DESCRIPTION [==[ Testing the idl-grammar for tao pegtl ]==]
    LANGUAGES CXX
    )

find_package(pegtl 4 REQUIRED CONFIG PATHS /temp/install/tao)

add_executable(express ${CMAKE_CURRENT_LIST_DIR}/src/express.cpp)
target_link_libraries(express PRIVATE taocpp::pegtl)

enable_testing()
include(CTest)

add_test(NAME Hello
         COMMAND express "Hello")
