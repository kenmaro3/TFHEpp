#include <cloudkey.hpp>
#include <detwfa.hpp>
#include <gatebootstrapping.hpp>
#include <keyswitch.hpp>
#include <mulfft.hpp>
#include <params.hpp>
#include <trgsw.hpp>
#include <trlwe.hpp>
#include <utils.hpp>

namespace TFHEpp {
using namespace std;

template <class P>
inline void RotatedTestVector(array<array<typename P::T, P::n>, 2> &testvector,
                              const uint32_t bara, const typename P::T mu)
{
    testvector[0] = {};
    if (bara < P::n) {
        for (int i = 0; i < bara; i++) testvector[1][i] = -mu;
        for (int i = bara; i < P::n; i++) testvector[1][i] = mu;
    }
    else {
        const typename P::T baraa = bara - P::n;
        for (int i = 0; i < baraa; i++) testvector[1][i] = mu;
        for (int i = baraa; i < P::n; i++) testvector[1][i] = -mu;
    }
}

template <class P>
inline void MyCustomTestVector(array<array<typename P::T, P::n>, 2> &testvector,
                              const uint32_t bara, const typename P::T mu)
{
    testvector[0] = {};
    if (bara < P::n) {
        for (int i = 0; i < bara; i++) testvector[1][i] = dtot32(0.5);
        for (int i = bara; i < P::n; i++) testvector[1][i] = dtot32(0.5);
    }
    else {
        const typename P::T baraa = bara - P::n;
        for (int i = 0; i < baraa; i++) testvector[1][i] = dtot32(0.5);
        for (int i = baraa; i < P::n; i++) testvector[1][i] = dtot32(0.5);
    }
}

template <class P>
inline void CreateCustomTestVector(array<array<typename P::T, P::n>, 2> &testvector,
                              const uint32_t bara, Encoder &encoder)
{
    Encoder test_encoder(encoder.a, encoder.b, encoder.bp);
    testvector[0] = {};
    for(int i=0; i<P::n; i++){
        double tmp = test_encoder.a + test_encoder.d/2.*double(i)/double(P::n);
        testvector[1][i] = test_encoder.encode(tmp);
    }

}

template <class P>
void ProgrammableBootstrappingTLWE2TRLWEFFTWITHKEY(TRLWE<typename P::targetP> &acc,
                                    const TLWE<typename P::domainP> &tlwe,
                                    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target, Key<lvl0param> sk)
{
    uint32_t bara_tmp = modSwitchFromTorusSpecific<typename P::targetP>(tlwe[P::domainP::n], encoder_domain.bp, encoder_target.bp);
    //uint32_t bara_tmp = modSwitchFromTorus<typename P::targetP>(tlwe[P::domainP::n]);
    uint32_t bara = 2 * P::targetP::n - bara_tmp;
    //uint32_t bara = modSwitchFromTorus<typename P::targetP>(tlwe[P::domainP::n]);

    //printf("tmp1: %d\n", P::domainP::n);
    //printf("tmp2: %llu\n", tlwe[P::domainP::n]);
    //printf("mybara_tmp: %llu\n", bara_tmp);
    //printf("mybara0   : %llu\n", bara);
    CreateCustomTestVector<typename P::targetP>(acc, bara, encoder_domain);
    TRLWE<typename P::targetP> temp;
    if(bara!=0){
        PolynomialMulByXai<typename P::targetP>(temp[0], acc[0], bara);
        PolynomialMulByXai<typename P::targetP>(temp[1], acc[1], bara);
        //printf("\n==================\n");
        //printf("baraforb: %d\n", bara);
        //printf("acc[0]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", acc[0][i]);
        //}
        //printf("\n");
        //printf("temp[0]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", temp[0][i]);
        //}
        //printf("\n");
        //printf("acc[1]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", acc[1][i]);
        //}
        //printf("\n");
        //printf("temp[1]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", temp[1][i]);
        //}
        //printf("\n");
        acc[0] = temp[0];
        acc[1] = temp[1];
    }

    //MyCustomTestVector<typename P::targetP>(acc, bara, P::targetP::mu);
    for (int i = 0; i < P::domainP::n; i++) {
        //bara = modSwitchFromTorus<typename P::targetP>(tlwe[i]);
        bara = modSwitchFromTorusSpecific<typename P::targetP>(tlwe[i], encoder_domain.bp, encoder_target.bp);
        //printf("==============================\n");
        //printf("\nbara_tlwe_%d: %llu\n", i, bara);
        if (bara == 0) continue;
        // Do not use CMUXFFT to avoid unnecessary copy.
        //CMUXFFTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
        CMUXFFTwithPolynomialMulByXaiWITHKEY<typename P::targetP>(
            acc, bkfft[i], bara, sk, i);
    }
}
//#define INST(P)                                      \
//    template void ProgrammableBootstrappingTLWE2TRLWEFFTWITHKEY<P>( \
//        TRLWE<typename P::targetP> & acc,            \
//        const TLWE<typename P::domainP> &tlwe,       \
//        const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder, Key<lvl0param> sk)
//TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
//#undef INST

template <class P>
void ProgrammableBootstrappingTLWE2TRLWEFFT(TRLWE<typename P::targetP> &acc,
                                    const TLWE<typename P::domainP> &tlwe,
                                    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target)
{
    //uint32_t bara = 2 * P::targetP::n - modSwitchFromTorus<typename P::targetP>(tlwe[P::domainP::n]);
    uint32_t bara = 2 * P::targetP::n - modSwitchFromTorusSpecific<typename P::targetP>(tlwe[P::domainP::n], encoder_domain.bp, encoder_target.bp);
    //uint32_t bara = modSwitchFromTorus<typename P::targetP>(tlwe[P::domainP::n]);

    //printf("tmp1: %d\n", P::domainP::n);
    //printf("tmp2: %llu\n", tlwe[P::domainP::n]);
    //printf("mybara0: %llu\n", bara);
    CreateCustomTestVector<typename P::targetP>(acc, bara, encoder_domain);
    TRLWE<typename P::targetP> temp;
    if(bara!=0){
        PolynomialMulByXai<typename P::targetP>(temp[0], acc[0], bara);
        PolynomialMulByXai<typename P::targetP>(temp[1], acc[1], bara);
        //printf("\n==================\n");
        //printf("baraforb: %d\n", bara);
        //printf("acc[0]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", acc[0][i]);
        //}
        //printf("\n");
        //printf("temp[0]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", temp[0][i]);
        //}
        //printf("\n");
        //printf("acc[1]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", acc[1][i]);
        //}
        //printf("\n");
        //printf("temp[1]: ");
        //for(int i=0; i<P::targetP::n; i++){
        //    printf("%d, ", temp[1][i]);
        //}
        //printf("\n");

        for(int i=0; i<P::targetP::n; i++){
            acc[0][i] = temp[0][i];
            acc[1][i] = temp[1][i];
        }
    }

    //MyCustomTestVector<typename P::targetP>(acc, bara, P::targetP::mu);
    for (int i = 0; i < P::domainP::n; i++) {
        bara = modSwitchFromTorusSpecific<typename P::targetP>(tlwe[i], encoder_domain.bp, encoder_target.bp);
        //printf("==============================\n");
        //printf("\nbara_tlwe_%d: %llu\n", i, bara);
        if (bara == 0) continue;
        // Do not use CMUXFFT to avoid unnecessary copy.
        //CMUXFFTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
        CMUXFFTwithPolynomialMulByXai<typename P::targetP>(
            acc, bkfft[i], bara);
    }
}
#define INST(P)                                      \
    template void ProgrammableBootstrappingTLWE2TRLWEFFT<P>( \
        TRLWE<typename P::targetP> & acc,            \
        const TLWE<typename P::domainP> &tlwe,       \
        const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target)
TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
#undef INST

template <class P>
void GateBootstrappingTLWE2TRLWEFFT(TRLWE<typename P::targetP> &acc,
                                    const TLWE<typename P::domainP> &tlwe,
                                    const BootstrappingKeyFFT<P> &bkfft)
{
    uint32_t bara = 2 * P::targetP::n - modSwitchFromTorus<typename P::targetP>(
                                            tlwe[P::domainP::n]);
    RotatedTestVector<typename P::targetP>(acc, bara, P::targetP::mu);
    for (int i = 0; i < P::domainP::n; i++) {
        bara = modSwitchFromTorus<typename P::targetP>(tlwe[i]);
        if (bara == 0) continue;
        // Do not use CMUXFFT to avoid unnecessary copy.
        CMUXFFTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
            acc, bkfft[i], bara);
    }
}
#define INST(P)                                      \
    template void GateBootstrappingTLWE2TRLWEFFT<P>( \
        TRLWE<typename P::targetP> & acc,            \
        const TLWE<typename P::domainP> &tlwe,       \
        const BootstrappingKeyFFT<P> &bkfft)
TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
#undef INST


template <class P>
void ProgrammableBootstrappingTLWE2TLWEFFTDEBUGWITHKEY(TRLWE<typename P::targetP> &res,
                                   const TLWE<typename P::domainP> &tlwe,
                                   const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target, Key<lvl0param> sk)
{
    //TRLWE<typename P::targetP> res;
    ProgrammableBootstrappingTLWE2TRLWEFFTWITHKEY<P>(res, tlwe, bkfft, encoder_domain, encoder_target, sk);
    //SampleExtractIndex<typename P::targetP>(res, acc, 0);
}
//#define INST(P)                                     \
//    template void ProgrammableBootstrappingTLWE2TLWEFFTDEBUGWITHKEY<P>( \
//        TRLWE<typename P::targetP> & res,            \
//        const TLWE<typename P::domainP> &tlwe,      \
//        const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder, Key<lvl0param> sk)
//TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
//#undef INST

//template <class P>
//void ProgrammableBootstrappingTLWE2TLWEFFTDEBUG(TRLWE<typename P::targetP> &res,
//                                   const TLWE<typename P::domainP> &tlwe,
//                                   const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target)
//{
//    //TRLWE<typename P::targetP> res;
//    ProgrammableBootstrappingTLWE2TRLWEFFT<P>(res, tlwe, bkfft, encoder_domain, encoder_target);
//    //SampleExtractIndex<typename P::targetP>(res, acc, 0);
//}
//#define INST(P)                                     \
//    template void ProgrammableBootstrappingTLWE2TLWEFFTDEBUG<P>( \
//        TRLWE<typename P::targetP> & res,            \
//        const TLWE<typename P::domainP> &tlwe,      \
//        const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target)
//TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
//#undef INST

