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

void pbs_test()
{
    double start, stop;
    double res_time_skgen;
    double res_time_gkgen;
    int res_size_lut, res_size_ctxt;

    res_size_lut = TFHEpp::lvl1param::n * 2;
    res_size_ctxt = TFHEpp::lvl0param::n + 1;

    double border = 20;
    TFHEpp::Encoder encoder(-border, border, 32);
    TFHEpp::Encoder target_encoder(-border, border, 32);

    // generate a random key
    start = get_time_msec();
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    stop = get_time_msec();
    res_time_skgen = stop - start;

    start = get_time_msec();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);
    stop = get_time_msec();
    res_time_gkgen = stop - start;

    double x =  1.2;

    TFHEpp::TLWE<TFHEpp::lvl0param> c1, c1_pbs;
    TFHEpp::TLWE<TFHEpp::lvl1param> c2, c2_pbs;


    // double d1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c1, sk->key.lvl0, encoder);
    // double d2 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c2, sk->key.lvl0, encoder);

    vector<double> res_ratio;
    vector<double> res_ratio_without_ks;
    vector<double> res_time;
    vector<double> res_time_without_ks;
    vector<double> res_lut_gen;
    for(int i=0; i<10; i++)
    {
        // printf("\n%d th iteration==================", i);
        c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
            x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);

        


        start = get_time_msec();
        std::array<std::array<typename TFHEpp::lvl1param::T, TFHEpp::lvl1param::n>, 2> testvector;
        auto programable_boostrap_function = IdentityFunction<TFHEpp::lvl1param>();
        programable_boostrap_function.custom_test_vector(testvector, encoder, target_encoder);
        stop = get_time_msec();
        res_lut_gen.emplace_back(stop - start);

        start = get_time_msec();
        ProgrammableBootstrapping(c1_pbs, c1, *gk.get(), encoder, target_encoder,
                                  programable_boostrap_function);
        stop = get_time_msec();
        res_time.emplace_back(stop - start);

        start = get_time_msec();
        ProgrammableBootstrappingWithoutKS(c2_pbs, c1, *gk.get(), encoder, target_encoder,
                                  programable_boostrap_function);
        stop = get_time_msec();
        res_time_without_ks.emplace_back(stop - start);



        double d1_pbs = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(c1_pbs, sk->key.lvl0,
                                                                target_encoder);

        double d2_pbs = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(c2_pbs, sk->key.lvl1,
                                                                target_encoder);


        double ratio = abs((d1_pbs - x)/ border);
        double ratio_without_ks = abs((d2_pbs - x)/ border);
        res_ratio.emplace_back(ratio);
        res_ratio_without_ks.emplace_back(ratio_without_ks);
    }
    printf("\n\n");
    printf("%d\n", res_size_ctxt);
    printf("%d\n", res_size_lut);
    printf("%f\n", res_time_skgen);
    printf("%f\n", res_time_gkgen);
    print_vec(res_ratio);
    print_vec(res_ratio_without_ks);
    print_vec(res_time);
    print_vec(res_time_without_ks);
    print_vec(res_lut_gen);
}

int main()
{
    pbs_test();
}
