#include <cassert>
#include <fstream>
#include <iostream>
#include <tfhe++.hpp>

int main()
{
    TFHEpp::SecretKey sk;
    {
        {
            std::ofstream ofs{"./secret.key", std::ios::binary};
            cereal::PortableBinaryOutputArchive ar(ofs);
            sk.serialize(ar);
        }
        TFHEpp::SecretKey ski;
        {
            std::ifstream ifs{"./secret.key", std::ios::binary};
            cereal::PortableBinaryInputArchive ar(ifs);
            ski.serialize(ar);
        }
        for (int i = 0; i < TFHEpp::lvl0param::n; i++)
            assert(sk.key.lvl0[i] == ski.key.lvl0[i]);
        for (int i = 0; i < TFHEpp::lvl1param::n; i++)
            assert(sk.key.lvl1[i] == ski.key.lvl1[i]);
        for (int i = 0; i < TFHEpp::lvl2param::n; i++)
            assert(sk.key.lvl2[i] == ski.key.lvl2[i]);
        std::cout << "n"
                  << ":" << ski.params.lvl0.n << std::endl;
        assert(sk.params == ski.params);
    }

    {
        TFHEpp::IOpacket iopacket;
        iopacket.tlwelvl0ports["test"].resize(1);
        iopacket.tlwelvl0ports["test"][0] =
            TFHEpp::tlweSymEncrypt<TFHEpp::lvl0param>(
                TFHEpp::lvl0param::mu, TFHEpp::lvl0param::alpha, sk.key.lvl0);
        {
            std::ofstream ofs{"./iopacket.data", std::ios::binary};
            cereal::PortableBinaryOutputArchive ar(ofs);
            iopacket.serialize(ar);
        }
    }
}