#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace TFHEpp;

int main()
{
    printf("hello, world\n\n");
    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk);

    double a = 0.;
    double b = 100.;
    //double x = -20.;
    double x1 = 20.;
    double x2 = 40.;

    //uint32_t p1 = dtot32(0.1);
    //uint32_t p2 = dtot32(0.2);
    //auto c1 = tlweSymEncrypt<lvl0param>(p1, lvl0param::alpha, sk->key.lvl0);
    //auto c2 = tlweSymEncrypt<lvl0param>(p2, lvl0param::alpha, sk->key.lvl0);
    //uint32_t d1 = tlweSymDecryptRaw<lvl0param>(c1, sk->key.lvl0);
    //uint32_t d2 = tlweSymDecryptRaw<lvl0param>(c2, sk->key.lvl0);
    //printf("alpha: %d\n", dtot32(lvl0param::alpha));
    //printf("p1: %d\n", p1);
    //printf("d1: %d\n", d1);
    //printf("p1: %d\n", p2);
    //printf("d1: %d\n", d2);
    

    //auto c3 = tlweSymEncrypt<lvl0param>(p1, lvl0param::alpha, sk->key.lvl0);
    //HomADD(c3, c1, c2);
    //uint32_t d3 = tlweSymDecryptRaw<lvl0param>(c3, sk->key.lvl0);
    //printf(": %d\n", p1+p2);
    //printf(": %d\n", d3);

    //auto c4 = tlweSymEncrypt<lvl0param>(p1, lvl0param::alpha, sk->key.lvl0);
    //HomSUB(c4, c2, c1);
    //uint32_t d4 = tlweSymDecryptRaw<lvl0param>(c4, sk->key.lvl0);
    //printf(": %d\n", p2-p1);
    //printf(": %d\n", d4);



    TFHEpp::Encoder encoder(a, b);
    //double test1 = encoder.encode(x);
    //cout << test1 << endl;
    //double test2 = encoder.encode(x+x);
    //cout << test2 << endl;

    TFHEpp::TLWE<TFHEpp::lvl0param> c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    double dec1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c1, sk->key.lvl0, encoder);

    printf("x1: %f, dec: %f\n\n", x1, dec1);


    TFHEpp::TLWE<TFHEpp::lvl0param> c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    double dec2 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c2, sk->key.lvl0, encoder);
    printf("x2: %f, dec: %f\n\n", x2, dec2);


    TFHEpp::TLWE<TFHEpp::lvl0param> ca1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    TFHEpp::HomADD(ca1, c1, c2);
    double deca1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(ca1, sk->key.lvl0, encoder);
    printf("HomADD: %f, deca1: %f\n\n", x1+x2, deca1);


    TFHEpp::TLWE<TFHEpp::lvl0param> cs1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    TFHEpp::HomSUB(cs1, c2, c1);
    double decs1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cs1, sk->key.lvl0, encoder);

    printf("HomSUB: %f, decs1: %f\n\n", x2-x1, decs1);

}
