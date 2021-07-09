#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <cassert>

using namespace std;
using namespace TFHEpp;

#include <chrono>
using namespace std::chrono;
inline double get_time_sec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}


void print_vec_1d(vector<double> x){
    for(int i=0; i<x.size(); i++){
        printf("%f, ", x[i]);
    }
    printf("\n");
}

void print_vec_2d(vector<vector<double>> x){
    for(int i=0; i<x.size(); i++){
        print_vec_1d(x[i]);
    }
    printf("\n");
}

int main(){
    printf("hello, world\n\n");
    double encoder_a = -2.;
    double encoder_b = 2.;
    int bs_bp = 24;

    TFHEpp::Encoder encoder_bs(encoder_a, encoder_b, bs_bp);

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder_bs);

    double x = -0.4;
    double x2 = 0.6;
    double d, mult;
    mult = 0.5;
    Encoder encoder(-2,2,16);
    Encoder encoder2(-2,2,16);
    TLWE<lvl0param> c1, c2, c3;
    c1 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x, lvl0param::alpha, sk->key.lvl0, encoder);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder);
    printf("original x: %f\n", d);
    
    TFHEpp::HomMULTCONSTREAL(c2, c1, mult, encoder2, 6, 2);
    d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c2, sk->key.lvl0, encoder2);
    printf("mult x: %f\n", d);
    encoder2.print();

    printf("\n===============================\n");
    //TFHEpp::HomMULTCONSTREAL(c2, c1, mult, encoder2, 6, 2);
    //TFHEpp::HomMULTCONSTINT(c2, c1, 2, encoder);
    //TFHEpp::HomMULTCONSTREAL3(c2, c1, 2.2, encoder);
    //TFHEpp::HomMULTCONSTREAL3(c2, c1, 2.2, encoder2, 6, 4);
    //d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c2, sk->key.lvl0, encoder2);
    //printf("x: %f = %f\n",x*mult, d);

    TLWE<lvl1param> c1_tmp;
    printf("\n===============================\n");
    for(int i=0; i<1; i++){
        if(i==0){
            TFHEpp::ProgrammableBootstrapping(c1, c1, *gk.get(), encoder, encoder_bs, my_identity_function);
            //TFHEpp::ProgrammableBootstrappingWithoutKS(c1_tmp, c1, *gk.get(), encoder, encoder_bs, my_identity_function);
        }else{
            TFHEpp::ProgrammableBootstrapping(c1, c1, *gk.get(), encoder_bs, encoder_bs, my_identity_function);
            //TFHEpp::ProgrammableBootstrappingWithoutKS(c1_tmp, c1_tmp, *gk.get(), encoder_bs, encoder_bs, my_identity_function);
        }
        //for(int j=0; j<=lvl0param::n; j++){
        //    printf("c1[%d]: %llu\n", j, c1[j]);
        //    c1[j] = c1[j] >> 8;
        //}
        //printf("\n");
        //encoder_bs.update(encoder_bs.a, encoder_bs.b, encoder_bs.bp-8);
        d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c1, sk->key.lvl0, encoder_bs);
        //d = TFHEpp::tlweSymDecryptDecode<lvl1param>(c1_tmp, sk->key.lvl1, encoder_bs);
        printf("bs: %f = %f\n",x, d);
    }
    printf("\n===============================\n");
    encoder_bs.print();
    
    //TFHEpp::HomMULTCONSTREAL(c1, c1, mult, encoder_bs, 6, 2);
    //TFHEpp::HomMULTCONSTINT(c1, c1, 2, encoder_bs);
    //TFHEpp::HomMULTCONSTREAL2(c1, c1, 2.2, encoder_bs);
    
    //c3 = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x2, lvl0param::alpha, sk->key.lvl0, encoder_bs);
    //d = TFHEpp::tlweSymDecryptDecode<lvl0param>(c3, sk->key.lvl0, encoder_bs);
    //printf("before conversion x: %f = %f\n",x2, d);
    array<uint64_t, lvl0param::n+1> c1_2;
    //TFHEpp::convert_t_to_t2(c1_2, c1);
    TFHEpp::convert_t_to_t2(c1_2, c1);
    for(int i=0; i<=lvl0param::n; i++){
        printf("%d: %llu, ", i, c1_2[i]);
    }
    printf("\n");
    d = TFHEpp::tlweSymDecryptDecode64<lvl0param>(c1_2, sk->key.lvl0, encoder_bs);
    printf("after conversion x: %f = %f\n",x, d);
    //printf("after conversion x: %f = %f\n",x2, d);
    encoder_bs.print();
    //return 0;



    for(int i=0; i<1; i++){
        Encoder encoder_tmp(encoder_bs.a, encoder_bs.b, encoder_bs.bp);
        //Encoder encoder_tmp(encoder.a, encoder.b, encoder.bp);
        //array<uint64_t, lvl0param::n+1> c1_2;
        //TFHEpp::convert_t_to_t2(c1_2, c1);
        mult = 2.;
        //for(int j=0; j<=lvl0param::n; j++){
        //    printf("c1_2[%d]: %llu\n", j, c1_2[j]);
        //}
        //printf("\n");
        //TFHEpp::HomMULTCONSTREAL4(c1_2, c1_2, mult, encoder_tmp, 6, 4);
        TFHEpp::HomMULTCONSTREAL4(c1_2, c1_2, mult, encoder_tmp, 6, 4);
        //TFHEpp::HomMULTCONSTINT(c1_2, c1_2, 3, encoder_bs);
        d = TFHEpp::tlweSymDecryptDecode64<lvl0param>(c1_2, sk->key.lvl0, encoder_tmp);
        printf("x: %f = %f\n",x*mult, d);
        encoder_tmp.print();
    }
    //encoder_bs.print();
    


}