template <class P>
void ProgrammableBootstrappingTLWE2TLWEFFT(TLWE<typename P::targetP> &res,
                                   const TLWE<typename P::domainP> &tlwe,
                                   const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder encoder_target)
{
    TRLWE<typename P::targetP> acc;
    ProgrammableBootstrappingTLWE2TRLWEFFT<P>(acc, tlwe, bkfft, encoder_domain, encoder_target);
    SampleExtractIndex<typename P::targetP>(res, acc, 0);
}
#define INST(P)                                     \
    template void ProgrammableBootstrappingTLWE2TLWEFFT<P>( \
        TLWE<typename P::targetP> & res,            \
        const TLWE<typename P::domainP> &tlwe,      \
        const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain, Encoder &encoder_target)
TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
#undef INST

template <class P>
void GateBootstrappingTLWE2TLWEFFT(TLWE<typename P::targetP> &res,
                                   const TLWE<typename P::domainP> &tlwe,
                                   const BootstrappingKeyFFT<P> &bkfft)
{
    TRLWE<typename P::targetP> acc;
    GateBootstrappingTLWE2TRLWEFFT<P>(acc, tlwe, bkfft);
    SampleExtractIndex<typename P::targetP>(res, acc, 0);
}
#define INST(P)                                     \
    template void GateBootstrappingTLWE2TLWEFFT<P>( \
        TLWE<typename P::targetP> & res,            \
        const TLWE<typename P::domainP> &tlwe,      \
        const BootstrappingKeyFFT<P> &bkfft)
TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
#undef INST

template <class P>
void GateBootstrappingTLWE2TLWEFFTvariableMu(
    TLWE<typename P::targetP> &res, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, const typename P::targetP::T mus2)
{
    TRLWE<typename P::targetP> acc, temp;
    uint32_t bara = 2 * P::targetP::n - modSwitchFromTorus<typename P::targetP>(
                                            tlwe[P::domainP::n]);
    RotatedTestVector<typename P::targetP>(acc, bara, mus2);
    for (int i = 0; i < P::domainP::n; i++) {
        bara = modSwitchFromTorus<typename P::targetP>(tlwe[i]);
        if (bara == 0) continue;
        // Do not use CMUXFFT to avoid unnecessary copy.
        CMUXFFTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
            acc, bkfft[i], bara);
    }
    SampleExtractIndex<typename P::targetP>(res, acc, 0);
    res[P::targetP::n] += mus2;
}
#define INST(P)                                               \
    template void GateBootstrappingTLWE2TLWEFFTvariableMu<P>( \
        TLWE<typename P::targetP> & res,                      \
        const TLWE<typename P::domainP> &tlwe,                \
        const BootstrappingKeyFFT<P> &bkfft, const typename P::targetP::T mus2)
TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
#undef INST

void ProgrammableBootstrapping(TLWE<lvl0param> &res, const TLWE<lvl0param> &tlwe,
                       const GateKey &gk, Encoder &encoder_domain, Encoder &encoder_target)
{
    TLWE<lvl1param> tlwelvl1;
    ProgrammableBootstrappingTLWE2TLWEFFT<lvl01param>(tlwelvl1, tlwe, gk.bkfftlvl01, encoder_domain, encoder_target);
    IdentityKeySwitch<lvl10param>(res, tlwelvl1, gk.ksk);
}

void ProgrammableBootstrappingWithoutKS(TLWE<lvl1param> &res, const TLWE<lvl0param> &tlwe,
                       const GateKey &gk, Encoder &encoder)
{
    ProgrammableBootstrappingTLWE2TLWEFFT<lvl01param>(res, tlwe, gk.bkfftlvl01, encoder);
}

//void ProgrammableBootstrappingWithoutSE(TRLWE<lvl1param> &res, const TLWE<lvl0param> &tlwe,
                       //const GateKey &gk, Encoder &encoder_domain, Encoder &encoder_target)
//{
    //ProgrammableBootstrappingTLWE2TLWEFFTDEBUG<lvl01param>(res, tlwe, gk.bkfftlvl01, encoder_domain, encoder_target);
//}

void ProgrammableBootstrappingWithoutSEWITHKEY(TRLWE<lvl1param> &res, const TLWE<lvl0param> &tlwe,
                       const GateKey &gk, Encoder &encoder_domain, Encoder &encoder_target, Key<lvl0param> sk)
{
    ProgrammableBootstrappingTLWE2TLWEFFTDEBUGWITHKEY<lvl01param>(res, tlwe, gk.bkfftlvl01, encoder_domain, encoder_target, sk);
}


void GateBootstrapping(TLWE<lvl0param> &res, const TLWE<lvl0param> &tlwe,
                       const GateKey &gk)
{
    TLWE<lvl1param> tlwelvl1;
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(tlwelvl1, tlwe, gk.bkfftlvl01);
    IdentityKeySwitch<lvl10param>(res, tlwelvl1, gk.ksk);
}
}  // namespace TFHEpp