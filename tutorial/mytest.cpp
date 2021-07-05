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
    double x2 = 40.;
    double x3 = -50.;

    TFHEpp::Encoder encoder1(a, b, 16);
    TFHEpp::Encoder encoder2(a, b, 16);
    TFHEpp::Encoder encoder3(a, b, 16);
    TFHEpp::Encoder encoder4(a, b, 16);
    TFHEpp::Encoder encoder5(a, b, 16);
    TFHEpp::Encoder encoder6(a, b, 16);
    TFHEpp::Encoder encoder7(a, b, 16);
    TFHEpp::Encoder encoder8(a, b, 16);

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder8);


    TLWE<lvl0param> c1, c2;

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
    //c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder1);
    //c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder2);
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


    cout << "\n\n=============================" << endl;
    cout << "PBS entire range test" << endl;

    for(int i=0; i<19; i++){
        x1 = a + (double)i*10.;
        uint32_t temp1 = encoder7.encode(x1);
        printf("original: %llu\n", temp1);
        cout << "\n=============================" << endl;
        TLWE<lvl1param> cpbtry, cpb0, cpb1, cpb2, cpb3, cpb4, cpb5;
        TFHEpp::TLWE<TFHEpp::lvl0param> c8 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder7);
        ProgrammableBootstrapping(c8, c8, *gk.get(), encoder7, encoder8, my_relu_function);
        double decpb0 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c8, sk->key.lvl0, encoder8);
        printf("BS: %f ==> %f, \n", x1, decpb0);
    }


    cout << "\n\n=============================" << endl;
    cout << "PBS and ADDCONST later" << endl;
    for(int i=0; i<5; i++){
        cout << "\n=============================" << endl;
        double x1 = -5.;
        double x2 = 20.;
        TLWE<lvl0param> enc_x1, enc_x2, enc_bs, enc_add;
        Encoder encoder1(-100., 100., 16);
        Encoder encoder2(-100., 100., 16);
        enc_x1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x1, lvl0param::alpha, sk->key.lvl0, encoder1);
        TFHEpp::ProgrammableBootstrapping(enc_bs, enc_x1, *gk.get(), encoder1, encoder1, my_identity_function);
        TFHEpp::HomADDCONST(enc_add, enc_bs, x2, encoder1);
        double dec_add = TFHEpp::tlweSymDecryptDecode<lvl0param>(enc_add, sk->key.lvl0, encoder1);
        printf("dec_add: %f = %f\n",x1+x2, dec_add);

    }

    cout << "\n\n=============================" << endl;
    cout << "PBS 2 times and ADD later" << endl;
    for(int i=0; i<5; i++){
        cout << "\n=============================" << endl;
        double x1 = 0.;
        double x2 = 20.;
        TLWE<lvl0param> enc_x1, enc_x2, enc_bs1, enc_bs2, enc_add;
        Encoder encoder1(-100., 100., 16);
        Encoder encoder2(-100., 100., 16);
        enc_x1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x1, lvl0param::alpha, sk->key.lvl0, encoder1);
        enc_x2 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x2, lvl0param::alpha, sk->key.lvl0, encoder2);
        TFHEpp::ProgrammableBootstrapping(enc_bs1, enc_x1, *gk.get(), encoder1, encoder1, my_identity_function);
        TFHEpp::ProgrammableBootstrapping(enc_bs1, enc_bs1, *gk.get(), encoder1, encoder1, my_identity_function);
        TFHEpp::ProgrammableBootstrapping(enc_bs2, enc_x2, *gk.get(), encoder2, encoder2, my_identity_function);
        //double dec_bs1 = TFHEpp::tlweSymDecryptDecode<lvl0param>(enc_bs1, sk->key.lvl0, encoder1);
        //printf("dec_bs1: %f = %f\n",x1, dec_bs1);
        //double dec_bs2 = TFHEpp::tlweSymDecryptDecode<lvl0param>(enc_bs2, sk->key.lvl0, encoder2);
        //printf("dec_bs2: %f = %f\n",x2, dec_bs2);
        TFHEpp::HomADDFixedEncoder(enc_add, enc_bs1, enc_bs2, encoder1, encoder2);
        double dec_add = TFHEpp::tlweSymDecryptDecode<lvl0param>(enc_add, sk->key.lvl0, encoder1);
        printf("dec_add: %f = %f\n",x1+x2, dec_add);

    }

    cout << "\n\n=============================" << endl;
    cout << "PBS 2 times and SUB later" << endl;
    for(int i=0; i<5; i++){
        cout << "\n=============================" << endl;
        double x1 = 0.;
        double x2 = 20.;
        TLWE<lvl0param> enc_x1, enc_x2, enc_bs, enc_sub;
        Encoder encoder1(-100., 100., 16);
        Encoder encoder2(-100., 100., 16);
        enc_x1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x1, lvl0param::alpha, sk->key.lvl0, encoder1);
        enc_x2 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x2, lvl0param::alpha, sk->key.lvl0, encoder2);
        TFHEpp::ProgrammableBootstrapping(enc_bs, enc_x1, *gk.get(), encoder1, encoder1, my_identity_function);
        TFHEpp::ProgrammableBootstrapping(enc_bs, enc_bs, *gk.get(), encoder1, encoder1, my_identity_function);
        double dec_bs = TFHEpp::tlweSymDecryptDecode<lvl0param>(enc_bs, sk->key.lvl0, encoder1);
        printf("dec_bs: %f = %f\n",x1, dec_bs);
        TFHEpp::HomSUBFixedEncoder(enc_sub, enc_bs, enc_x2, encoder1, encoder2);
        double dec_sub = TFHEpp::tlweSymDecryptDecode<lvl0param>(enc_sub, sk->key.lvl0, encoder1);
        printf("dec_sub: %f = %f\n",x1-x2, dec_sub);

    }

    cout << "\n\n=============================" << endl;
    cout << "MAX function" << endl;
    for(int i=0; i<5; i++){
        cout << "\n=============================" << endl;
        TLWE<lvl0param> test_x, test_y, test_x_minus_y, test_bs1, test_res;
        Encoder encoder_x(a, b, 16);
        Encoder encoder_y(a, b, 16);
        Encoder encoder_bs(a, b, 16);
        double p_x = 30.;
        double p_y = -20.;
        test_x = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(p_x, lvl0param::alpha, sk->key.lvl0, encoder_x);
        test_y = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(p_y, lvl0param::alpha, sk->key.lvl0, encoder_y);

        TFHEpp::HomMAX(test_res, test_x, test_y, encoder_x, encoder_y, encoder_bs, *gk.get());
        double dec_res = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(test_res, sk->key.lvl0, encoder_bs);
        printf("MAX: %f\n", dec_res);
    }

}
