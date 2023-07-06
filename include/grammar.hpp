// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags
#pragma once

#include <tao/pegtl.hpp>

namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

/* literal grammar */

// boolean literal
struct boolean_literal : sor<TAO_PEGTL_STRING("TRUE"), TAO_PEGTL_STRING("FALSE")> {};

// integer literals
struct dec_literal : seq<opt<one<'-'>>, plus<digit>> {};
struct oct_literal : seq<one<'0'>, plus<odigit>> {};
struct hex_literal : seq<one<'0'>, one<'x','X'>, plus<xdigit>> {};

struct float_literal; // float literals start with and integer one, disambiguation is required
struct fixed_pt_literal; // fixed-point literals start with and integer one, disambiguation is required
struct integer_literal : seq<not_at<float_literal>,
                             not_at<fixed_pt_literal>,
                             sor<oct_literal,
                                 hex_literal,
                                 dec_literal>> {};

// float literals
using zero = one<'0'>;
using dot = one<'.'>;
using kw_exp = one<'e', 'E'>;
struct decimal_exponent : seq<kw_exp, opt<one<'-'>>, plus<digit>> {};
struct float_literal : seq< not_at<fixed_pt_literal>,
                            opt<one<'-'>>,
                            not_at<seq<dot, kw_exp>>,
                            star<digit>,
                            opt<seq<dot, star<digit>>>,
                            decimal_exponent> {};

// fixed-point literals
using kw_fixed = one<'d', 'D'>;
struct fixed_pt_literal : seq< opt<one<'-'>>,
                               not_at<seq<dot, kw_fixed>>,
                               star<digit>,
                               opt< seq<dot, star<digit>>>,
                               kw_fixed> {};

// char literals
using singlequote = one<'\''>;
using doublequote = one<'"'>;
using backslash = one<'\\'>;
using escapable_char = sor<
        singlequote,
        one<'"'>,
        one<'?'>,
        backslash,
        one<'a'>,
        one<'b'>,
        one<'f'>,
        one<'n'>,
        one<'r'>,
        one<'t'>,
        one<'v'>>;
struct escaped_octal : seq<backslash, rep_max<3,odigit>> {};
struct escaped_hexa : seq<backslash, one<'x'>, rep_max<2,xdigit>> {};
struct escaped_unicode : seq<backslash, one<'u'>, rep_max<4,xdigit>> {};
struct escape_sequence : sor<
        seq<backslash, escapable_char>,
        escaped_unicode,
        escaped_hexa,
        escaped_octal> {};
struct character : sor<escape_sequence, seq<not_at<singlequote>, any>> {};
struct character_literal : seq<singlequote, character, singlequote> {};

struct wide_character : sor<escape_sequence, seq<not_at<singlequote>, utf8::any>> {};
struct wide_character_literal : seq<one<'L'>, singlequote, wide_character, singlequote> {};

// string literals
struct string_character : sor<escape_sequence, seq<not_at<doublequote>, any>> {};
struct substring_literal : seq<doublequote, star<string_character>, doublequote> {};
struct string_literal : seq<substring_literal, star<seq<space, substring_literal>>> {};

// wstring literals
struct wstring_character : sor<escape_sequence, seq<not_at<doublequote>, utf8::any>> {};
struct wide_substring_literal : seq<one<'L'>, doublequote, star<wstring_character>, doublequote> {};
struct wide_string_literal : seq<wide_substring_literal, star<seq<space, wide_substring_literal>>> {};

struct literal : sor< boolean_literal,
                      integer_literal,
                      float_literal,
                      fixed_pt_literal,
                      character_literal,
                      wide_character_literal,
                      string_literal,
                      wide_string_literal> {};

// const expression grammar
struct ws : plus<space> {};
struct open_parentheses : pad<one<'('>, ws> {};
struct close_parentheses : pad<one<')'>, ws> {};

struct equal_op : pad<one<'='>, ws> {};
struct or_op : pad<one<'|'>, ws> {};
struct xor_op : pad<one<'^'>, ws> {};
struct and_op : pad<one<'&'>, ws> {};
struct lshift_op : pad<TAO_PEGTL_KEYWORD("<<"), ws> {};
struct rshift_op : pad<TAO_PEGTL_KEYWORD(">>"), ws> {};
struct add_op : pad<one<'+'>, ws> {};
struct sub_op : pad<one<'-'>, ws> {};
struct mult_op : pad<one<'*'>, ws> {};
struct div_op : pad<one<'/'>, ws> {};
struct mod_op : pad<one<'%'>, ws> {};
struct neg_op : pad<one<'~'>, ws> {};
struct unary_op : sor<sub_op, add_op, neg_op> {};

struct any_shift_op : sor<lshift_op, rshift_op> {};
struct any_add_op : sor<add_op, sub_op> {};
struct any_mult_op : sor<mult_op, div_op, mod_op> {};

struct scoped_or_literal : sor<literal, plus<alnum>> {}; // for now
struct const_expr; // forward declaration
struct primary_expr : sor<seq<open_parentheses, const_expr, close_parentheses>, scoped_or_literal> {};
struct unary_expr : sor<seq<unary_op, primary_expr>, primary_expr> {};
struct mult_expr : seq<unary_expr, opt<any_mult_op, mult_expr>> {};
struct add_expr : seq<mult_expr, opt<any_add_op, add_expr>> {};
struct shift_expr : seq<add_expr, opt<any_shift_op, shift_expr>> {};
struct and_expr : seq<shift_expr, opt<and_op, and_expr>> {};
struct xor_expr : seq<and_expr, opt<xor_op, xor_expr>> {};
struct const_expr : seq<xor_expr, opt<or_op, const_expr>> {};
