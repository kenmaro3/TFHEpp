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
    printf("hello, mytest3\n");
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk);


    double a = 0.;
    double b = 100.;

    Encoder encoder(a, b, 24);

    double x1 = 3.;
    double x2 = 4.5;

    double test1 = encoder.decode(encoder.encode(x1));

    auto test2 = encoder.encode(x1);
    auto test3 = encoder.encode(x2);
    auto test4 = test2 + test3;
    auto test5 = test2 * x2;
    double test6 = encoder.decode(test4);
    double test7 = encoder.decode(test5);
    cout << test6 << endl;
    cout << test7 << endl;





}