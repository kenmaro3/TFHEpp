#include <stdio.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tfhe++.hpp>
#include <vector>

using namespace TFHEpp;
using namespace std;

void ser_deser_test()
{
    printf("hello, world\n");

    double encoder_a = -20.;
    double encoder_b = 20.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder(encoder_a, encoder_b, bs_bp);
    IdentityFunction identity_function = IdentityFunction<lvl1param>();

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    double x = 10.6;
    double x2 = 5.6;
    double d, mult;
    TLWE<lvl0param> c1, c2, c3;
    TLWE<lvl1param> c1p, c2p, c3p;
    c1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x, lvl0param::alpha,
                                                 sk->key.lvl0, encoder);
    c2 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x2, lvl0param::alpha,
                                                 sk->key.lvl0, encoder);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);
    // encoder.print();

    TFHEpp::ProgrammableBootstrapping(c1, c1, *gk.get(), encoder, encoder,
                                      identity_function);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);

    {
        std::ofstream ofs{"./gk.key", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        gk->serialize(ar);
    }

    std::unique_ptr<TFHEpp::GateKey> gki = std::make_unique<TFHEpp::GateKey>();
    {
        std::ifstream ifs{"./gk.key", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        gki->serialize(ar);
    }

    TFHEpp::ProgrammableBootstrapping(c1, c1, *gki.get(), encoder, encoder,
                                      identity_function);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);

    {
        std::ofstream ofs{"./sk.key", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        sk->serialize(ar);
    }

    std::unique_ptr<TFHEpp::SecretKey> ski =
        std::make_unique<TFHEpp::SecretKey>();
    {
        std::ifstream ifs{"./sk.key", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ski->serialize(ar);
    }

    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, ski->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);

    {
        std::ofstream ofs{"./ct.data", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(c1);
    }

    TFHEpp::TLWE<lvl0param> ci;
    {
        std::ifstream ifs{"./ct.data", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ci);
    }

    TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, ski->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);
}

int main()
{
    printf("hello, world\n");

    double encoder_a = -20.;
    double encoder_b = 20.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder(encoder_a, encoder_b, bs_bp);
    IdentityFunction identity_function = IdentityFunction<lvl1param>();

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    double x = 10.6;
    double x2 = 5.6;
    double d, mult;
    TLWE<lvl0param> c1, c2, c3;
    TLWE<lvl1param> c1p, c2p, c3p;
    c1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x, lvl0param::alpha,
                                                 sk->key.lvl0, encoder);
    c2 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x2, lvl0param::alpha,
                                                 sk->key.lvl0, encoder);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);
    // encoder.print();

    TFHEpp::ProgrammableBootstrapping(c1, c1, *gk.get(), encoder, encoder,
                                      identity_function);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);

    ////////////////////////////////////
    std::stringstream ss;

    {
        cereal::PortableBinaryOutputArchive ar(ss);
        gk->serialize(ar);
    }

    std::unique_ptr<TFHEpp::GateKey> gki = std::make_unique<TFHEpp::GateKey>();
    {
        cereal::PortableBinaryInputArchive ar(ss);
        gki->serialize(ar);
    }

    TFHEpp::ProgrammableBootstrapping(c1, c1, *gki.get(), encoder, encoder,
                                      identity_function);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);

    {
        cereal::PortableBinaryOutputArchive ar(ss);
        sk->serialize(ar);
    }

    std::unique_ptr<TFHEpp::SecretKey> ski =
        std::make_unique<TFHEpp::SecretKey>();
    {
        cereal::PortableBinaryInputArchive ar(ss);
        ski->serialize(ar);
    }

    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, ski->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);

    {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(c1);
    }

    TFHEpp::TLWE<lvl0param> ci;
    {
        cereal::PortableBinaryInputArchive ar(ss);
        ar(ci);
    }

    TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, ski->key.lvl0, encoder);
    printf("original %f = %f\n", x, d);
}
