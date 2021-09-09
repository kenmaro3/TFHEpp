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
        auto encoder1 = Encoder(-dist_max, dist_max, 31);
        auto encoder2 = Encoder(-dist_max, dist_max, 31);

        init(function, encoder1, encoder2, seed_gen);
    }

    bool assert_bootstrap(double result, double arg)
    {
        auto function = T();
        auto expected = function.run(arg);

        return abs(expected - result) < permit_error;
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

    auto result = true;

    for (int test = 0; test < num_test; test++) {
        result &= identity_tester.test_bootstrap();
        result &= relu_tester.test_bootstrap();
        result &= sigmoid_tester.test_bootstrap();
    }

    if (result)
        cout << "Passed" << endl;
    else {
        cout << "Error" << endl;
    }
}