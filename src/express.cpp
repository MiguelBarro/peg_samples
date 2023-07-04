// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <iostream>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>

namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

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

struct scoped_or_literal : pad<star<alnum>, ws> {};
struct const_expr; // forward declaration
struct primary_expr : sor<seq<open_parentheses, const_expr, close_parentheses>, scoped_or_literal> {};
struct unary_expr : sor<seq<unary_op, primary_expr>, primary_expr> {};
struct mult_expr : sor<seq<unary_expr, any_mult_op, mult_expr>, unary_expr> {};
struct add_expr : sor<seq<mult_expr, any_add_op, add_expr>, mult_expr> {};
struct shift_expr : sor<seq<add_expr, any_shift_op, shift_expr>, add_expr> {};
struct and_expr : sor<seq<shift_expr, and_op, and_expr>, shift_expr> {};
struct xor_expr : sor<seq<and_expr, xor_op, xor_expr>, and_expr> {};
struct const_expr : sor<seq<xor_expr, or_op, const_expr>, xor_expr> {};

using namespace std;

template<typename Rule>
struct report_action
{
    template<typename Input>
    static void apply(const Input& in)
    {
            using namespace std;
            cout << "Rule: " << typeid(Rule).name() << " " << in.string() << endl;
    }
};

int main (int argc, char *argv[])
{
    // Let's try to build it
    if ( argc <= 1 )
        return -1;

    pegtl::argv_input in( argv, 1 );
    if( pegtl::parse<const_expr, report_action>(in))
    {
        cout << "success!" << endl;
    }
    else {
        cerr << "I don't understand." << endl;
    }

    return 0;
}
