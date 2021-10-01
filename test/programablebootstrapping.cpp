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

template <typename T>
class DirectCustomTestVectorTester : public AbstructBootstrapTester {
public:
    std::array<std::array<lvl1param::T, lvl1param::n>, 2> testvector;

    DirectCustomTestVectorTester(random_device &seed_gen)
    {
        int dist_max = 100;
        double permit_error = dist_max / 10;

        init(function, seed_gen, dist_max, permit_error);
    }

    bool test() override
    {
        auto relu = ReLUFunction<lvl1param>();

        double x = (double)dist(engine);
        auto encoder = Encoder(-dist_max, dist_max, 31);

        auto c = encrypt(x, encoder);

        relu.custom_test_vector(testvector, c[1], encoder, encoder);
        auto direct = DirectCustomTestVector<lvl1param>(testvector);

        ProgrammableBootstrapping(c, c, *gk(encoder).get(), encoder, encoder,
                                  direct);

        double res = decrypt(c, encoder);

        auto expected = x > 0 ? x : 0;

        return assert_test(res, expected);
    }

    ~DirectCustomTestVectorTester() { delete function; }
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

    auto direct_tester =
        DirectCustomTestVectorTester<ReLUFunction<TFHEpp::lvl1param>>(seed_gen);

    std::vector<AbstructBootstrapTester *> testers{
        &identity_tester, &sigmoid_tester, &relu_tester, &direct_tester};

    test(testers);
}