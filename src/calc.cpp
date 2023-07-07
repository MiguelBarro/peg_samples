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

#define load_action(Rule, id, stack) \
template<> \
struct report_action<Rule> \
{ \
    template<typename Input> \
    static void apply(const Input& in, expr_reg& m, calc_stack& s) \
    { \
        m += (m.empty() ? "" : ";") + std::string{#id}; \
        cout << "Rule: " << typeid(Rule).name() \
             << " " << in.string() << endl; \
        istringstream ss(in.string()); \
        stack ; \
    } \
};

#define op_action(Rule, id, operation) \
template<> \
struct report_action<Rule> \
{ \
    template<typename Input> \
    static void apply(const Input& in, expr_reg& m, calc_stack& s) \
    { \
        m += (m.empty() ? "" : ";") + std::string{#id}; \
        cout << "Rule: " << typeid(Rule).name() \
             << " " << in.string() << endl; \
        operation ; \
    } \
};

template<>
struct report_action<boolean_literal>
{
    template<typename Input>
    static void apply(const Input& in, expr_reg& m, calc_stack& s)
    {
        cout << "Rule: " << typeid(boolean_literal).name()
             << " " << in.string() << endl;

        m += (m.empty() ? "" : ";") + std::string{"bool"};

        struct custom_tf : std::numpunct<char> {
            std::string do_truename()  const { return "TRUE"; }
            std::string do_falsename() const { return "FALSE"; }
        };

        istringstream ss(in.string());
        ss.imbue(locale(ss.getloc(), new custom_tf));

        bool res;
        ss >> boolalpha >> res;
        s.emplace_back(res);
    }
};

load_action(dec_literal, decimal, long long res; ss >> res; s.emplace_back(res))
load_action(oct_literal, octal, long long res; ss >> setbase(ios_base::oct) >> res; s.emplace_back(res))
load_action(hex_literal, hexa, long long res; ss >> setbase(ios_base::hex) >> res; s.emplace_back(res))
load_action(float_literal, float, long double res; ss >> res; s.emplace_back(res))
load_action(fixed_pt_literal, fixed, long double res; ss >> res; s.emplace_back(res); cout << res << endl)

op_action(or_exec, or,)
op_action(xor_exec, xor,)
op_action(and_exec, and,)
op_action(rshift_exec, >>,)
op_action(lshift_exec, <<,)
op_action(add_exec, add,)
op_action(sub_exec, sub,)
op_action(mult_exec, mult,)
op_action(div_exec, div,)
op_action(mod_exec, mod,)
op_action(minus_exec, minus,)
op_action(plus_exec, plus,)
op_action(inv_exec, inv,)

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

        // compare with expected evaluation order
        bool res = m == argv[2];

        // compare evaluation result
        if (s.front().type() == typeid(bool))
        {
            res = any_cast<bool>(s.front()) == (atoi(argv[3]) != 0);
        }
        else if (s.front().type() == typeid(long long))
        {
            res = any_cast<long long>(s.front()) == atoll(argv[3]);
        }
        else if (s.front().type() == typeid(long double))
        {
            res = any_cast<long double>(s.front()) == atof(argv[3]);
        }

        ret = res ? 0 : -1;
    }
    else {
        cerr << "I don't understand." << endl;
        ret = -1;
    }

    return ret;
}
