// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <iostream>
#include <cstdlib>

#include <tao/pegtl/contrib/analyze.hpp>

#include <grammar.hpp>

using namespace std;

template<typename Rule>
struct report_action
{
    template<typename Input>
    static void apply(const Input& in, int& /*id*/)
    {
            using namespace std;
            cout << "Rule: " << typeid(Rule).name() << " " << in.string() << endl;
    }
};

template<>
struct report_action<scoped_or_literal>
{
    template<typename Input>
    static void apply(const Input& in, int& id)
    {
            using namespace std;
            ++id;
            cout << "Rule: " << typeid(scoped_or_literal).name()
                 << " " << in.string() << endl;
    }
};

int main (int argc, char *argv[])
{
    using my_grammar = const_expr;

    std::size_t issues = tao::pegtl::analyze< my_grammar >(-1);
    if (issues > 0)
    {
        return tao::pegtl::analyze< my_grammar >(1);
    }

    // expected inputs:
    // • expression to parse
    // • expected identifiers to parse
    // test passes if the expression is parsed properly and the number of identifiers matches
    if ( argc <= 2 || argc > 3 )
        return -1;

    int res = 0;
    int identified = 0;
    int expected = atoi(argv[2]);

    pegtl::argv_input in( argv, 1);

    if( pegtl::parse<my_grammar, report_action>(in, identified) && in.empty())
    {
        cout << "parsing success!" << endl;

        // Check that literals are only parsed once
        return identified - expected;
    }
    else {
        cerr << "I don't understand." << endl;
        res = -1;
    }

    return res;
}
