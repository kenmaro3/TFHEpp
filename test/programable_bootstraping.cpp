#include <cassert>
#include <iostream>
#include <random>
#include <tfhe++.hpp>
#

using namespace std;
using namespace TFHEpp;

int main()
{
    constexpr uint32_t num_test = 100;
    constexpr int dist_max = 100;
    constexpr double permit_error = dist_max / 20;

    IdentityFunction identity_function = IdentityFunction();
    TFHEpp::Encoder encoder(-dist_max, dist_max, 31);

    random_device seed_gen;
    default_random_engine engine(seed_gen());
    uniform_int_distribution<> dist(-dist_max + permit_error,
                                    dist_max - permit_error);

    auto sk = std::make_unique<TFHEpp::SecretKey>();
    auto gk = std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    for (int test = 0; test < num_test; test++) {
        double x = (double)dist(engine);

        auto c = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);

        ProgrammableBootstrapping(c, c, *gk.get(), encoder, encoder,
                                  identity_function);

        double res = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            c, sk->key.lvl0, encoder);

        if (abs(x - res) > permit_error) {
            std::cerr << "expected: " << x << "\nresult :" << res << std::endl;
            assert(false);
        }
    }

    cout << "Passed" << endl;
}