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

#define report_specialization(Rule, id) \
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

report_specialization(dec_literal, decimal)
report_specialization(oct_literal, octal)
report_specialization(hex_literal, hexa)
report_specialization(character_literal, char)
report_specialization(wide_character_literal, wchar)
report_specialization(escape_sequence, escape)
report_specialization(escaped_octal, escaped_octal)
report_specialization(escaped_hexa, escaped_hexa)
report_specialization(escaped_unicode, escaped_unicode)
report_specialization(string_literal, string)
report_specialization(wide_string_literal, wstring)
report_specialization(float_literal, float)
report_specialization(fixed_pt_literal, fixed)
report_specialization(boolean_literal, bool)

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
