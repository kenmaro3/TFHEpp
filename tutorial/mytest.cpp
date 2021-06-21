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
    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk);

    double a = 0.;
    double b = 100.;
    //double x = -20.;
    double x1 = 10.;
    double x2 = 40.;



    TFHEpp::Encoder encoder(a, b, 16);

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

    TFHEpp::TLWE<TFHEpp::lvl0param> cac1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    double add_p1 = 0.32222;
    TFHEpp::HomADDCONST(cac1, c1, add_p1, encoder);
    double decac1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cac1, sk->key.lvl0, encoder);

    printf("HomADDCONST: %f, decac1: %f\n\n", x1 + add_p1, decac1);


    cout << "=============" << endl;

    TFHEpp::TLWE<TFHEpp::lvl0param> cmc1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(x1, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    int mult_p1 = 5;
    double mult_p2 = 1.2;
    int max_num = 3;
    int precision = 4;
    double start, end;
    start = get_time_sec();
    TFHEpp::HomMULTCONSTINT(cmc1, c1, mult_p1);
    end = get_time_sec();
    cout << "time " << end-start << endl;
    double decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);
    printf("HomMULTCONSTINT: %f, decmc1: %f\n\n", x1*mult_p1, decmc1);

    Encoder const_encoder(0, 2, 8);

    TFHEpp::HomMULTCONSTREAL(cmc1, c1, mult_p2, encoder, const_encoder);
    cout << "encoder.a " << encoder.a << endl;
    cout << "encoder.b " << encoder.b << endl;
    cout << "encoder.d " << encoder.d << endl;
    cout << "encoder.bp " << encoder.bp << endl;
    end = get_time_sec();
    cout << "time " << end-start << endl;
    decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);
    printf("HomMULTCONSTREAL: %f, decmc1: %f\n\n", x1*mult_p2, decmc1);

    ////TFHEpp::HomMULTCONST(cmc1, cmc1, mult_p1, encoder, 2);
    ////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);
    ////printf("HomMULTCONST: %f, decmc1: %f\n\n", x1*mult_p1*mult_p1, decmc1);
    //cout << "=============" << endl;

    //TFHEpp::HomMULTCONST(cmc1, cmc1, mult_p2, encoder, 2);
    //decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);
    //printf("HomMULTCONST: %f, decmc1: %f\n\n", x1*mult_p1*mult_p2, decmc1);

    //cout << "=============" << endl;
    ////double mult_p2 = 1.25;
    ////TFHEpp::HomMULTCONST(cmc1, cmc1, mult_p2, encoder);
    ////decmc1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cmc1, sk->key.lvl0, encoder);

    ////printf("HomMULTCONST: %f, decmc1: %f\n\n", x1*mult_p1*mult_p2, decmc1);


    //sk->print<lvl0param>();

    //for(int i=0; i<=lvl0param::n; i++){
    //    printf("%llu, ", cmc1[i]);
    //}
    //printf("\n");


  

    //TLWE<lvl0param> cpb1;
    //ProgrammableBootstrapping(cpb1, c1, *gk.get());
    //double decpb1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(cpb1, sk->key.lvl0, encoder);

    //printf("PBS: %f, decmc1: %f\n\n", x1, decpb1);


}
