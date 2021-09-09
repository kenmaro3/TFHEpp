#pragma once

#include <cassert>
#include <iostream>
#include <random>

#include "tfhe++.hpp"

namespace TFHEpp {
constexpr int dist_max = 100;
constexpr double permit_error = dist_max / 10;
constexpr uint32_t num_test = 100;

class AbstructBootstrapTester {
public:
    AbstructFunction *function;
    Encoder encoder_domain;
    Encoder encoder_target;
    std::unique_ptr<TFHEpp::GateKey> gk;
    std::unique_ptr<TFHEpp::SecretKey> sk;
    uniform_int_distribution<> dist;
    default_random_engine engine;

    void init(AbstructFunction *function, Encoder &encoder_domain,
              Encoder &encoder_target, random_device &seed_gen)
    {
        this->function = function;

        this->encoder_domain = encoder_domain;
        this->encoder_target = encoder_target;

        sk = std::make_unique<TFHEpp::SecretKey>();
        gk = std::make_unique<TFHEpp::GateKey>(*sk, encoder_domain);

        engine = default_random_engine(seed_gen());
        dist = uniform_int_distribution<>(-dist_max + permit_error,
                                          dist_max - permit_error);
    }

    bool test_bootstrap()
    {
        double expected = (double)dist(engine);

        TLWE<TFHEpp::lvl0param> c0 =
            TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
                expected, TFHEpp::lvl0param::alpha, sk->key.lvl0,
                encoder_domain);

        ProgrammableBootstrapping(c0, c0, *gk.get(), encoder_domain,
                                  encoder_target, *function);

        double res = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            c0, sk->key.lvl0, encoder_domain);

        if (!assert_bootstrap(res, expected)) {
            std::cerr << "----\nexpected: " << expected << "\nresult :" << res
                      << std::endl;

            return false;
        }

        return true;
    };

    virtual bool assert_bootstrap(double result, double expected) = 0;
};
}  // namespace TFHEpp