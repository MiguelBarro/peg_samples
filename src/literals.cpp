// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>

#include <tao/pegtl/contrib/analyze.hpp>

#include <grammar.hpp>

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
repor_specialization(boolean_literal, bool)

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
