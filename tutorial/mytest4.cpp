#include <stdio.h>

#include <cassert>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>

using namespace std;
using namespace TFHEpp;

#include <chrono>
using namespace std::chrono;
inline double get_time_msec(void)
{
    return static_cast<double>(duration_cast<nanoseconds>(
                                   steady_clock::now().time_since_epoch())
                                   .count()) /
           1000000;
}

void print_vec_1d(vector<double> x)
{
    for (int i = 0; i < x.size(); i++) {
        printf("%f, ", x[i]);
    }
    printf("\n");
}

void print_vec_2d(vector<vector<double>> x)
{
    for (int i = 0; i < x.size(); i++) {
        print_vec_1d(x[i]);
    }
    printf("\n");
}

class MyltiplyFunction : public AbstructFunction {
    double y;
    MyltiplyFunction(double y) { this->y = y; }

    double run(double x, double y) { return x * y; }
};

int main()
{
    printf("hello, world\n\n");
    double encoder_a = -20.;
    double encoder_b = 20.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder_bs(encoder_a, encoder_b, bs_bp);
    IdentityFunction identity_function = IdentityFunction();

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder_bs);

    double x = 10.6;
    double x2 = 5.6;
    double d, mult;
    Encoder encoder(encoder_a, encoder_b, 32);
    Encoder encoder2(encoder_a, encoder_b, 32);
    TLWE<lvl0param> c1, c2, c3;
    TLWE<lvl1param> c1p, c2p, c3p;
    c1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x, lvl0param::alpha,
                                                 sk->key.lvl0, encoder);
    c2 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x2, lvl0param::alpha,
                                                 sk->key.lvl0, encoder);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);
    encoder.print();

    printf("\n===============================\n");
    for (int i = 1; i < 19; i++) {
        double x = encoder_a + encoder.d / 2. / 19. * double(i);
        TLWE<lvl0param> c1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(
            x, lvl0param::alpha, sk->key.lvl0, encoder);
        double d =
            TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
        printf("x %f = %f\n", x, d);
    }

    double start, end;
    vector<double> ts;
    printf("\n===============================\n");
    for (int i = 0; i < 10; i++) {
        if (i == 0) {
            start = get_time_msec();
            TFHEpp::ProgrammableBootstrapping(c1, c1, *gk.get(), encoder,
                                              encoder_bs, identity_function);
            end = get_time_msec();
            ts.push_back(end - start);
            d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0,
                                                        encoder_bs);
            printf("bs  %f = %f\n", x, d);
            TFHEpp::ProgrammableBootstrapping(c2, c2, *gk.get(), encoder,
                                              encoder_bs, identity_function);
            d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c2, sk->key.lvl0,
                                                        encoder_bs);
            printf("bs %f = %f\n", x2, d);
        }
        else {
            start = get_time_msec();
            TFHEpp::ProgrammableBootstrapping(c1, c1, *gk.get(), encoder_bs,
                                              encoder_bs, identity_function);
            end = get_time_msec();
            ts.push_back(end - start);
            d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0,
                                                        encoder_bs);
            printf("bs  %f = %f\n", x, d);
        }
    }
    double avg = 0;
    for (int i = 0; i < ts.size(); i++) {
        avg += ts[i];
    }
    avg = avg / double(ts.size());
    printf("bs avg: %f\n", avg);

    TLWE<lvl0param> test1;
    TFHEpp::HomMAX(test1, c1, c2, encoder_bs, encoder_bs, encoder_bs,
                   *gk.get());
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(test1, sk->key.lvl0,
                                                encoder_bs);
    printf("max of (%f, %f) = %f\n", x, x2, d);
    return 0;
}
