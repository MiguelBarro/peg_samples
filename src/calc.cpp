// vim: tags+=~/Documents/DHI/PEGTL/taopeg.tags

#include <any>
#include <array>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <typeindex>

#include <tao/pegtl/contrib/analyze.hpp>

#include <grammar.hpp>

using namespace std;

using calc_stack = std::list<std::any>;
using expr_reg = std::string;

// std::any support basically:
// + all integers are managed as long long
// + all floats (fixed included) are managed as long double (in MSVC will be an actual double)

template<typename T> T promote(const std::any& x)
{
    if ( typeid(T) == x.type())
    {
        return any_cast<T>(x);
    }

    if ( typeid(long long) == x.type() )
    {
        return static_cast<T>(any_cast<long long>(x));
    }
    else if ( typeid(long double) == x.type() )
    {
        return static_cast<T>(any_cast<long double>(x));
    }
    else if ( typeid(bool) == x.type() )
    {
        return static_cast<T>(any_cast<bool>(x));
    }

    throw runtime_error("bad promote");
}

const std::type_info& promotion_type(const std::any& a, const std::any& b)
{
    static std::map<std::type_index, int> priorities = {
        {typeid(long double), 2},
        {typeid(long long), 1},
        {typeid(bool), 0},
    };

    static std::array<const type_info*,3> infos = {
        &typeid(bool),
        &typeid(long long),
        &typeid(long double)
    };

    if (a.type() == b.type())
    {
        return a.type();
    }
    else
    {
       return *infos[std::max(priorities.at(a.type()), priorities.at(b.type()))];
    }
}

// Actions
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
        s.emplace_front(res);
    }
};

load_action(dec_literal, decimal,
    long long res;
    ss >> res;
    s.emplace_front(res))

load_action(oct_literal, octal,
    long long res;
    ss >> setbase(ios_base::oct) >> res;
    s.emplace_front(res))

load_action(hex_literal, hexa,
    long long res;
    ss >> setbase(ios_base::hex) >> res;
    s.emplace_front(res))

load_action(float_literal, float,
    long double res;
    ss >> res;
    s.emplace_front(res))

load_action(fixed_pt_literal, fixed, long double res;
    ss >> res;
    s.emplace_front(res);
    cout << res << endl)

