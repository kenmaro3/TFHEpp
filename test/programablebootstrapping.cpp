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
        auto encoder = Encoder(-dist_max, dist_max, 31);

        auto c = encrypt(x, encoder);

        ProgrammableBootstrapping(c, c, *gk(encoder).get(), encoder, encoder,
                                  *function);

        double res = decrypt(c, encoder);

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
        SameEncoderBoostrapTester<IdentityFunction<TFHEpp::lvl1param>>(
            seed_gen);
    auto sigmoid_tester =
        SameEncoderBoostrapTester<SigmoidFunction<TFHEpp::lvl1param>>(seed_gen);
    auto relu_tester =
        SameEncoderBoostrapTester<ReLUFunction<TFHEpp::lvl1param>>(seed_gen);

    std::vector<AbstructBootstrapTester *> testers{
        &identity_tester, &sigmoid_tester, &relu_tester};

    test(testers);
}