#include <cassert>
#include <iostream>
#include <random>
#include <tester.hpp>

using namespace std;
using namespace TFHEpp;

template <typename T>
class SameEncoderBoostrapTester : public AbstructBootstrapTester {
public:
    SameEncoderBoostrapTester(random_device &seed_gen)
    {
        auto function = new T();

        int dist_max = 100;
        double permit_error = dist_max / 10;

        init(function, seed_gen, dist_max, permit_error);
    }

    bool test() override
    {
        double x = (double)dist(engine);

        auto encoder_domain = Encoder(-dist_max, dist_max, 31);

        TLWE<TFHEpp::lvl0param> c0 =
            TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
                x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder_domain);

        ProgrammableBootstrapping(c0, c0, *gk(encoder_domain).get(),
                                  encoder_domain, encoder_domain, *function);

        double res = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            c0, sk->key.lvl0, encoder_domain);

        auto function = T();
        auto expected = function.run(x);

        return assert_test(res, expected);
    }

    ~SameEncoderBoostrapTester() { delete function; }
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

        printf("%lf x %lf = %lf ~= %lf\n\n", x1, x2, x1 * x2, d);

        return assert_test(abs(x1 * x2), d, false);
    }
};

int main()
{
    random_device seed_gen;

    auto identity_tester =
        SameEncoderBoostrapTester<IdentityFunction>(seed_gen);
    auto sigmoid_tester = SameEncoderBoostrapTester<SigmoidFunction>(seed_gen);
    auto relu_tester = SameEncoderBoostrapTester<ReLUFunction>(seed_gen);
    auto mul_tester = MulTester(seed_gen);

    auto result = true;

    for (int test = 0; test < num_test; test++) {
        result &= identity_tester.test();
        result &= relu_tester.test();
        result &= sigmoid_tester.test();
        result &= mul_tester.test();
    }

    if (result)
        cout << "Passed" << endl;
    else {
        cout << "Error" << endl;
    }
}