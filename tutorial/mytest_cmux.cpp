#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <tfhe++.hpp>

using namespace std;
using namespace TFHEpp;

int main()
{
    SecretKey *sk = new SecretKey;
    cout << "hello, world" << endl;

    bool p1;
    bool p0;

    TRLWE<TFHEpp::lvl1param> cres;
    TRGSWFFT<TFHEpp::lvl1param> c_flag;

    TRLWE<TFHEpp::lvl1param> c0;
    TRLWE<TFHEpp::lvl1param> c1;

    TRLWE<TFHEpp::lvl0param> c0_tmp;
    TRLWE<TFHEpp::lvl0param> c1_tmp;

    TLWE<TFHEpp::lvl1param> c0_lwe;
    TLWE<TFHEpp::lvl1param> c1_lwe;

    TLWE<TFHEpp::lvl0param> c0_lwe_tmp;
    TLWE<TFHEpp::lvl0param> c1_lwe_tmp;

    p0 = 0;
    p1 = 1;

    if (p0 == 0) {
        c0_lwe = TFHEpp::tlweSymEncrypt<TFHEpp::lvl1param>(
            -TFHEpp::lvl1param::mu, TFHEpp::lvl1param::alpha, sk->key.lvl1);
    }
    else {
        c0_lwe = TFHEpp::tlweSymEncrypt<TFHEpp::lvl1param>(
            TFHEpp::lvl1param::mu, TFHEpp::lvl1param::alpha, sk->key.lvl1);
    }

    if (p1 == 0) {
        c1_lwe = TFHEpp::tlweSymEncrypt<TFHEpp::lvl1param>(
            -TFHEpp::lvl1param::mu, TFHEpp::lvl1param::alpha, sk->key.lvl1);
    }
    else {
        c1_lwe = TFHEpp::tlweSymEncrypt<TFHEpp::lvl1param>(
            TFHEpp::lvl1param::mu, TFHEpp::lvl1param::alpha, sk->key.lvl1);
    }

    // TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl0param>(c0_tmp, c0_lwe_tmp,
    // 0);
    TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c0, c0_lwe, 0);
    // TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c0, c0_lwe, 1);
    // TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c0, c0_lwe, 2);

    TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c1, c1_lwe, 0);
    // TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c1, c1_lwe, 1);
    // TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c1, c1_lwe, 2);
    //  TFHEpp::InverseSampleExtractIndex<TFHEpp::lvl1param>(c1, c1_lwe, 1);

    // Polynomial<TFHEpp::lvl1param> plainpoly = {};
    // plainpoly[0] = 1;
    // c_flag = trgswfftSymEncrypt<TFHEpp::lvl1param>(
    //     plainpoly, TFHEpp::lvl1param::alpha, sk->key.lvl1);

    // CMUXFFT<lvl1param>(cres, c_flag, c1, c0);

    Polynomial<TFHEpp::lvl1param> plainpoly = {};
    plainpoly[0] = 0;
    c_flag = trgswfftSymEncrypt<TFHEpp::lvl1param>(
        plainpoly, TFHEpp::lvl1param::alpha, sk->key.lvl1);

    CMUXFFT<lvl1param>(cres, c_flag, c1, c0);

    array<bool, TFHEpp::lvl1param::n> res0;
    array<bool, TFHEpp::lvl1param::n> res1;
    array<bool, TFHEpp::lvl1param::n> res_cmux;
    res0 = trlweSymDecrypt<TFHEpp::lvl1param>(c0, sk->key.lvl1);
    res1 = trlweSymDecrypt<TFHEpp::lvl1param>(c1, sk->key.lvl1);
    res_cmux = trlweSymDecrypt<TFHEpp::lvl1param>(cres, sk->key.lvl1);

    // for (int i=0; i<lvl1param::n; i++){
    //     printf("%d, %d", p1[i], res[i]);
    //     printf("\n");
    // }
    // for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
    //    printf("%d,", p1[i]);
    //}
    // printf("\n");

    // for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
    //     printf("%d,", res[i]);
    // }
    // printf("%d, ", res0[0]);
    // printf("%d, ", res0[1]);
    // printf("%d, ", res0[2]);
    // printf("\n");
    // printf("%d, ", res1[0]);
    // printf("%d, ", res1[1]);
    // printf("%d, ", res1[2]);
    printf("%d, ", res_cmux[0]);
    printf("\n");
}