#include "detwfa.hpp"

namespace TFHEpp {
template <class P>
void CMUXFFT(TRLWE<P> &res, const TRGSWFFT<P> &cs, const TRLWE<P> &c1,
             const TRLWE<P> &c0)
{
    for (int i = 0; i < P::n; i++) {
        res[0][i] = c1[0][i] - c0[0][i];
        res[1][i] = c1[1][i] - c0[1][i];
    }
    trgswfftExternalProduct<P>(res, res, cs);
    for (int i = 0; i < P::n; i++) {
        res[0][i] += c0[0][i];
        res[1][i] += c0[1][i];
    }
}
#define INST(P)                                                     \
    template void CMUXFFT<P>(TRLWE<P> & res, const TRGSWFFT<P> &cs, \
                             const TRLWE<P> &c1, const TRLWE<P> &c0)
TFHEPP_EXPLICIT_INSTANTIATION_LVL1_2(INST)
#undef INST

template <class P>
void CMUXFFTwithPolynomialMulByXaiWITHKEY(TRLWE<P> &acc, const TRGSWFFT<P> &cs,
                                           const typename P::T a, Key<lvl0param> sk, int index)
{
    TRLWE<P> temp;
    PolynomialMulByXai<P>(temp[0], acc[0], a);
    PolynomialMulByXai<P>(temp[1], acc[1], a);
    //printf("a: %d\n", a);
    //printf("acc[0]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", acc[0][i]);
    //}
    //printf("\n");
    //printf("temp[0]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", temp[0][i]);
    //}
    //printf("\n");
    //printf("acc[1]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", acc[1][i]);
    //}
    //printf("\n");
    //printf("temp[1]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", temp[1][i]);
    //}
    //printf("\n");
    if(sk[index] == 1){
        for (int i = 0; i < P::n; i++) {
            //acc[0][i] += temp[0][i];
            //acc[1][i] += temp[1][i];
            acc[0][i] = temp[0][i];
            acc[1][i] = temp[1][i];
        }
        //printf("sk[%d] == 1\n", index);
    }else{
        //printf("sk[%d] == 0\n", index);

    }

    
    
    

}
#define INST(P)                                             \
    template void CMUXFFTwithPolynomialMulByXaiWITHKEY<P>( \
        TRLWE<P> & acc, const TRGSWFFT<P> &cs, const typename P::T a, Key<lvl0param> sk, int index)
TFHEPP_EXPLICIT_INSTANTIATION_LVL1_2(INST)
#undef INST


template <class P>
void CMUXFFTwithPolynomialMulByXai(TRLWE<P> &acc, const TRGSWFFT<P> &cs,
                                           const typename P::T a)
{
    TRLWE<P> temp;
    PolynomialMulByXai<P>(temp[0], acc[0], a);
    PolynomialMulByXai<P>(temp[1], acc[1], a);
    //printf("a: %d\n", a);
    //printf("acc[0]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", acc[0][i]);
    //}
    //printf("\n");
    //printf("temp[0]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", temp[0][i]);
    //}
    //printf("\n");
    //printf("acc[1]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", acc[1][i]);
    //}
    //printf("\n");
    //printf("temp[1]: ");
    //for(int i=0; i<P::n; i++){
    //    printf("%d, ", temp[1][i]);
    //}
    //printf("\n");

    trgswfftExternalProduct<P>(temp, temp, cs);
    acc[0] = temp[0];
    acc[1] = temp[1];
    //for (int i = 0; i < P::n; i++) {
    //    acc[0][i] = temp[0][i];
    //    acc[1][i] = temp[1][i];
    //}
}
#define INST(P)                                             \
    template void CMUXFFTwithPolynomialMulByXai<P>( \
        TRLWE<P> & acc, const TRGSWFFT<P> &cs, const typename P::T a)
TFHEPP_EXPLICIT_INSTANTIATION_LVL1_2(INST)
#undef INST


template <class P>
void CMUXFFTwithPolynomialMulByXaiMinusOne(TRLWE<P> &acc, const TRGSWFFT<P> &cs,
                                           const typename P::T a)
{
    TRLWE<P> temp;
    PolynomialMulByXaiMinusOne<P>(temp[0], acc[0], a);
    PolynomialMulByXaiMinusOne<P>(temp[1], acc[1], a);
    printf("a: %d\n", a);
    printf("acc[0]: ");
    for(int i=0; i<P::n; i++){
        printf("%d, ", acc[0][i]);
    }
    printf("\n");
    printf("temp[0]: ");
    for(int i=0; i<P::n; i++){
        printf("%d, ", temp[0][i]);
    }
    printf("\n");
    printf("acc[1]: ");
    for(int i=0; i<P::n; i++){
        printf("%d, ", acc[1][i]);
    }
    printf("\n");
    printf("temp[1]: ");
    for(int i=0; i<P::n; i++){
        printf("%d, ", temp[1][i]);
    }
    printf("\n");
    trgswfftExternalProduct<P>(temp, temp, cs);
    for (int i = 0; i < P::n; i++) {
        acc[0][i] += temp[0][i];
        acc[1][i] += temp[1][i];
    }
}
#define INST(P)                                             \
    template void CMUXFFTwithPolynomialMulByXaiMinusOne<P>( \
        TRLWE<P> & acc, const TRGSWFFT<P> &cs, const typename P::T a)
TFHEPP_EXPLICIT_INSTANTIATION_LVL1_2(INST)
#undef INST

}  // namespace TFHEpp
