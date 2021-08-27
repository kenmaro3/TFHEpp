#include <stdio.h>

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
inline double get_time_sec(void)
{
    return static_cast<double>(duration_cast<nanoseconds>(
                                   steady_clock::now().time_since_epoch())
                                   .count()) /
           1000000000;
}

int main()
{
    double a = -100.;
    double b = 100.;
    double x1 = 10.;
    double x2 = 40.;
    double x3 = -50.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder_bs(a, b, bs_bp);
    IdentityFunction identity_function = IdentityFunction();

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder_bs);

    TLWE<lvl0param> c1, c2;

    cout << "\n\n=============================" << endl;
    cout << "BS then summation" << endl;

    for (int i = 0; i < 1; i++) {
        double d1, d2, d3;
        TLWE<lvl0param> c1, c2, c3;
        x1 = 20.;
        x2 = 30.;
        Encoder encoder(-100., 100., 32);
        cout << "\n=============================" << endl;
        TLWE<lvl1param> cpb1;
        TLWE<lvl0param> cpb0;
        c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
        c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
        ProgrammableBootstrapping(c1, c1, *gk.get(), encoder, encoder_bs,
                                  identity_function);
        ProgrammableBootstrapping(c2, c2, *gk.get(), encoder, encoder_bs,
                                  identity_function);
        d1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c1, sk->key.lvl0,
                                                             encoder);
        d2 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c2, sk->key.lvl0,
                                                             encoder);
        TFHEpp::HomADDFixedEncoder(c3, c1, c2, encoder, encoder);
        d3 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c3, sk->key.lvl0,
                                                             encoder);
        printf("x1: %f ==> %f, \n", x1, d1);
        printf("x2: %f ==> %f, \n", x2, d2);
        printf("x3: %f ==> %f, \n", x1 + x2, d3);
    }

    return 0;

    cout << "\n\n=============================" << endl;
    cout << "PBS entire range test" << endl;

    for (int i = 0; i < 19; i++) {
        x1 = a + (double)i * 10.;
        Encoder encoder(-100., 100., 31);
        cout << "\n=============================" << endl;
        TLWE<lvl1param> cpb1;
        TLWE<lvl0param> cpb0;
        TLWE<lvl0param> ct = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
        double dec = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            ct, sk->key.lvl0, encoder);
        printf("x: %f ==> %f, \n", x1, dec);
        ProgrammableBootstrappingWithoutKS(cpb1, ct, *gk.get(), encoder,
                                           encoder_bs, identity_function);
        ProgrammableBootstrapping(cpb0, ct, *gk.get(), encoder, encoder_bs,
                                  identity_function);

        double decpb = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(
            cpb1, sk->key.lvl1, encoder_bs);
        printf("BS1: %f ==> %f, \n", x1, decpb);
        decpb = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(
            cpb0, sk->key.lvl0, encoder_bs);
        printf("BS2: %f ==> %f, \n", x1, decpb);
    }
}
