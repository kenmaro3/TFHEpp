#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <tfhe++.hpp>

using namespace TFHEpp;

int main()
{
    constexpr uint32_t num_test = 10;
    double time = 0;

    random_device seed_gen;
    default_random_engine engine(seed_gen());
    uniform_int_distribution<uint32_t> binary(0, 1);
    SecretKey sk;
    GateKey *gk = new GateKey(sk);
    PackingKey *pack = new PackingKey(sk);

    vector<array<bool, DEF_N>> p(num_test);
    for(int h = 0;h<num_test;h++) for (bool &i : p[h]) i = binary(engine) > 0;
    vector<PrepackTLWElvl1> carray(num_test);
    for(int h =0;h<num_test;h++) for (int i = 0; i<DEF_N;i++) carray[h][i] = tlweSymEncryptlvl1(p[h][i]?DEF_μ:-DEF_μ,DEF_αbk,sk.key.lvl1);
    array<TRLWElvl1,num_test> d;

    chrono::system_clock::time_point start, end;
    start = chrono::system_clock::now();
    for (int test = 0; test < num_test; test++) {

        Packinglvl1(d[test],carray[test],(*pack));
    }
    end = chrono::system_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    array<array<bool, DEF_N>,num_test> p2;
    for(int h =0;h<num_test;h++) p2[h] = trlweSymDecryptlvl1(d[h], sk.key.lvl1);
    for(int h =0;h<num_test;h++) for (int i = 0; i < DEF_N; i++) assert(static_cast<int>(p[h][i]) == static_cast<int>(p2[h][i]));
    cout << "Passed" << endl;
    cout << time/num_test << "ms" << endl;
}