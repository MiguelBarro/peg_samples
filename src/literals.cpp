// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>

namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

/* literal grammar */

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

struct literal : sor< integer_literal,
                      float_literal,
                      fixed_pt_literal,
                      character_literal,
                      wide_character_literal,
                      string_literal,
                      wide_string_literal> {};

using namespace std;

using mystate = std::map<std::string, int>;

template<typename Rule>
struct report_action
{
    template<typename Input>
    static void apply(const Input& in, mystate& /*s*/)
    {
            using namespace std;
            cout << "Rule: " << typeid(Rule).name() << " " << in.string() << endl;
    }
};

#define repor_specialization(Rule, id) \
template<> \
struct report_action<Rule> \
{ \
    template<typename Input> \
    static void apply(const Input& in, mystate& s) \
    { \
            using namespace std; \
            ++s[#id]; \
            cout << "Rule: " << typeid(Rule).name() \
                 << " " << in.string() << endl; \
    } \
};

repor_specialization(dec_literal, decimal)
repor_specialization(oct_literal, octal)
repor_specialization(hex_literal, hexa)
repor_specialization(character_literal, char)
repor_specialization(wide_character_literal, wchar)
repor_specialization(escape_sequence, escape)
repor_specialization(escaped_octal, escaped_octal)
repor_specialization(escaped_hexa, escaped_hexa)
repor_specialization(escaped_unicode, escaped_unicode)
repor_specialization(string_literal, string)
repor_specialization(wide_string_literal, wstring)
repor_specialization(float_literal, float)
repor_specialization(fixed_pt_literal, fixed)

int main (int argc, char *argv[])
{
    using my_grammar = literal;

    std::size_t issues = tao::pegtl::analyze< my_grammar >(-1);
    if (issues > 0)
    {
        return tao::pegtl::analyze< my_grammar >(1);
    }

    // expected inputs:
    // • expression to parse
    // • expected type: integer, float, ...
    // test passes if the expression is parsed properly and the number of identifiers matches
    if ( argc <= 2 || argc > 3 )
        return -1;

    int res = 0;
    mystate s;

    pegtl::argv_input in( argv, 1);

    if( pegtl::parse<my_grammar, report_action>(in, s) && in.empty())
    {
        cout << "parsing success!" << endl;

        // Check that literals are there and only parsed once
        auto it = s.find(argv[2]);
        return it != s.end() ? it->second - 1 : -1;
    }
    else {
        cerr << "I don't understand." << endl;
        res = -1;
    }

    return res;
}
