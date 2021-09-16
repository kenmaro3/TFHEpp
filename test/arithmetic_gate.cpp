#include <cassert>
#include <iostream>
#include <random>
#include <tester.hpp>

using namespace std;
using namespace TFHEpp;

#define ADD_OR_SUB_ARG                                              \
    TFHEpp::TLWE<lvl0param> &c3, const TFHEpp::TLWE<lvl0param> &c1, \
        const TFHEpp::TLWE<lvl0param> &c2, Encoder &encoder_domain, \
        Encoder &encoder_target, const double x1, const double x2

class AddOrSubTester : public AbstructBootstrapTester {
public:
    int dist_max = 100;
    double permit_error = dist_max / 100;
    std::function<double(ADD_OR_SUB_ARG)> calc;

    AddOrSubTester(random_device &seed_gen,
                   std::function<double(ADD_OR_SUB_ARG)> calc)
    {
        init(function, seed_gen, dist_max, permit_error);
        this->calc = calc;
    }

    bool test() override
    {
        TLWE<TFHEpp::lvl0param> c1, c2, c3;

        double x1 = dist(engine);
        double x2 = dist(engine);

        TFHEpp::Encoder encoder(-dist_max * 2, dist_max * 2, 31);

        c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
        c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);

        double expected = calc(c3, c1, c2, encoder, encoder, x1, x2);

        double d = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            c3, sk->key.lvl0, encoder);

        return assert_test(expected, d, false);
    }
};

double add_gate(ADD_OR_SUB_ARG)
{
    TFHEpp::HomADD(c3, c1, c2, encoder_domain, encoder_target);
    return x1 + x2;
};

double add_fixed_gate(ADD_OR_SUB_ARG)
{
    TFHEpp::HomADDFixedEncoder(c3, c1, c2, encoder_domain, encoder_target);
    return x1 + x2;
};

double add_const_gate(ADD_OR_SUB_ARG)
{
    TFHEpp::HomADDCONST(c3, c1, x2, encoder_domain);
    return x1 + x2;
};

double sub_gate(ADD_OR_SUB_ARG)
{
    TFHEpp::HomSUB(c3, c1, c2, encoder_domain, encoder_target);
    return x1 - x2;
};

// Note:
// if the function named sub_fixed_gate, the compilation fail...
// But I don't know the reason... :(
double sub_fixed_gate_(ADD_OR_SUB_ARG)
{
    TFHEpp::HomSUBFixedEncoder(c3, c1, c2, encoder_domain, encoder_target);
    return x1 - x2;
};

class MulTester : public AbstructBootstrapTester {
public:
    int dist_max = 20;
    double permit_error = pow(dist_max, 2) / 10;

    MulTester(random_device &seed_gen)
    {
        init(function, seed_gen, dist_max, permit_error);
    }

    bool test() override
    {
        TLWE<TFHEpp::lvl0param> c1, c2, c3;

        double x1 = dist(engine);
        x1 = abs(x1);

        double x2 = (double)dist(engine);
        x2 = abs(x2);

        TFHEpp::Encoder encoder_domain(-40, 40, 32);
        TFHEpp::Encoder encoder_target(-40, 1800, 32);

        c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder_domain);
        c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder_domain);

        TFHEpp::HomMUL(c3, c1, c2, *gk(encoder_domain).get(), encoder_domain,
                       encoder_domain, encoder_target);

        double d = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            c3, sk->key.lvl0, encoder_target);

        // printf("%lf x %lf = %lf ~= %lf\n\n", x1, x2, x1
        // * x2, d);

        return assert_test(abs(x1 * x2), d, false);
    }
};

int main()
{
    random_device seed_gen;

    auto mul_tester = MulTester(seed_gen);
    auto add_tester = AddOrSubTester(seed_gen, add_gate);
    auto add_fixed_tester = AddOrSubTester(seed_gen, add_fixed_gate);
    auto add_const = AddOrSubTester(seed_gen, add_const_gate);
    auto sub_tester = AddOrSubTester(seed_gen, sub_gate);
    auto sub_fixed_gate = AddOrSubTester(seed_gen, sub_fixed_gate_);

    std::vector<AbstructBootstrapTester *> testers{
        &add_tester, &add_fixed_tester, &add_const,
        &mul_tester, &sub_tester,       &sub_fixed_gate};

    test(testers);
}