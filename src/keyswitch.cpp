#include <cloudkey.hpp>
#include <mulfft.hpp>

namespace TFHEpp {
void IdentityKeySwitchlvl10(TLWElvl0 &res, const TLWElvl1 &tlwe,
                            const KeySwitchingKey &ksk)
{
    const uint32_t prec_offset = 1U << (32 - (1 + DEF_basebit * DEF_t));
    const uint32_t mask = (1U << DEF_basebit) - 1;
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
    const uint32_t mask = (1 << DEF_basebitlvl21) - 1;
    const uint64_t prec_offset = 1UL
                                 << (64 - (1 + DEF_basebitlvl21 * DEF_tbar));

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

void Packinglvl1(TRLWElvl1 &restrlwe, const PrepackTLWElvl1 &ptlwe, const PackingKey &pk){
    constexpr uint32_t mask = (1U << DEF_Bgpkbit) - 1;
    constexpr uint32_t halfBg = 1U << (DEF_Bgpkbit - 1);

    restrlwe = {};
    for(int i = 0;i<DEF_N;i++) restrlwe[1][i] = ptlwe[i][DEF_N];

    for(int i = 0;i<DEF_N;i++){
        //Init
        Polynomiallvl1 poly = {};
        for(int j = 0;j<DEF_N;j++) poly[j] = ptlwe[j][i];

        //Decompose
        DecomposedPolynomiallvl1 dpoly;
        for(int j = 0;j<DEF_N;j++){
            uint32_t temp = poly[j] + DEF_offsetpklvl1;
            for(int k = 0;k<DEF_lpk;k++)
                dpoly[k][j] = ((temp >> (numeric_limits<uint32_t>::digits - (k + 1) * DEF_Bgpkbit)) &
                 mask) -
                halfBg;
        }

        //IFFT
        DecomposedPolynomialInFDlvl1 dpolyfft;
        for(int j = 0;j<DEF_lpk;j++) TwistIFFTlvl1(dpolyfft[j],dpoly[j]);

        //ExternalProduct
        TRLWEInFDlvl1 trlwefft;
        MulInFD<DEF_N>(trlwefft[0],dpolyfft[0],pk.pack[i][0][0]);
        MulInFD<DEF_N>(trlwefft[1],dpolyfft[0],pk.pack[i][0][1]);
        for(int j = 1;j<DEF_lpk;j++){
            FMAInFD<DEF_N>(trlwefft[0],dpolyfft[j],pk.pack[i][j][0]);
            FMAInFD<DEF_N>(trlwefft[1],dpolyfft[j],pk.pack[i][j][1]);
        }
        TRLWElvl1 trlwe;
        TwistFFTlvl1(trlwe[0],trlwefft[0]);
        TwistFFTlvl1(trlwe[1],trlwefft[1]);

        //Subtruct
        for(int j = 0;j<DEF_N;j++){
            restrlwe[0][j] -= trlwe[0][j];
            restrlwe[1][j] -= trlwe[1][j];
        }
    }
}
}  // namespace TFHEpp
