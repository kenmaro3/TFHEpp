#include <cloudkey.hpp>
#include <tlwe.hpp>
#include <trgsw.hpp>
#include <trlwe.hpp>

namespace TFHEpp{
    GateKey::GateKey(SecretKey sk){
        for (int i = 0; i < DEF_N; i++)
            for (int j = 0; j < DEF_t; j++)
                for (uint32_t k = 0; k < (1 << DEF_basebit) - 1; k++)
                    ksk[i][j][k] = tlweSymEncryptlvl0(
                        sk.key.lvl1[i] * (k + 1) *
                            (1U << (32 - (j + 1) * DEF_basebit)),
                        DEF_αks, sk.key.lvl0);
        for (int i = 0; i < DEF_n/DEF_Addends; i++){
            bklvl01[i][0] = trgswSymEncryptlvl1(
                static_cast<int32_t>(sk.key.lvl0[2*i]*sk.key.lvl0[2*i+1]), DEF_αbk, sk.key.lvl1);
            bklvl01[i][1] = trgswSymEncryptlvl1(
                static_cast<int32_t>(sk.key.lvl0[2*i]*(sk.key.lvl0[2*i+1]-1)), DEF_αbk, sk.key.lvl1);
            bklvl01[i][2] = trgswSymEncryptlvl1(
                static_cast<int32_t>((sk.key.lvl0[2*i]-1)*sk.key.lvl0[2*i+1]), DEF_αbk, sk.key.lvl1);
            bklvl01[i][3] = trgswSymEncryptlvl1(
                static_cast<int32_t>((sk.key.lvl0[2*i]-1)*(sk.key.lvl0[2*i+1]-1)), DEF_αbk, sk.key.lvl1);
        }
    }

    CircuitKey::CircuitKey(SecretKey sk){
        array<uint32_t, DEF_nbar + 1> key;
        for (int i = 0; i < DEF_nbar; i++) key[i] = sk.key.lvl2[i];
        key[DEF_nbar] = -1;
#pragma omp parallel for collapse(4)
        for (int z = 0; z < 2; z++)
            for (int i = 0; i <= DEF_nbar; i++)
                for (int j = 0; j < DEF_tbar; j++)
                    for (int u = 0; u < (1 << DEF_basebitlvl21) - 1; u++) {
                        TRLWElvl1 c =
                            trlweSymEncryptZerolvl1(DEF_αprivks, sk.key.lvl1);
                        c[z][0] += (u + 1) * key[i]
                                   << (32 - (j + 1) * DEF_basebitlvl21);
                        privksk[z][i][j][u] = c;
                    }
        for (int i = 0; i < DEF_n/DEF_Addends; i++){
            bklvl02[i][0] = trgswSymEncryptlvl2(
                static_cast<int32_t>(sk.key.lvl0[2*i]*sk.key.lvl0[2*i+1]), DEF_αbklvl02, sk.key.lvl2);
            bklvl02[i][1] = trgswSymEncryptlvl2(
                static_cast<int32_t>(sk.key.lvl0[2*i]*(sk.key.lvl0[2*i+1]-1)), DEF_αbklvl02, sk.key.lvl2);
            bklvl02[i][2] = trgswSymEncryptlvl2(
                static_cast<int32_t>((sk.key.lvl0[2*i]-1)*sk.key.lvl0[2*i+1]), DEF_αbklvl02, sk.key.lvl2);
            bklvl02[i][3] = trgswSymEncryptlvl2(
                static_cast<int32_t>((sk.key.lvl0[2*i]-1)*(sk.key.lvl0[2*i+1]-1)), DEF_αbklvl02, sk.key.lvl2);
        }
    }
}