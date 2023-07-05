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

add_test(NAME Hello COMMAND express "Hello" 1)
add_test(NAME brackets COMMAND express "(Hello)" 1)
add_test(NAME neg COMMAND express "~Hello" 1)
add_test(NAME plus COMMAND express "Zipi + Zape" 2)
add_test(NAME minus COMMAND express "Zipi - Zape" 2)
add_test(NAME times COMMAND express "Zipi * Zape" 2)
add_test(NAME div COMMAND express "Zipi / Zape" 2)
add_test(NAME mod COMMAND express "Zipi % Zape" 2)
add_test(NAME and COMMAND express "Zipi & Zape" 2)
add_test(NAME or COMMAND express "Zipi | Zape" 2)
add_test(NAME lshift COMMAND express "Zipi << Zape" 2)
add_test(NAME rshift COMMAND express "Zipi >> Zape" 2)

add_test(NAME comp0 COMMAND express "(Zipi + Zape) * Pantuflo" 3)
add_test(NAME comp1 COMMAND express "Pantuflo * ( Zipi + Zape )" 3)
add_test(NAME comp2 COMMAND express "~(Pantuflo * ( Zipi + Zape ))" 3)
add_test(NAME comp3 COMMAND express "Jaimita & ~(Pantuflo * ( Zipi + Zape ))" 4)
add_test(NAME comp4 COMMAND express "(Jaimita | miguelita) & ~(Pantuflo * ( Zipi + Zape ))" 5)