#define float_op_action(Rule, id, operation) \
template<> \
struct report_action<Rule> \
{ \
    template<typename Input> \
    static void apply(const Input& in, expr_reg& m, calc_stack& s) \
    { \
        cout << "Rule: " << typeid(Rule).name() \
             << " " << in.string() << endl; \
 \
        m += (m.empty() ? "" : ";") + std::string{#id}; \
 \
        /* calculate the result */ \
        auto it = s.begin(); \
        std::any s1 = *it++, s2 = *it, res; \
 \
        const auto& pt = promotion_type(s1, s2); \
 \
        if ( typeid(long long) == pt ) \
        { \
            res = promote<long long>(s2) operation promote<long long>(s1); \
        } \
        else if ( typeid(long double) == pt ) \
        { \
            res = promote<long double>(s2) operation promote<long double>(s1); \
        } \
        else \
        { \
            throw runtime_error("invalid arguments for the operation " #operation ); \
        } \
 \
        /* update the stack */ \
        s.pop_front(); \
        s.front() = std::move(res); \
 \
    } \
};

#define int_op_action(Rule, id, operation) \
template<> \
struct report_action<Rule> \
{ \
    template<typename Input> \
    static void apply(const Input& in, expr_reg& m, calc_stack& s) \
    { \
        cout << "Rule: " << typeid(Rule).name() \
             << " " << in.string() << endl; \
 \
        m += (m.empty() ? "" : ";") + std::string{#id}; \
 \
        /* calculate the result */ \
        auto it = s.begin(); \
        std::any s1 = *it++, s2 = *it, res; \
 \
        const auto& pt = promotion_type(s1, s2); \
 \
        if ( typeid(long long) == pt ) \
        { \
            res = promote<long long>(s2) operation promote<long long>(s1); \
        } \
        else \
        { \
            throw runtime_error("invalid arguments for the operation " #operation ); \
        } \
 \
        /* update the stack */ \
        s.pop_front(); \
        s.front() = std::move(res); \
 \
    } \
};

#define bool_op_action(Rule, id, operation) \
template<> \
struct report_action<Rule> \
{ \
    template<typename Input> \
    static void apply(const Input& in, expr_reg& m, calc_stack& s) \
    { \
        cout << "Rule: " << typeid(Rule).name() \
             << " " << in.string() << endl; \
 \
        m += (m.empty() ? "" : ";") + std::string{#id}; \
 \
        /* calculate the result */ \
        auto it = s.begin(); \
        std::any s1 = *it++, s2 = *it, res; \
 \
        const auto& pt = promotion_type(s1, s2); \
 \
        if ( typeid(long long) == pt ) \
        { \
            res = promote<long long>(s2) operation promote<long long>(s1); \
        } \
        else if ( typeid(bool) == pt ) \
        { \
            res = promote<bool>(s2) operation promote<bool>(s1); \
        } \
        else \
        { \
            throw runtime_error("invalid arguments for the operation " #operation ); \
        } \
 \
        /* update the stack */ \
        s.pop_front(); \
        s.front() = std::move(res); \
 \
    } \
};

bool_op_action(or_exec, or, |)
bool_op_action(xor_exec, xor, ^)
bool_op_action(and_exec, and, &)
int_op_action(rshift_exec, >>, >>)
int_op_action(lshift_exec, <<, <<)
int_op_action(mod_exec, mod, %)
float_op_action(add_exec, add, +)
float_op_action(sub_exec, sub, -)
float_op_action(mult_exec, mult, *)
float_op_action(div_exec, div, /)

template<>
struct report_action<minus_exec>
{
    template<typename Input>
    static void apply(const Input& in, expr_reg& m, calc_stack& s)
    {
        using namespace std;
        cout << "Rule: " << typeid(minus_exec).name() << " " << in.string() << endl;

        m += (m.empty() ? "" : ";") + std::string{"minus"};

        if ( typeid(long long) == s.front().type() )
        {
            s.front() = -promote<long long>(s.front());
        }
        else if ( typeid(long double) == s.front().type() )
        {
            s.front() = -promote<long double>(s.front());
        }
        else
        {
            throw runtime_error("invalid argument for the minus unary operator");
        }
    }
};

template<>
struct report_action<plus_exec>
{
    template<typename Input>
    static void apply(const Input& in, expr_reg& m, calc_stack& s)
    {
        using namespace std;
        cout << "Rule: " << typeid(plus_exec).name() << " " << in.string() << endl;

        m += (m.empty() ? "" : ";") + std::string{"plus"};

        // noop
    }
};

template<>
struct report_action<inv_exec>
{
    template<typename Input>
    static void apply(const Input& in, expr_reg& m, calc_stack& s)
    {
        using namespace std;
        cout << "Rule: " << typeid(inv_exec).name() << " " << in.string() << endl;

        m += (m.empty() ? "" : ";") + std::string{"inv"};

        if ( typeid(long long) == s.front().type() )
        {
            s.front() = ~promote<long long>(s.front());
        }
        else if ( typeid(bool) == s.front().type() )
        {
            s.front() = !promote<bool>(s.front());
        }
        else
        {
            throw runtime_error("invalid argument for the inverse unary operator");
        }
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
            bool eval = any_cast<bool>(s.front());
            cout << "evaluated result: " << eval << endl;
            res = eval == (atoi(argv[3]) != 0);
        }
        else if (s.front().type() == typeid(long long))
        {
            long long eval = any_cast<long long>(s.front());
            cout << "evaluated result: " << eval << endl;
            res = eval == atoll(argv[3]);
        }
        else if (s.front().type() == typeid(long double))
        {
            long double eval = any_cast<long double>(s.front());
            cout << "evaluated result: " << eval << endl;
            res = eval == atof(argv[3]);
        }

        cout << "expected result: " << argv[3] << endl;

        ret = res ? 0 : -1;
    }
    else {
        cerr << "I don't understand." << endl;
        ret = -1;
    }

    return ret;
}
