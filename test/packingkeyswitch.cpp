#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <tfhe++.hpp>

using namespace TFHEpp;

int main()
{
    cout << "lvl1" << endl;
    const uint32_t num_test = 10;
    double time = 0;
    chrono::system_clock::time_point start, end;
    for (int test = 0; test < num_test; test++) {
        random_device seed_gen;
        default_random_engine engine(seed_gen());
        uniform_int_distribution<uint32_t> binary(0, 1);

        SecretKey sk;
        GateKey *gk = new GateKey(sk);
        PackingKey *packksk = new PackingKey(sk);
        array<bool, DEF_N> p;
        for (bool &i : p) i = binary(engine) > 0;
        array<uint32_t, DEF_N> pmu;
        for (int i = 0; i < DEF_N; i++) pmu[i] = p[i] ? DEF_μ : -DEF_μ;
        array<TLWElvl1,DEF_N> carray;
        for (int i = 0; i<DEF_N;i++) carray[i] = tlweSymEncryptlvl1(pmu[i],DEF_αbk,sk.key.lvl1);
        TRLWElvl1 d;
        array<TLWElvl0,DEF_N> earray;
        start = chrono::system_clock::now();
        for(int i = 0; i<DEF_N; i++) IdentityKeySwitchlvl10(earray[i],carray[i],(*gk).ksk);
        PackingKeySwitchlvl01(d,earray,(*packksk).packksk);
        end = chrono::system_clock::now();
        time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        array<bool, DEF_N> p2 = trlweSymDecryptlvl1(d, sk.key.lvl1);
        for (int i = 0; i < DEF_N; i++) assert(p[i] == p2[i]);
        cout<<test<<endl;
    }
    cout << "Passed" << endl;
    cout << time/num_test << endl;
}