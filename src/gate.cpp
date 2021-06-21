#include <gatebootstrapping.hpp>
#include <keyswitch.hpp>
#include <math.h>


namespace TFHEpp {

// No input
void HomCONSTANTONE(TLWE<lvl0param> &res)
{
    res = {};
    res[lvl0param::n] = lvl0param::mu;
}

void HomCONSTANTZERO(TLWE<lvl0param> &res)
{
    res = {};
    res[lvl0param::n] = -lvl0param::mu;
}

// 1 input
void HomNOT(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca)
{
    for (int i = 0; i <= lvl0param::n; i++) res[i] = -ca[i];
}

void HomCOPY(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca)
{
    for (int i = 0; i <= lvl0param::n; i++) res[i] = ca[i];
}


void HomADD(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2)
{
    encoder1.a += encoder2.a;
    encoder1.b += encoder2.b;
    encoder1.d = encoder1.b - encoder1.a;
    encoder1.half = (encoder1.b + encoder1.a)/2.;
    encoder1.half_d = encoder1.d/2.;
    encoder1.bp += 1;
    for (int i = 0; i <= lvl0param::n; i++) res[i] = ca[i] + cb[i];
}

void HomSUB(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2)
{
    encoder1.a -= encoder2.b;
    encoder1.b -= encoder2.a;
    encoder1.d = encoder1.b - encoder1.a;
    encoder1.half = (encoder1.b + encoder1.a)/2.;
    encoder1.half_d = encoder1.d/2.;
    encoder1.bp += 1;


    for (int i = 0; i <= lvl0param::n; i++) res[i] = ca[i] - cb[i] + encoder1.dtotx(0.5);
}

void HomADDCONST(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder)
{
    //encoder.a += encoder.a;
    //encoder.b += encoder.b;
    //encoder.d = encoder.b - encoder.a;
    //encoder.half = (encoder.b + encoder.a)/2.;
    //encoder.half_d = encoder.d/2.;
    //encoder.bp += 1;
    for (int i = 0; i < lvl0param::n; i++) res[i] = ca[i];
    //res[lvl0param::n] = ca[lvl0param::n] + encoder.dtotx(encoder.encode_0_1(b));
    if(b>0){
        uint32_t tmp = encoder.encode(b + encoder.a);
        res[lvl0param::n] = ca[lvl0param::n] + tmp;
    }else{
        uint32_t tmp = encoder.encode(-b + encoder.a);
        res[lvl0param::n] = ca[lvl0param::n] - tmp;

    }
    printf("here: %llu\n", encoder.encode(b));
}

void HomMULTCONSTINT(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const int &b)
{


    if(b >=0){
        for (int i = 0; i <= lvl0param::n; i++){

            res[i] = ca[i] * b;
        }

    }else{
        double tmp_b = abs(b);
        for (int i = 0; i <= lvl0param::n; i++){

            double b_decimal = tmp_b - int(tmp_b);
            int inv_b = int(1./b_decimal);

            lvl0param::T tmp = inv_b == 0 ? 0 : ca[i]/inv_b;
            lvl0param::T tmp1 = ca[i] * int(tmp_b) + tmp;
            res[i] = -tmp1;
        }

    }
}

void HomMULTCONSTREAL(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder, Encoder &encoder2)
{

    //uint32_t mid_old = encoder.encode(encoder.half);
    int max_num = encoder2.b;
    int precision = encoder2.bp;

    encoder.a = encoder.a * max_num;
    encoder.b = encoder.b * max_num;
    encoder.d = encoder.b-encoder.a;
    encoder.half_d = (encoder.b-encoder.a)/2.;
    encoder.half = (encoder.b+encoder.a)/2.;
    encoder.bp = encoder.bp + encoder2.bp;


    if(b >=0){
        for (int i = 0; i <= lvl0param::n; i++){

            //double b_0_1 = b / encoder2.b;
            double b_0_1 = encoder2.encode_0_1(b);
            //double b_0_1 = b / (encoder2.b - encoder2.a) + 0.5;
            //printf("\nb_0_1:: %f\n", b_0_1);
            //uint32_t ca_minus_mid = ca[i] - mid_old;
            //res[i] = ca_minus_mid * encoder2.dtotx(tmp);
            //res[i] += mid_new;
            res[i] = ca[i] * encoder2.dtotx(b_0_1);
            //printf("\n%llu\n", ca[i]);
            //printf("\n%llu\n", encoder2.dtotx(tmp));
            //printf("%llu\n", res[i]);
            
        }

    }else{
        double tmp_b = abs(b);
        for (int i = 0; i <= lvl0param::n; i++){

            double b_decimal = tmp_b - int(tmp_b);
            int inv_b = int(1./b_decimal);

            lvl0param::T tmp = inv_b == 0 ? 0 : ca[i]/inv_b;
            lvl0param::T tmp1 = ca[i] * int(tmp_b) + tmp;
            res[i] = -tmp1;
        }

    }
}

void HomADD(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca, const TRLWE<lvl1param> &cb)
{
    for (int i = 0; i <2; i++) 
        for(int j=0; j<= lvl1param::n; j++)
            res[i][j] = ca[i][j] + cb[i][j];
}

void HomADDCONST(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca, const array<double, lvl1param::n> &b, Encoder &encoder)
{
    for (int i = 0; i < lvl1param::n; i++) res[0][i] = ca[0][i];
    for (int i = 0; i < lvl1param::n; i++) res[1][i] = ca[1][i] + encoder.dtotx(encoder.encode_0_1(b[i]));
}


void HomSUB(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca, const TRLWE<lvl1param> &cb)
{
    for (int i = 0; i <2; i++) 
        for(int j=0; j<= lvl1param::n; j++)
            res[i][j] = ca[i][j] - cb[i][j];
}


template <int casign, int cbsign, typename lvl0param::T offset>
inline void HomGate(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                    const TLWE<lvl0param> &cb, const GateKey &gk)
{
    for (int i = 0; i <= lvl0param::n; i++)
        res[i] = casign * ca[i] + cbsign * cb[i];
    res[lvl0param::n] += offset;
    GateBootstrapping(res, res, gk);
}

void HomNAND(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<-1, -1, lvl0param::mu>(res, ca, cb, gk);
}

void HomNOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<-1, -1, -lvl0param::mu>(res, ca, cb, gk);
}

void HomXNOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<-2, -2, -2 * lvl0param::mu>(res, ca, cb, gk);
}

void HomAND(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<1, 1, -lvl0param::mu>(res, ca, cb, gk);
}

void HomOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
           const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<1, 1, lvl0param::mu>(res, ca, cb, gk);
}

void HomXOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<2, 2, 2 * lvl0param::mu>(res, ca, cb, gk);
}

void HomANDNY(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
              const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<-1, 1, -lvl0param::mu>(res, ca, cb, gk);
}

void HomANDYN(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
              const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<1, -1, -lvl0param::mu>(res, ca, cb, gk);
}

void HomORNY(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<-1, 1, lvl0param::mu>(res, ca, cb, gk);
}

void HomORYN(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk)
{
    HomGate<1, -1, lvl0param::mu>(res, ca, cb, gk);
}

// 3input
// cs?c1:c0
void HomMUX(TLWE<lvl0param> &res, const TLWE<lvl0param> &cs,
            const TLWE<lvl0param> &c1, const TLWE<lvl0param> &c0,
            const GateKey &gk)
{
    TLWE<lvl0param> temp;
    for (int i = 0; i <= lvl0param::n; i++) temp[i] = cs[i] + c1[i];
    for (int i = 0; i <= lvl0param::n; i++) res[i] = -cs[i] + c0[i];
    temp[lvl0param::n] -= lvl0param::mu;
    res[lvl0param::n] -= lvl0param::mu;
    TLWE<lvl1param> and1;
    TLWE<lvl1param> and0;
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(and1, temp, gk.bkfftlvl01);
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(and0, res, gk.bkfftlvl01);

    for (int i = 0; i <= lvl1param::n; i++) and1[i] += and0[i];
    and1[lvl1param::n] += lvl1param::mu;
    IdentityKeySwitch<lvl10param>(res, and1, gk.ksk);
}

