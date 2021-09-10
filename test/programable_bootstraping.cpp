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

int main()
{
    random_device seed_gen;

    auto identity_tester =
        SameEncoderBoostrapTester<IdentityFunction>(seed_gen);
    auto sigmoid_tester = SameEncoderBoostrapTester<SigmoidFunction>(seed_gen);
    auto relu_tester = SameEncoderBoostrapTester<ReLUFunction>(seed_gen);

    std::vector<AbstructBootstrapTester *> testers{
        &identity_tester, &sigmoid_tester, &relu_tester};

    test(testers);
}