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

    Encoder encoder(a, b);

    double x1 = 10.;
    double x2 = 50.;

    uint32_t tmp1 = encoder.encode(x1);
    printf("%llu\n", tmp1);
    uint32_t tmp2 = encoder.encode(x2);
    printf("%llu\n", tmp2);

    double tmpd = encoder.encode_0_1(x1);
    printf("%f\n", tmpd);

    tmpd = tmpd * 3.2;

    double tmpd2 = encoder.decode_0_1(tmpd);
    printf("%f\n", tmpd2);

    tmpd = encoder.encode_0_1(x2);
    printf("%f\n", tmpd);

    tmpd2 = encoder.decode_0_1(tmpd);
    printf("%f\n", tmpd2);




}