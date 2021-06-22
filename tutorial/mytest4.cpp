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
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk);

    double a = -10.;
    double b = 10.;
    int bp = 16;

    double x = 2.;

    Encoder encoder1(a, b, bp);

    TLWE<TFHEpp::lvl1param> c1 = tlweSymEncodeEncrypt<lvl1param>(x, lvl1param::alpha, sk->key.lvl1, encoder1);
    double dec1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl1param>(c1, sk->key.lvl1, encoder1);
    cout << dec1 << endl;
    encoder1.print();

    TLWE<lvl0param> ex_0;
    //const int index = 0;
    //SampleExtractIndex<lvl1param>(ex_1, c1, i);
    IdentityKeySwitch<lvl10param>(ex_0, c1, gk->ksk);
    dec1 = TFHEpp::tlweSymDecryptDecode<TFHEpp::lvl0param>(ex_0, sk->key.lvl0, encoder1);
    cout << dec1 << endl;
    encoder1.print();


}