void HomNMUX(TLWE<lvl0param> &res, const TLWE<lvl0param> &cs,
            const TLWE<lvl0param> &c1, const TLWE<lvl0param> &c0,
            const GateKey &gk)
{
    TLWE<lvl0param> temp;
    for (int i = 0; i <= lvl0param::n; i++) temp[i] = cs[i] + c1[i];
    for (int i = 0; i <= lvl0param::n; i++) res[i] = -cs[i] + c0[i];
    temp[lvl0param::n] -= lvl0param::mu;
    res[lvl0param::n] -= lvl0param::mu;
    TLWE<lvl1param> and1;
    TLWE<lvl1param> and0;
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(and1, temp, gk.bkfftlvl01);
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(and0, res, gk.bkfftlvl01);

    for (int i = 0; i <= lvl1param::n; i++) and1[i] = -and1[i]-and0[i];
    and1[lvl1param::n] -= lvl1param::mu;
    IdentityKeySwitch<lvl10param>(res, and1, gk.ksk);
}

template <class P>
void HomMUXwoSE(TRLWE<typename P::targetP> &res,
                const TLWE<typename P::domainP> &cs,
                const TLWE<typename P::domainP> &c1,
                const TLWE<typename P::domainP> &c0,
                const BootstrappingKeyFFT<P> &bkfft)
{
    TLWE<typename P::domainP> temp1;
    TLWE<typename P::domainP> temp0;
    for (int i = 0; i <= P::domainP::n; i++) temp1[i] = cs[i] + c1[i];
    for (int i = 0; i <= P::domainP::n; i++) temp0[i] = -cs[i] + c0[i];
    temp1[lvl0param::n] -= P::domainP::mu;
    temp0[lvl0param::n] -= P::domainP::mu;
    TRLWE<typename P::targetP> and0;
    GateBootstrappingTLWE2TRLWEFFT<P>(res, temp1, bkfft);
    GateBootstrappingTLWE2TRLWEFFT<P>(and0, temp0, bkfft);

    for (int i = 0; i < P::targetP::n; i++) {
        res[0][i] += and0[0][i];
        res[1][i] += and0[1][i];
    };
    res[1][0] += P::targetP::mu;
}
#define INST(P)                                                      \
    template void HomMUXwoSE<P>(TRLWE<typename P::targetP> & res,    \
                                const TLWE<typename P::domainP> &cs, \
                                const TLWE<typename P::domainP> &c1, \
                                const TLWE<typename P::domainP> &c0, \
                                const BootstrappingKeyFFT<P> &bkfft)
TFHEPP_EXPLICIT_INSTANTIATION_LVL01_02(INST);
#undef INST

void ExtractSwitchAndHomMUX(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &csr,
                            const TRLWE<lvl1param> &c1r,
                            const TRLWE<lvl1param> &c0r, const GateKey &gk)
{
    TLWE<lvl1param> templvl1;
    TLWE<lvl0param> cs, c1, c0;
    SampleExtractIndex<lvl1param>(templvl1, csr, 0);
    IdentityKeySwitch<lvl10param>(cs, templvl1, gk.ksk);
    SampleExtractIndex<lvl1param>(templvl1, c1r, 0);
    IdentityKeySwitch<lvl10param>(c1, templvl1, gk.ksk);
    SampleExtractIndex<lvl1param>(templvl1, c0r, 0);
    IdentityKeySwitch<lvl10param>(c0, templvl1, gk.ksk);

    for (int i = 0; i <= lvl0param::n; i++) c1[i] += cs[i];
    for (int i = 0; i <= lvl0param::n; i++) c0[i] -= cs[i];
    c1[lvl0param::n] -= lvl0param::mu;
    c0[lvl0param::n] -= lvl0param::mu;
    TRLWE<lvl1param> and0;
    GateBootstrappingTLWE2TRLWEFFT<lvl01param>(res, c1, gk.bkfftlvl01);
    GateBootstrappingTLWE2TRLWEFFT<lvl01param>(and0, c0, gk.bkfftlvl01);

    for (int i = 0; i < lvl1param::n; i++) {
        res[0][i] += and0[0][i];
        res[1][i] += and0[1][i];
    };
    res[1][0] += lvl1param::n;
}

}  // namespace TFHEpp