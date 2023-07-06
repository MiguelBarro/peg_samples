// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <any>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

#include <tao/pegtl/contrib/analyze.hpp>

#include <grammar.hpp>

using namespace std;

using calc_stack = std::list<std::any>;
using expr_reg = std::string;

template<typename Rule>
struct report_action
{
    template<typename Input>
    static void apply(const Input& in, expr_reg&, calc_stack&)
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
    static void apply(const Input& in, expr_reg& m, calc_stack&) \
    { \
        m += (m.empty() ? "" : ";") + std::string{#id}; \
        cout << "Rule: " << typeid(Rule).name() \
             << " " << in.string() << endl; \
    } \
};

report_specialization(boolean_literal, bool)
report_specialization(dec_literal, decimal)
report_specialization(oct_literal, octal)
report_specialization(hex_literal, hexa)
report_specialization(float_literal, float)
report_specialization(fixed_pt_literal, fixed)

report_specialization(or_exec, or)
report_specialization(xor_exec, xor)
report_specialization(and_exec, and)
report_specialization(rshift_exec, >>)
report_specialization(lshift_exec, <<)
report_specialization(add_exec, add)
report_specialization(sub_exec, sub)
report_specialization(mult_exec, mult)
report_specialization(div_exec, div)
report_specialization(mod_exec, mod)
report_specialization(minus_exec, minus)
report_specialization(plus_exec, plus)
report_specialization(inv_exec, inv)

int main (int argc, char *argv[])
{
    using my_grammar = const_expr;

    std::size_t issues = tao::pegtl::analyze< my_grammar >(-1);
    if (issues > 0)
    {
        return tao::pegtl::analyze< my_grammar >(1);
    }

    // expected inputs:
    // • expression to calculate
    // • expressions to evaluate
    // • expected result (interpreted as expression result)
    // test passes if the expression is parsed properly and the number of identifiers matches
    if ( argc == 3 )
        return -1;

    int ret = 0;
    calc_stack s;
    expr_reg m;

    pegtl::argv_input in( argv, 1);

    if( pegtl::parse<my_grammar, report_action>(in, m, s) && in.empty())
    {
        cout << "parsing success!" << endl;
        cout << "evaluated expressions: " << m << endl;
        cout << "expected expressions: " << argv[2] << endl;

        // compare with expected
        ret = m == argv[2] ? 0 : -1;
    }
    else {
        cerr << "I don't understand." << endl;
        ret = -1;
    }

    return ret;
}
