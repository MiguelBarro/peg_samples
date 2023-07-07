cmake_minimum_required(VERSION 3.23)

project(idl-grammar VERSION 1.0.0
    DESCRIPTION [==[ Testing the idl-grammar for tao pegtl ]==]
    LANGUAGES CXX
    )

find_package(pegtl 4 REQUIRED CONFIG PATHS /temp/install/tao)

add_library(grammar INTERFACE)
target_include_directories(grammar INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(express ${CMAKE_CURRENT_LIST_DIR}/src/express.cpp)
target_link_libraries(express PRIVATE taocpp::pegtl grammar)

add_executable(literals ${CMAKE_CURRENT_LIST_DIR}/src/literals.cpp)
target_link_libraries(literals PRIVATE taocpp::pegtl grammar)

add_executable(calculator ${CMAKE_CURRENT_LIST_DIR}/src/calc.cpp)
target_compile_features(calculator PRIVATE cxx_std_17)
target_link_libraries(calculator PRIVATE taocpp::pegtl grammar)

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
add_test(NAME literal.char.6 COMMAND literals [==['"']==] "char") # ' '

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

add_test(NAME literal.char.neg.1 COMMAND literals "'abc'" "char") 
add_test(NAME literal.char.neg.2 COMMAND literals "'''" "char") 
add_test(NAME literal.escape.neg.1 COMMAND literals [==['\c']==] "escape") 
add_test(NAME literal.escape.neg.2 COMMAND literals [==['\ca']==] "escape") 
add_test(NAME literal.escape.octal.neg.1 COMMAND literals [==['\3241']==] "escaped_octal") 
add_test(NAME literal.escape.octal.neg.2 COMMAND literals [==['\A']==] "escaped_octal") 
add_test(NAME literal.escape.hexa.neg.1 COMMAND literals [==['\xA232']==] "escaped_hexa") 
add_test(NAME literal.escape.hexa.neg.2 COMMAND literals [==['\xG']==] "escaped_hexa") 
add_test(NAME literal.escape.unicode.neg.1 COMMAND literals [==['\uA232A']==] "escaped_unicode") 
add_test(NAME literal.escape.unicode.neg.2 COMMAND literals [==['\uG']==] "escaped_unicode") 
set_tests_properties(
        literal.char.neg.1
        literal.char.neg.2
        literal.escape.neg.1
        literal.escape.neg.2
        literal.escape.octal.neg.1
        literal.escape.octal.neg.2
        literal.escape.hexa.neg.1
        literal.escape.hexa.neg.2
        literal.escape.unicode.neg.1
        literal.escape.unicode.neg.2
        PROPERTIES WILL_FAIL TRUE)

# no easy way to pass utf8 encoding as argument
add_test(NAME literal.wchar COMMAND literals [==[L'a']==] "wchar")
add_test(NAME literal.wchar.escape COMMAND literals [==[L'\'']==] "escape") # \'
add_test(NAME literal.wchar.escape.octal COMMAND literals [==[L'\324']==] "escaped_octal") # octal
add_test(NAME literal.wchar.escape.hexa COMMAND literals [==[L'\xA2']==] "escaped_hexa") # hexa
add_test(NAME literal.wchar.escape.unicode COMMAND literals [==[L'\u3240']==] "escaped_unicode") # unicode

## string literals
add_test(NAME literal.string.1 COMMAND literals [==["hello world"]==] "string")
add_test(NAME literal.string.2 COMMAND literals [==["hello" "world"]==] "string")
add_test(NAME literal.string.3 COMMAND literals [==["hello\nworld"]==] "string")
add_test(NAME literal.string.4 COMMAND literals [==["hello 'world'"]==] "string")
add_test(NAME literal.wstring.1 COMMAND literals [==[L"hello world"]==] "wstring")
add_test(NAME literal.wstring.2 COMMAND literals [==[L"hello" L"world"]==] "wstring")
add_test(NAME literal.wstring.3 COMMAND literals [==[L"hello\nworld"]==] "wstring")
add_test(NAME literal.wstring.4 COMMAND literals [==[L"hello 'world'"]==] "wstring")

add_test(NAME literal.string.neg.1 COMMAND literals [==["""]==] "string")
add_test(NAME literal.string.neg.2 COMMAND literals [==["hello" L"world"]==] "string")
add_test(NAME literal.wstring.neg.1 COMMAND literals [==[L"""]==] "string")
add_test(NAME literal.wstring.neg.2 COMMAND literals [==[L"hello" "world"]==] "string")
set_tests_properties(
        literal.string.neg.1
        literal.string.neg.2
        literal.wstring.neg.1
        literal.wstring.neg.2
        PROPERTIES WILL_FAIL TRUE)

## float literals
add_test(NAME literal.float.1 COMMAND literals "12.3456e13" "float")
add_test(NAME literal.float.2 COMMAND literals "12.e13" "float")
add_test(NAME literal.float.3 COMMAND literals "12e13" "float")
add_test(NAME literal.float.4 COMMAND literals ".1e13" "float")
add_test(NAME literal.float.5 COMMAND literals "-12.3456e13" "float")
add_test(NAME literal.float.6 COMMAND literals "-12.e-13" "float")
add_test(NAME literal.float.7 COMMAND literals "-12e-13" "float")
add_test(NAME literal.float.8 COMMAND literals "0.e13" "float")
add_test(NAME literal.float.9 COMMAND literals ".000e13" "float")
add_test(NAME literal.float.10 COMMAND literals "00.00e13" "float")

add_test(NAME literal.float.neg.1 COMMAND literals "12.3456e13" "decimal")
add_test(NAME literal.float.neg.2 COMMAND literals "+12.3456e13" "float")
add_test(NAME literal.float.neg.3 COMMAND literals "12.3456e+13" "float")
add_test(NAME literal.float.neg.4 COMMAND literals "12.a4A6e13" "float")
add_test(NAME literal.float.neg.5 COMMAND literals ".e13" "float")
set_tests_properties(
        literal.float.neg.1
        literal.float.neg.2
        literal.float.neg.3
        literal.float.neg.4
        literal.float.neg.5
        PROPERTIES WILL_FAIL TRUE)

## fixed point literals
add_test(NAME literal.fixed.1 COMMAND literals "1234567890123456D" "fixed")
add_test(NAME literal.fixed.2 COMMAND literals "-123456789.123456D" "fixed")
add_test(NAME literal.fixed.3 COMMAND literals "12345678.d" "fixed")
add_test(NAME literal.fixed.4 COMMAND literals "-.1234567890d" "fixed")
add_test(NAME literal.fixed.5 COMMAND literals "12345.99d" "fixed")
add_test(NAME literal.fixed.6 COMMAND literals "-000123.990d" "fixed")
add_test(NAME literal.fixed.7 COMMAND literals "0.00D" "fixed")
add_test(NAME literal.fixed.8 COMMAND literals "-0.00D" "fixed")

add_test(NAME literal.fixed.neg.1 COMMAND literals "1234567890123456D" "decimal")
add_test(NAME literal.fixed.neg.2 COMMAND literals "+1234567890123456D" "fixed")
add_test(NAME literal.fixed.neg.3 COMMAND literals "123456A8F0C23456D" "fixed")
add_test(NAME literal.fixed.neg.4 COMMAND literals "0123456789D" "octal")
add_test(NAME literal.fixed.neg.5 COMMAND literals ".D" "fixed")
set_tests_properties(
        literal.fixed.neg.1
        literal.fixed.neg.2
        literal.fixed.neg.3
        literal.fixed.neg.4
        literal.fixed.neg.5
        PROPERTIES WILL_FAIL TRUE)

## boolean literals
add_test(NAME literal.bool.1 COMMAND literals "TRUE" "bool")
add_test(NAME literal.bool.2 COMMAND literals "FALSE" "bool")
add_test(NAME literal.bool.neg.1 COMMAND literals "true" "bool")
add_test(NAME literal.bool.neg.2 COMMAND literals "false" "bool")
add_test(NAME literal.bool.neg.3 COMMAND literals "True" "bool")
add_test(NAME literal.bool.neg.4 COMMAND literals "False" "bool")
set_tests_properties(
        literal.bool.neg.1
        literal.bool.neg.2
        literal.bool.neg.3
        literal.bool.neg.4
        PROPERTIES WILL_FAIL TRUE)

#expression evaluation
add_test(NAME calc.dec COMMAND calculator "1" "decimal" 1)
add_test(NAME calc.bool COMMAND calculator "TRUE" "bool" 1)
add_test(NAME calc.octal COMMAND calculator "0343" "octal" 227)
add_test(NAME calc.hexa COMMAND calculator "0xff" "hexa" 255)
add_test(NAME calc.float COMMAND calculator "1.41421e2" "float" 141.421)
add_test(NAME calc.fixed COMMAND calculator "1.41421d" "fixed" 1.41421)
add_test(NAME calc.brackets COMMAND calculator "(1)" "decimal" 1)
add_test(NAME calc.inf COMMAND calculator "~1" "decimal;inv" -2)
add_test(NAME calc.neg COMMAND calculator "-1" "decimal;minus" -1)
add_test(NAME calc.plus COMMAND calculator "01 + 1" "octal;decimal;add" 2)
add_test(NAME calc.minus COMMAND calculator "0x3 - 1" "hexa;decimal;sub" 2)
add_test(NAME calc.times COMMAND calculator "1 * 0x2" "decimal;hexa;mult" 2)
add_test(NAME calc.div COMMAND calculator "4e0 / 2d" "float;fixed;div" 2)
add_test(NAME calc.mod COMMAND calculator "2 % 3" "decimal;decimal;mod" 2)
add_test(NAME calc.and COMMAND calculator "0X3 & 2" "hexa;decimal;and" 2)
add_test(NAME calc.or COMMAND calculator "1 | 1" "decimal;decimal;or" 1)
add_test(NAME calc.lshift COMMAND calculator "1 << 1" "decimal;decimal;<<" 2)
add_test(NAME calc.rshift COMMAND calculator "0x4 >> 1" "hexa;decimal;>>" 2)

add_test(NAME calc.comp0 COMMAND calculator "(02 + 0x1) * 2" "octal;hexa;add;decimal;mult" 6)
add_test(NAME calc.comp1 COMMAND calculator "4 / ( 07 & 0x2 )" "decimal;octal;hexa;and;div" 2)
add_test(NAME calc.comp2 COMMAND calculator "-(5 * ( 03 + 0xF ))" "decimal;octal;hexa;add;mult;minus" -90)
add_test(NAME calc.comp3 COMMAND calculator "0xf0 & ~(4 * ( 0x7 - 02 ))" "hexa;decimal;hexa;octal;sub;mult;inv;and" 224)
add_test(NAME calc.comp4 COMMAND calculator "(0x7 | 0x9) & ~(6 * ( 024 - 5 ))" "hexa;hexa;or;decimal;octal;decimal;sub;mult;inv;and" 5)
add_test(NAME calc.comp5 COMMAND calculator "(2 + 4) / 3.0d" "decimal;decimal;add;fixed;div" 2.0)
add_test(NAME calc.comp6 COMMAND calculator "(2e0 + 4) / 3.0d" "float;decimal;add;fixed;div" 2.0)
