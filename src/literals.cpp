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
// integer literals
struct dec_literal : seq<opt<one<'-'>>, plus<digit>> {};
struct oct_literal : seq<one<'0'>, plus<odigit>> {};
struct hex_literal : seq<one<'0'>, one<'x','X'>, plus<xdigit>> {};

struct integer_literal : sor< oct_literal,
                              hex_literal,
                              dec_literal> {};
// char literals
using singlequote = one<'\''>;
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
struct escape_sequence : seq<backslash, escapable_char> {};
struct character_literal : seq<singlequote, sor<escape_sequence, any>, singlequote> {};

struct literal : sor< integer_literal,
                      character_literal> {};

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

template<>
struct report_action<character_literal>
{
    template<typename Input>
    static void apply(const Input& in, mystate& s)
    {
            using namespace std;
            ++s["char"];
            cout << "Rule: " << typeid(character_literal).name()
                 << " " << in.string() << endl;
    }
};

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
