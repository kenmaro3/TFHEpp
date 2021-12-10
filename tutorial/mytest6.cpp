#include <stdio.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tfhe++.hpp>
#include <vector>

using namespace TFHEpp;
using namespace std;

#include <chrono>
using namespace std::chrono;
inline double get_time_msec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000;
}

void print_vec(vector<double> x){
    for (int i=0; i<x.size(); i++){
        printf("%f, ", x[i]);
    }
    printf("\n");
}

void mul_test()
{
    printf("hello, world\n");

    double border = 20;
    TFHEpp::Encoder encoder(border*(-1), border, 32);
    TFHEpp::Encoder target_encoder(border*(-1), border, 32);

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();

    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    double x =  1.2;
    double x2 = 2.2;

    TLWE<TFHEpp::lvl0param> c1, c2, c3;

    c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
        x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
    c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
        x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);

    // double d1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c1, sk->key.lvl0, encoder);
    // double d2 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c2, sk->key.lvl0, encoder);
    double start, stop;

    vector<double> res_ratio;
    vector<double> res_time;
    for(int i=0; i<100; i++){
        printf("\n%d th iteration==================", i);
        c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);
        c2 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x2, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);

        start = get_time_msec();
        TFHEpp::HomMULT(c3, c1, c2, *gk.get(), encoder, encoder, target_encoder);
        stop = get_time_msec();

        double d3 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c3, sk->key.lvl0,
                                                                target_encoder);
        
        double ratio = abs(d3-x*x2)/border;
        res_ratio.emplace_back(ratio);
        res_time.emplace_back(stop-start);
    }
    printf("\n\n");
    print_vec(res_ratio);
    print_vec(res_time);
}

int main()
{
    printf("hello, world\n");
    mul_test();
}
