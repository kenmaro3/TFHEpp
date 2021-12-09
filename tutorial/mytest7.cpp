#include <stdio.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tfhe++.hpp>
#include <vector>

using namespace TFHEpp;
using namespace std;

void mul_test()
{
    lweKey key;
    array<double, TFHEpp::lvl1param::n> a_double_array, b_double_array,
        c_double_array, d_double_array;
    TFHEpp::Polynomial<TFHEpp::lvl1param> a_int_array, b_int_array, c_int_array;

    TFHEpp::Encoder encoder(0, 20, 31);

    a_double_array.fill(0);
    b_double_array.fill(0);
    
    a_int_array.fill(0);
    b_int_array.fill(0);

    for (int i = 0; i < 3; i++) {
        a_double_array[i] = i + 1;
        b_double_array[i] = i + 1;

        a_int_array[i] = i + 1;
        b_int_array[i] = i + 1;
    }

    TRLWE<TFHEpp::lvl1param> c1, c2;

    c1 = TFHEpp::trlweSymEncodeEncrypt<TFHEpp::lvl1param>(
        a_double_array, TFHEpp::lvl1param::alpha, key.lvl1, encoder);

    puts("---- input(decrypted) ----");
    c_double_array =
        TFHEpp::trlweSymDecryptDecode<TFHEpp::lvl1param>(c1, key.lvl1, encoder);
    for (int i = 0; i < 10; i++) printf("%lf\n", c_double_array[i]);

    puts("---- mul result(decrypted) ----");
    TFHEpp::HomMULTCONST(c2, c1, b_double_array, encoder);

    d_double_array =
        TFHEpp::trlweSymDecryptDecode<TFHEpp::lvl1param>(c2, key.lvl1, encoder);

    for (int i = 0; i < 10; i++) printf("%lf\n", d_double_array[i]);

    puts("---- mul expected ----");
    TFHEpp::PolyMul<TFHEpp::lvl1param>(c_int_array, a_int_array, b_int_array);
    for (int i = 0; i < 10; i++) printf("%d\n", c_int_array[i]);
}

int main()
{
    printf("hello, world\n");
    mul_test();
}
