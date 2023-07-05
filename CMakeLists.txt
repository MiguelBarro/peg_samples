cmake_minimum_required(VERSION 3.23)

project(idl-grammar VERSION 1.0.0
    DESCRIPTION [==[ Testing the idl-grammar for tao pegtl ]==]
    LANGUAGES CXX
    )

find_package(pegtl 4 REQUIRED CONFIG PATHS /temp/install/tao)

add_executable(express ${CMAKE_CURRENT_LIST_DIR}/src/express.cpp)
target_link_libraries(express PRIVATE taocpp::pegtl)

add_executable(literals ${CMAKE_CURRENT_LIST_DIR}/src/literals.cpp)
target_link_libraries(literals PRIVATE taocpp::pegtl)

enable_testing()
include(CTest)

# const expression testing
add_test(NAME expr.Hello COMMAND express "Hello" 1)
add_test(NAME expr.brackets COMMAND express "(Hello)" 1)
add_test(NAME expr.neg COMMAND express "~Hello" 1)
add_test(NAME expr.plus COMMAND express "Zipi + Zape" 2)
add_test(NAME expr.minus COMMAND express "Zipi - Zape" 2)
add_test(NAME expr.times COMMAND express "Zipi * Zape" 2)
add_test(NAME expr.div COMMAND express "Zipi / Zape" 2)
add_test(NAME expr.mod COMMAND express "Zipi % Zape" 2)
add_test(NAME expr.and COMMAND express "Zipi & Zape" 2)
add_test(NAME expr.or COMMAND express "Zipi | Zape" 2)
add_test(NAME expr.lshift COMMAND express "Zipi << Zape" 2)
add_test(NAME expr.rshift COMMAND express "Zipi >> Zape" 2)

add_test(NAME expr.comp0 COMMAND express "(Zipi + Zape) * Pantuflo" 3)
add_test(NAME expr.comp1 COMMAND express "Pantuflo * ( Zipi + Zape )" 3)
add_test(NAME expr.comp2 COMMAND express "~(Pantuflo * ( Zipi + Zape ))" 3)
add_test(NAME expr.comp3 COMMAND express "Jaimita & ~(Pantuflo * ( Zipi + Zape ))" 4)
add_test(NAME expr.comp4 COMMAND express "(Jaimita | miguelita) & ~(Pantuflo * ( Zipi + Zape ))" 5)

# literal testing
add_test(NAME literal.decimal.1 COMMAND literals "123456789" "decimal")
add_test(NAME literal.decimal.2 COMMAND literals "102340567089" "decimal")
add_test(NAME literal.octal.1 COMMAND literals "01234567" "octal")
add_test(NAME literal.octal.2 COMMAND literals "00120340567" "octal")
add_test(NAME literal.hexa.x COMMAND literals "0x123456789ABCDEF" "hexa")
add_test(NAME literal.hexa.X COMMAND literals "0X123456789ABCDEF" "hexa")
add_test(NAME literal.hexa.1 COMMAND literals "0x01234506789ABC0DEF" "hexa")

add_test(NAME literal.decimal.neg.1 COMMAND literals "+123456789" "decimal")
add_test(NAME literal.decimal.neg.2 COMMAND literals "123456789ABCDEF" "decimal")
add_test(NAME literal.octal.neg.1 COMMAND literals "0123456789" "octal")
add_test(NAME literal.octal.neg.2 COMMAND literals "0123456789ABCDEF" "octal")
add_test(NAME literal.hexa.neg.1 COMMAND literals "0x123456789ABCDEFGHI" "hexa")
add_test(NAME literal.hexa.neg.2 COMMAND literals "0x2345670x89ABCDEF" "hexa")
set_tests_properties(
        literal.decimal.neg.1
        literal.decimal.neg.2
        literal.octal.neg.1 
        literal.octal.neg.2 
        literal.hexa.neg.1 
        literal.hexa.neg.2 
        PROPERTIES WILL_FAIL TRUE)
