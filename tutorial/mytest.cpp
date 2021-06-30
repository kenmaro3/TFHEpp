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

#include <chrono>
using namespace std::chrono;
inline double get_time_sec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}

int main()
{
    printf("hello, world\n\n");
    double a = -100.;
    double b = 100.;
    //double x = -20.;
    double x1 = 10.;
    double x2 = -40.;
    double x3 = -50.;

    TFHEpp::Encoder encoder1(a, b, 16);
    TFHEpp::Encoder encoder2(a, b, 16);
    TFHEpp::Encoder encoder3(a, b, 16);
    TFHEpp::Encoder encoder4(a, b, 16);
    TFHEpp::Encoder encoder5(a, b, 16);
    TFHEpp::Encoder encoder6(a, b, 16);
    TFHEpp::Encoder encoder7(a, b, 16);
    TFHEpp::Encoder encoder8(a, b, 24);

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder8);



    //cout << "\n=============================" << endl;

    //TFHEpp::TLWE<TFHEpp::lvl0param> c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder1);
    //double dec1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c1, sk->key.lvl0, encoder1);

    //printf("x1: %f, dec: %f\n", x1, dec1);
    //encoder1.print();


    //cout << "\n=============================" << endl;
    //TFHEpp::TLWE<TFHEpp::lvl0param> c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder2);
    //double dec2 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c2, sk->key.lvl0, encoder2);
    //printf("x2: %f, dec: %f\n", x2, dec2);
    //encoder2.print();

    //cout << "\n=============================" << endl;
    //TFHEpp::TLWE<TFHEpp::lvl0param> c3 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x3, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder3);
    //double dec3 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c3, sk->key.lvl0, encoder3);
    //printf("x3: %f, dec: %f\n", x3, dec3);
    //encoder3.print();



    //cout << "\n=============================" << endl;
    //TFHEpp::TLWE<TFHEpp::lvl0param> ca1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder1);
    //TFHEpp::HomADD(ca1, c1, c2, encoder1, encoder2);
    //double deca1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(ca1, sk->key.lvl0, encoder1);
    //printf("HomADD: %f, deca1: %f\n", x1+x2, deca1);
    //encoder1.print();


    //cout << "\n=============================" << endl;
    //TFHEpp::TLWE<TFHEpp::lvl0param> cs1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder2);
    //TFHEpp::HomSUB(cs1, c3, c2, encoder3, encoder2);
    //double decs1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cs1, sk->key.lvl0, encoder3);

    //printf("HomSUB: %f, decs1: %f\n", x3-x2, decs1);
    //encoder3.print();


    //cout << "\n=============================" << endl;
    //TFHEpp::TLWE<TFHEpp::lvl0param> cac1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder4);
    //double add_p1 = -0.32222;
    //TFHEpp::HomADDCONST(cac1, cac1, add_p1, encoder4);
    //double decac1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cac1, sk->key.lvl0, encoder4);
    //printf("HomADDCONST: %f, decac1: %f\n", x1 + add_p1, decac1);
    //encoder4.print();


    //cout << "\n=============================" << endl;
    //TFHEpp::TLWE<TFHEpp::lvl0param> cmc1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder1);
    //int mult_p1 = -5;
    //int max_num = 3;
    //int precision = 4;
    //double start, end;
    //start = get_time_sec();
    //TFHEpp::HomMULTCONSTINT(cmc1, cmc1, mult_p1);
    //end = get_time_sec();
    //cout << "time " << end-start << endl;
    //double decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder1);
    //printf("HomMULTCONSTINT: %f, decmc1: %f\n", x1*mult_p1, decmc1);
    //encoder1.print();


    //cout << "\n=============================" << endl;
    //double mult_p2 = -0.5;
    ////Encoder const_encoder(-1, 1, 4);
    //x1 = 10.;
    //int mult_bp = 4;
    //double mult_max = 4;

    //TFHEpp::TLWE<TFHEpp::lvl0param> cmc2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder5);
    //TFHEpp::HomMULTCONST01(cmc2, cmc2, mult_p2, encoder5, mult_bp);
    ////TFHEpp::HomMULTCONSTREAL(cmc2, cmc2, mult_p2, encoder5, mult_bp, mult_max);
    ////TFHEpp::HomMULTCONSTREALDEBUG(cmc2, cmc2, mult_p2, encoder5);
    //double decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc2, sk->key.lvl0, encoder5);
    //printf("HomMULTCONSTREAL: %f, decmc1: %f\n\n", x1*mult_p2, decmc1);
    //encoder5.print();

    //uint32_t p1 = phase<lvl0param>(cmc2, sk->key.lvl0);
    //printf("p1: %llu\n", p1);
    //printf("p2: %llu\n", UINT32_MAX);


    ////TFHEpp::TLWE<TFHEpp::lvl0param> cneg1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder6);
    ////encoder6.negate();
    ////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cneg1, sk->key.lvl0, encoder6);
    ////printf("HomNEG: %f, decmc1: %f\n\n", x1*(-1.), decmc1);
    ////encoder6.print();

    ////encoder6.negate();
    ////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cneg1, sk->key.lvl0, encoder6);
    ////printf("HomNEG: %f, decmc1: %f\n\n", x1, decmc1);
    ////encoder6.print();


    //////TFHEpp::HomMULTCONST(cmc1, cmc1, mult_p1, encoder, 2);
    //////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);
    //////printf("HomMULTCONST: %f, decmc1: %f\n\n", x1*mult_p1*mult_p1, decmc1);
    ////cout << "=============" << endl;

    ////TFHEpp::HomMULTCONST(cmc1, cmc1, mult_p2, encoder, 2);
    ////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);
    ////printf("HomMULTCONST: %f, decmc1: %f\n\n", x1*mult_p1*mult_p2, decmc1);

    ////cout << "=============" << endl;
    //////double mult_p2 = 1.25;
    //////TFHEpp::HomMULTCONST(cmc1, cmc1, mult_p2, encoder);
    //////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);

    //////printf("HomMULTCONST: %f, decmc1: %f\n\n", x1*mult_p1*mult_p2, decmc1);


    ////sk->print<lvl0param>();

    ////for(int i=0; i<=lvl0param::n; i++){
    ////    printf("%llu, ", cmc1[i]);
    ////}
    ////printf("\n");


    cout << "\n=============================" << endl;

    for(int i=0; i<1; i++){
        //x1 = a + (double)i*10.;
        x1 = 50.;
        uint32_t temp1 = encoder7.encode(x1);
        printf("original: %llu\n", temp1);
        //TFHEpp::TLWE<TFHEpp::lvl0param> c7 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder7);
        //double dec7 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c7, sk->key.lvl0, encoder7);

        //printf("x1: %f, dec: %f\n", x1, dec7);
        //encoder7.print();
    

        cout << "\n=============================" << endl;
        TLWE<lvl1param> cpbtry, cpb0, cpb1, cpb2, cpb3, cpb4, cpb5;
        TLWE<lvl0param> ca;
        TRLWE<lvl1param> cpb;
        TFHEpp::TLWE<TFHEpp::lvl0param> c8 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder7);

        //ProgrammableBootstrappingWithoutSEWITHKEY(cpb, c8, *gk.get(), encoder7, encoder8, sk->key.lvl0);
        //ProgrammableBootstrappingWithoutSE(cpb, c8, *gk.get(), encoder7, encoder8);
        //SampleExtractIndex<lvl1param>(cpb0, cpb, 0);
        //ProgrammableBootstrappingWithoutKS(cpb0, c8, *gk.get(), encoder7, encoder8);
        //double decpb0 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(cpb0, sk->key.lvl1, encoder8);
        ProgrammableBootstrapping(ca, c8, *gk.get(), encoder7, encoder8);
        double decpb0 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(ca, sk->key.lvl0, encoder8);
        printf("BS: %f ==> %f, \n", x1, decpb0);
    }
    //double decp= TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c8, sk->key.lvl0, encoder7);
    //TFHEpp::PBDEBUG<TFHEpp::lvl0param>(c8, sk->key.lvl0, encoder7, encoder8);
    //c8 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder7);
    //TFHEpp::PBDEBUG<TFHEpp::lvl0param>(c8, sk->key.lvl0, encoder7, encoder8);
    //c8 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder7);
    //printf("\n=======================\n");
    //printf("PBS: %f\n\n", decp);

    //ProgrammableBootstrappingWithoutSE(cpb, c8, *gk.get(), encoder7);


    //auto key = sk->key.lvl0;

    //uint32_t phase = c8[lvl0param::n];
    //for(int i=0; i<lvl0param::n; i++){
    //    if(key[i]==1){
    //        phase = (phase- c8[i]);
    //    }
    //}

    //cout << phase << endl;

    //int domain_n_bit = 10;
    //int target_n_bit = 4;

    //array<uint32_t, lvl0param::n+1> test1;
    //for(int i=0; i<=lvl0param::n; i++){
    //    test1[i] = (c8[i] >> (domain_n_bit - target_n_bit - 1)) % (1UL << (target_n_bit + 1));
    //}

    //uint32_t test2 = test1[lvl0param::n];
    //for(int i=0; i<lvl0param::n; i++){
    //    if(key[i]==1){
    //        test2 = (test2 - test1[i]) % (1UL << (target_n_bit + 1));
    //    }
    //}
    //cout << test2 << endl;

    //printf("\n=====================\n");
    //printf("PBS result: ");
    //for(int i=0; i<lvl1param::n; i++){
    //    TLWE<lvl1param> cpb0;
    //    SampleExtractIndex<lvl1param>(cpb0, cpb, i);
    //    double decpb0 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(cpb0, sk->key.lvl1, encoder7);
    //    printf("%f, ", decpb0);
    //}
    //    printf("\n");


    //ProgrammableBootstrappingWithoutKS(cpbtry, c8, *gk.get(), encoder7);
    //double decpbtry = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(cpbtry, sk->key.lvl1, encoder7);

    //sk->print<lvl0param>();
    ////sk->print<lvl1param>();

    //printf("\n=======================\n");
    //printf("PBS: %f\n\n", decpbtry);

}
