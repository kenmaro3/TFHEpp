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
    double a = -100.;
    double b = 100.;
    TFHEpp::Encoder encoder(a, b, 31);
    TFHEpp::Encoder encoder_ksk(0, 1, 31);

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);


    vector<double> p_vec(lvl1param::n);
    //for(int i=0; i<lvl1param::n; i++) p_vec[i] = (double)(i)/100;
    for(int i=0; i<5; i++) p_vec[i] = (double)(i);

    cout << "lvl1" << endl;
    lweKey key;
    array<double, lvl1param::n> pmu;
    for (int i = 0; i < lvl1param::n; i++)
        pmu[i] =  p_vec[i];

    TRLWE<TFHEpp::lvl1param> c1 = trlweSymEncodeEncrypt<lvl1param>(pmu, lvl1param::alpha, key.lvl1, encoder);
    TRLWE<TFHEpp::lvl1param> c2 = trlweSymEncodeEncrypt<lvl1param>(pmu, lvl1param::alpha, key.lvl1, encoder);
    array<double, lvl1param::n> d1 = trlweSymDecryptDecode<lvl1param>(c1, key.lvl1, encoder);
    array<double, lvl1param::n> d2 = trlweSymDecryptDecode<lvl1param>(c2, key.lvl1, encoder);
    for(int i=0; i<10; i++) printf("%f, ", d1[i]);
    printf("\n");
    for(int i=0; i<10; i++) printf("%f, ", d2[i]);
    printf("\n");

    //TRLWE<TFHEpp::lvl1param> c3 = trlweSymEncryptZero<lvl1param>(lvl1param::alpha, key.lvl1);
    TRLWE<TFHEpp::lvl1param> c3;
    HomADD(c3, c1, c2);
    array<double, lvl1param::n> d3 = trlweSymDecryptDecode<lvl1param>(c3, key.lvl1, encoder);
    for(int i=0; i<10; i++) printf("%f, ", d3[i]);
    printf("\n");

    //TRLWE<TFHEpp::lvl1param> c4 = trlweSymEncryptZero<lvl1param>(lvl1param::alpha, key.lvl1);
    TRLWE<TFHEpp::lvl1param> c4;
    HomSUB(c4, c1, c2);
    array<double, lvl1param::n> d4 = trlweSymDecryptDecode<lvl1param>(c4, key.lvl1, encoder);
    for(int i=0; i<10; i++) printf("%f, ", d4[i]);
    printf("\n");

    vector<double> ex_res(10);
    TLWE<lvl1param> ex_1;
    for(int i=0; i<10; i++){
        const int index = 0;
        SampleExtractIndex<lvl1param>(ex_1, c1, i);
        double decex_1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(ex_1, sk->key.lvl1, encoder);
        ex_res[i] = decex_1;

    }
    printf("\nSE:\n");
    for(int i=0; i<10; i++){
        printf("%f, ", ex_res[i]);
    }
    printf("\n");


    TLWE<lvl0param> ex_0;
    for(int i=0; i<10; i++){
        const int index = 0;
        SampleExtractIndex<lvl1param>(ex_1, c1, i);
        //IdentityKeySwitch<lvl10param>(ex_0, ex_1, gk->ksk);
        IdentityKeySwitchWITHEncoder<lvl10param>(ex_0, ex_1, gk->ksk, encoder, encoder);
        double decex_0 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(ex_0, sk->key.lvl0, encoder);
        ex_res[i] = decex_0;

    }
    printf("\nSE -> KS:\n");
    for(int i=0; i<10; i++){
        printf("%f, ", ex_res[i]);
    }
    printf("\n");


    //printf("\nISA:\n");
    //double x1 = 12.;
    //TLWE<lvl1param> c5 = tlweSymEncodeEncrypt<lvl1param>(x1, lvl1param::alpha, sk->key.lvl1, encoder);
    //TRLWE<lvl1param> c6 = trlweSymEncryptZero<lvl1param>(lvl1param::alpha, sk->key.lvl1);
    //InverseSampleExtractIndex<lvl1param>(c6, c5, 1);
    //double dec5 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(c5, sk->key.lvl1, encoder);
    //printf(": %f, dec5: %f\n\n", x1, dec5);
    //array<double, lvl1param::n> d6 = trlweSymDecryptDecode<lvl1param>(c6, key.lvl1, encoder);
    //for(int i=0; i<10; i++) printf("%f, ", d6[i]);
    //printf("\n");

    //printf("\nHomADDCONST:\n");
    //array<double, lvl1param::n> x2;
    //for(int i=0; i<6; i++) x2[i] = 10. - double(i);
    //TRLWE<lvl1param> c7 = trlweSymEncryptZero<lvl1param>(lvl1param::alpha, sk->key.lvl1);
    //HomADDCONST(c7, c7, x2, encoder);
    //array<double, lvl1param::n> d7 = trlweSymDecryptDecode<lvl1param>(c7, key.lvl1, encoder);
    //for(int i=0; i<10; i++) printf("%f, ", d7[i]);
    //printf("\n");


}