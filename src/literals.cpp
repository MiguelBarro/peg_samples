// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>

namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

// literal grammar
struct dec_literal : seq<opt<one<'-'>>, plus<digit>> {};

struct octal_digit : range<'0', '7'> {};
struct oct_literal : seq<one<'0'>, plus<octal_digit>> {};

struct hex_literal : seq<one<'0'>, one<'x','X'>, plus<xdigit>> {};

struct integer_literal : sor< oct_literal,
                              hex_literal,
                              dec_literal> {};

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

template<>
struct report_action<dec_literal>
{
    template<typename Input>
    static void apply(const Input& in, mystate& s)
    {
            using namespace std;
            ++s["decimal"];
            cout << "Rule: " << typeid(dec_literal).name()
                 << " " << in.string() << endl;
    }
};

template<>
struct report_action<oct_literal>
{
    template<typename Input>
    static void apply(const Input& in, mystate& s)
    {
            using namespace std;
            ++s["octal"];
            cout << "Rule: " << typeid(oct_literal).name()
                 << " " << in.string() << endl;
    }
};

template<>
struct report_action<hex_literal>
{
    template<typename Input>
    static void apply(const Input& in, mystate& s)
    {
            using namespace std;
            ++s["hexa"];
            cout << "Rule: " << typeid(hex_literal).name()
                 << " " << in.string() << endl;
    }
};

int main (int argc, char *argv[])
{
    using my_grammar = integer_literal;

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
