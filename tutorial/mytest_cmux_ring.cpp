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

    array<bool, TFHEpp::lvl1param::n> p1;
    array<bool, TFHEpp::lvl1param::n> p0;
    array<uint32_t, TFHEpp::lvl1param::n> pmu1;
    array<uint32_t, TFHEpp::lvl1param::n> pmu0;

    TRLWE<TFHEpp::lvl1param> cres;
    TRGSWFFT<TFHEpp::lvl1param> c_flag;

    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
        if (i < 10) {
            p1[i] = 1;
        }
        else {
            p1[i] = 0;
        }
    }
    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
        p0[i] = 0;
    }

    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
        if (p1[i] == 0) {
            pmu1[i] = -TFHEpp::lvl1param::mu;
        }
        else {
            pmu1[i] = TFHEpp::lvl1param::mu;
        }
    }

    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
        if (p0[i] == 0) {
            pmu0[i] = -TFHEpp::lvl1param::mu;
        }
        else {
            pmu0[i] = TFHEpp::lvl1param::mu;
        }
    }

    TRLWE<TFHEpp::lvl1param> c1 = trlweSymEncrypt<TFHEpp::lvl1param>(
        pmu1, TFHEpp::lvl1param::alpha, sk->key.lvl1);
    TRLWE<TFHEpp::lvl1param> c0 = trlweSymEncrypt<TFHEpp::lvl1param>(
        pmu0, TFHEpp::lvl1param::alpha, sk->key.lvl1);

    Polynomial<TFHEpp::lvl1param> plainpoly = {};
    plainpoly[0] = 1;
    c_flag = trgswfftSymEncrypt<TFHEpp::lvl1param>(
        plainpoly, TFHEpp::lvl1param::alpha, sk->key.lvl1);

    CMUXFFT<lvl1param>(cres, c_flag, c1, c0);
    array<bool, TFHEpp::lvl1param::n> res;
    res = trlweSymDecrypt<TFHEpp::lvl1param>(cres, sk->key.lvl1);

    // for (int i=0; i<lvl1param::n; i++){
    //     printf("%d, %d", p1[i], res[i]);
    //     printf("\n");
    // }
    // for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
    //    printf("%d,", p1[i]);
    //}
    // printf("\n");

    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
        printf("%d,", res[i]);
    }
    printf("\n");
    printf("\n");
}