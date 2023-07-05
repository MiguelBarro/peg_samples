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
## integer literals
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

## character literals
add_test(NAME literal.char.1 COMMAND literals "'a'" "char") # a
add_test(NAME literal.char.2 COMMAND literals "'A'" "char") # A
add_test(NAME literal.char.3 COMMAND literals "'0'" "char") # 0
add_test(NAME literal.char.4 COMMAND literals "'<'" "char") # <
add_test(NAME literal.char.5 COMMAND literals "' '" "char") # ' '

add_test(NAME literal.escape.1 COMMAND literals [==['\'']==] "escape") # \'
add_test(NAME literal.escape.2 COMMAND literals [==['\"']==] "escape") # \"
add_test(NAME literal.escape.3 COMMAND literals [==['\?']==] "escape") # ?
add_test(NAME literal.escape.4 COMMAND literals [==['\\']==] "escape") # \
add_test(NAME literal.escape.5 COMMAND literals [==['\'']==] "escape") # \'
add_test(NAME literal.escape.6 COMMAND literals [==['\b']==] "escape") # \b
add_test(NAME literal.escape.7 COMMAND literals [==['\a']==] "escape") # \a
add_test(NAME literal.escape.8 COMMAND literals [==['\f']==] "escape") # \f
add_test(NAME literal.escape.9 COMMAND literals [==['\r']==] "escape") # \r
add_test(NAME literal.escape.10 COMMAND literals [==['\b']==] "escape") # \b
add_test(NAME literal.escape.11 COMMAND literals [==['\v']==] "escape") # \v
add_test(NAME literal.escape.12 COMMAND literals [==['\t']==] "escape") # \t
add_test(NAME literal.escape.13 COMMAND literals [==['\n']==] "escape") # \n

add_test(NAME literal.escape.octal.1 COMMAND literals [==['\324']==] "escaped_octal") # octal
add_test(NAME literal.escape.octal.2 COMMAND literals [==['\32']==] "escaped_octal") # octal
add_test(NAME literal.escape.octal.3 COMMAND literals [==['\3']==] "escaped_octal") # octal

add_test(NAME literal.escape.hexa.1 COMMAND literals [==['\xA2']==] "escaped_hexa") # hexa
add_test(NAME literal.escape.hexa.2 COMMAND literals [==['\xF']==] "escaped_hexa") # hexa

add_test(NAME literal.escape.unicode.1 COMMAND literals [==['\u3240']==] "escaped_unicode") # unicode
add_test(NAME literal.escape.unicode.2 COMMAND literals [==['\u32A']==] "escaped_unicode") # unicode
add_test(NAME literal.escape.unicode.3 COMMAND literals [==['\u3F']==] "escaped_unicode") # unicode
add_test(NAME literal.escape.unicode.4 COMMAND literals [==['\u3']==] "escaped_unicode") # unicode

add_test(NAME literal.char.neg COMMAND literals "'abc'" "char") 
add_test(NAME literal.escape.neg.1 COMMAND literals [==['\c']==] "escape") 
add_test(NAME literal.escape.neg.2 COMMAND literals [==['\ca']==] "escape") 
add_test(NAME literal.escape.octal.neg.1 COMMAND literals [==['\3241']==] "escaped_octal") 
add_test(NAME literal.escape.octal.neg.2 COMMAND literals [==['\A']==] "escaped_octal") 
add_test(NAME literal.escape.hexa.neg.1 COMMAND literals [==['\xA232']==] "escaped_hexa") 
add_test(NAME literal.escape.hexa.neg.2 COMMAND literals [==['\xG']==] "escaped_hexa") 
add_test(NAME literal.escape.unicode.neg.1 COMMAND literals [==['\uA232A']==] "escaped_unicode") 
add_test(NAME literal.escape.unicode.neg.2 COMMAND literals [==['\uG']==] "escaped_unicode") 
set_tests_properties(
        literal.char.neg
        literal.escape.neg.1
        literal.escape.neg.2
        literal.escape.octal.neg.1
        literal.escape.octal.neg.2
        literal.escape.hexa.neg.1
        literal.escape.hexa.neg.2
        literal.escape.unicode.neg.1
        literal.escape.unicode.neg.2
        PROPERTIES WILL_FAIL TRUE)

