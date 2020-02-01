#include <cloudkey.hpp>

namespace TFHEpp {
void IdentityKeySwitchlvl10(TLWElvl0 &res, const TLWElvl1 &tlwe,
                            const KeySwitchingKey &ksk)
{
    constexpr uint32_t prec_offset = 1U << (32 - (1 + DEF_basebit * DEF_t));
    constexpr uint32_t mask = (1U << DEF_basebit) - 1;
    res = {};
    res[DEF_n] = tlwe[DEF_N];
    for (int i = 0; i < DEF_N; i++) {
        const uint32_t aibar = tlwe[i] + prec_offset;
        for (int j = 0; j < DEF_t; j++) {
            const uint32_t aij = (aibar >> (32 - (j + 1) * DEF_basebit)) & mask;
            if (aij != 0)
                for (int k = 0; k <= DEF_n; k++)
                    res[k] -= ksk[i][j][aij - 1][k];
        }
    }
}

void PrivKeySwitchlvl21(TRLWElvl1 &res, const TLWElvl2 &tlwe, const int u,
                        const PrivKeySwitchKey &privksk)
{
    constexpr uint64_t prec_offset = 1UL
                                 << (64 - (1 + DEF_basebitlvl21 * DEF_tbar));
    constexpr uint32_t mask = (1 << DEF_basebitlvl21) - 1;

    res = {};

    for (int i = 0; i <= DEF_nbar; i++) {
        uint64_t aibar = tlwe[i] + prec_offset;

        for (int j = 0; j < DEF_tbar; j++) {
            const uint64_t aij =
                (aibar >> (64 - (j + 1) * DEF_basebitlvl21)) & mask;

            if (aij != 0) {
                for (int p = 0; p < DEF_N; p++) {
                    res[0][p] -= privksk[u][i][j][aij - 1][0][p];
                    res[1][p] -= privksk[u][i][j][aij - 1][1][p];
                }
            }
        }
    }
}

template <typename T = uint32_t, uint32_t N = DEF_N>
static inline void PolynomialMulByXaiSub(array<T, N> &res, const array<T, N> &poly,
                               const T a)
{
    for (int i = 0; i < a; i++) res[i] -= -poly[i - a + N];
    for (int i = a; i < N; i++) res[i] -= poly[i - a];
}

void PackingKeySwitchlvl01(TRLWElvl1 &res, const array<TLWElvl0,DEF_N> &tlwearray, const PackingKeySwitchKey &packksk){
    constexpr uint32_t prec_offset = 1U << (32 - (1 + DEF_basebitlvl01 * DEF_that)); 
    constexpr uint32_t mask = (1U << DEF_basebitlvl01) - 1;

    res = {};
    for(int32_t i = 0; i < DEF_N; i++) res[1][i] = tlwearray[i][DEF_n];

    for(int h = 0; h < DEF_N; h++){
        const TLWElvl0 &tlwe = tlwearray[h];
        for(int i = 0; i < DEF_n; i++){
            const uint32_t aibar = tlwe[i] + prec_offset;
            for(int j = 0; j<DEF_that; j++){
                const uint32_t aij = (aibar >> (32 - (j + 1) * DEF_basebitlvl01)) & mask;
                if (aij != 0)
                    {
                        PolynomialMulByXaiSub<uint32_t, DEF_N>(res[0], packksk[i][j][aij-1][0],h);
                        PolynomialMulByXaiSub<uint32_t, DEF_N>(res[1], packksk[i][j][aij-1][1],h);
                    }
            }
        }
    }
}

}  // namespace TFHEpp
