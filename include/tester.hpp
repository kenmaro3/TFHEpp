#pragma once

#include <cassert>
#include <iostream>
#include <random>

#include "tfhe++.hpp"

namespace TFHEpp {

constexpr uint32_t num_test = 20;

class AbstructBootstrapTester {
public:
    int dist_max = 100;
    double permit_error = dist_max / 10;

    CustomTestVector<TFHEpp::lvl1param> *function;
    std::unique_ptr<TFHEpp::SecretKey> sk;
    uniform_int_distribution<> dist;
    default_random_engine engine;

    void init(CustomTestVector<TFHEpp::lvl1param> *function,
              random_device &seed_gen, int dist_max, double permit_error)
    {
        this->function = function;
        this->dist_max = dist_max;
        this->permit_error = permit_error;

        sk = std::make_unique<TFHEpp::SecretKey>();

        engine = default_random_engine(seed_gen());
        dist = uniform_int_distribution<>(-dist_max, dist_max);
    }

    bool assert_test(double expected, double res, bool output = false)
    {
        double diff = abs(expected - res);
        bool is_succeeded = diff < permit_error;

        if (!is_succeeded || output) {
            std::cerr << "----\nexpected: " << expected << "\nresult :" << res
                      << std::endl;

            std::cerr << diff << "(diff) > " << permit_error << "(permit)"
                      << std::endl;
        }
        return is_succeeded;
    }

    std::unique_ptr<TFHEpp::GateKey> gk(Encoder &encoder)
    {
        return std::make_unique<TFHEpp::GateKey>(*sk, encoder);
    }

    TLWE<TFHEpp::lvl0param> encrypt(double x, Encoder &encoder)
    {
        return TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    }

    double decrypt(TLWE<TFHEpp::lvl0param> &c, Encoder &encoder)
    {
        return TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c, sk->key.lvl0,
                                                               encoder);
    }

    virtual bool test() = 0;
};

void test(vector<AbstructBootstrapTester *> testers)
{
    bool result = true;

    for (int test = 0; test < num_test; test++)
        for (auto &tester : testers) result &= tester->test();

    if (result)
        cout << "Passed" << endl;
    else {
        cout << "Error" << endl;
    }
}

}  // namespace TFHEpp