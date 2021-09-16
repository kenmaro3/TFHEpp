#include <cassert>
#include <iostream>
#include <random>
#include <tester.hpp>

using namespace std;
using namespace TFHEpp;

#define CALC_ARGS                                                   \
    TFHEpp::TLWE<lvl0param> &c3, const TFHEpp::TLWE<lvl0param> &c1, \
        const TFHEpp::TLWE<lvl0param> &c2, Encoder &encoder_domain, \
        Encoder &encoder_target, const double x1, const double x2, GateKey *gk

class AddOrSubTester : public AbstructBootstrapTester {
public:
    int dist_max = 100;
    double permit_error = dist_max / 100;
    std::function<double(CALC_ARGS)> calc;

    AddOrSubTester(random_device &seed_gen,
                   std::function<double(CALC_ARGS)> calc)
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

        c1 = encrypt(x1, encoder);
        c2 = encrypt(x2, encoder);

        double expected = calc(c3, c1, c2, encoder, encoder, x1, x2, NULL);

        double d = decrypt(c3, encoder);

        return assert_test(expected, d, false);
    }
};

double add_gate(CALC_ARGS)
{
    TFHEpp::HomADD(c3, c1, c2, encoder_domain, encoder_target);
    return x1 + x2;
};

double add_fixed_gate(CALC_ARGS)
{
    TFHEpp::HomADDFixedEncoder(c3, c1, c2, encoder_domain, encoder_target);
    return x1 + x2;
};

double add_const_gate(CALC_ARGS)
{
    TFHEpp::HomADDCONST(c3, c1, x2, encoder_domain);
    return x1 + x2;
};

double sub_gate(CALC_ARGS)
{
    TFHEpp::HomSUB(c3, c1, c2, encoder_domain, encoder_target);
    return x1 - x2;
};

double sub_fixed_gate(CALC_ARGS)
{
    TFHEpp::HomSUBFixedEncoder(c3, c1, c2, encoder_domain, encoder_target);
    return x1 - x2;
};

class MulTester : public AbstructBootstrapTester {
public:
    int dist_max = 20;
    double permit_error = pow(dist_max, 2) / 15;

    MulTester(random_device &seed_gen)
    {
        init(function, seed_gen, dist_max, permit_error);
    }

    bool test() override
    {
        TLWE<TFHEpp::lvl0param> c1, c2, c3;

        double x1 = dist(engine);
        double x2 = dist(engine);

        TFHEpp::Encoder encoder_domain(-40, 40, 31);
        TFHEpp::Encoder encoder_target(-400, 400, 31);

        c1 = encrypt(x1, encoder_domain);
        c2 = encrypt(x2, encoder_domain);

        TFHEpp::HomMULT(c3, c1, c2, *gk(encoder_domain).get(), encoder_domain,
                        encoder_domain, encoder_target);

        auto _gk = gk(encoder_domain);

        double expected = x1 * x2;

        double d = decrypt(c3, encoder_target);
        return assert_test(expected, d, false);
    }
};

int main()
{
    random_device seed_gen;

    auto add_tester = AddOrSubTester(seed_gen, add_gate);
    auto add_fixed_tester = AddOrSubTester(seed_gen, add_fixed_gate);
    auto add_const = AddOrSubTester(seed_gen, add_const_gate);
    auto sub_tester = AddOrSubTester(seed_gen, sub_gate);
    auto sub_fixed_tester = AddOrSubTester(seed_gen, sub_fixed_gate);
    auto mult_tester = MulTester(seed_gen);

    std::vector<AbstructBootstrapTester *> testers{
        &add_tester, &add_fixed_tester, &add_const,
        &sub_tester, &sub_fixed_tester, &mult_tester};

    test(testers);
}