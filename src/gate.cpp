#include <math.h>

#include <gatebootstrapping.hpp>
#include <keyswitch.hpp>
#include <trlwe.hpp>

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

void adjust_bp(TLWE<lvl0param> &c, Encoder &encoder, int bp_diff)
{
    for (int i = 0; i <= lvl0param::n; i++) {
        c[i] = c[i] << bp_diff;
    }
    encoder.update(encoder.a, encoder.b, encoder.bp + bp_diff);
}

void HomADD(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2)
{
    assert(encoder1.a == encoder2.a);
    assert(encoder1.b == encoder2.b);
    assert(encoder1.bp == encoder2.bp);
    encoder1.update(encoder1.a + encoder2.a, encoder1.b + encoder2.b,
                    encoder1.bp + 1);
    for (int i = 0; i <= lvl0param::n; i++) res[i] = ca[i] + cb[i];
}

void HomADDFixedEncoder(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                        const TLWE<lvl0param> &cb, Encoder &encoder1,
                        Encoder &encoder2)
{
    assert(encoder1.a == encoder2.a);
    assert(encoder1.b == encoder2.b);
    assert(encoder1.bp == encoder2.bp);
    for (int i = 0; i <= lvl0param::n; i++) res[i] = ca[i] + cb[i];
    res[lvl0param::n] -= encoder1.encode(0.);
}

void HomSUB(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2)
{
    assert(encoder1.a == encoder2.a);
    assert(encoder1.b == encoder2.b);
    assert(encoder1.bp == encoder2.bp);
    encoder1.update(encoder1.a - encoder2.b, encoder1.b - encoder2.a,
                    encoder1.bp + 1);
    for (int i = 0; i <= lvl0param::n; i++)
        res[i] = ca[i] - cb[i] + encoder1.dtotx(0.5);
}

void HomSUBFixedEncoder(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                        const TLWE<lvl0param> &cb, Encoder &encoder1,
                        Encoder &encoder2)
{
    assert(encoder1.a == encoder2.a);
    assert(encoder1.b == encoder2.b);
    assert(encoder1.bp == encoder2.bp);
    for (int i = 0; i <= lvl0param::n; i++) res[i] = ca[i] - cb[i];
    res[lvl0param::n] += encoder1.encode(0.);
}

void HomMAX(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2,
            Encoder &encoder_bs, GateKey &gk)
{
    assert(encoder1.a == encoder2.a);
    assert(encoder1.b == encoder2.b);
    assert(encoder1.bp == encoder2.bp);
    TLWE<lvl0param> test_x_minus_y, test_bs;
    TFHEpp::HomSUBFixedEncoder(test_x_minus_y, ca, cb, encoder1, encoder2);

    ReLUFunction<lvl1param> relu_function = ReLUFunction<lvl1param>();
    TFHEpp::ProgrammableBootstrapping(test_bs, test_x_minus_y, gk, encoder1,
                                      encoder_bs, relu_function);
    TFHEpp::HomADDFixedEncoder(res, test_bs, cb, encoder_bs, encoder2);
}

void HomADDCONST(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                 const double &b, Encoder &encoder)
{
    for (int i = 0; i < lvl0param::n; i++) res[i] = ca[i];
    if (b > 0) {
        lvl0param::T tmp = encoder.encode(b + encoder.a);
        res[lvl0param::n] = ca[lvl0param::n] + tmp;
    }
    else {
        lvl0param::T tmp = encoder.encode(-b + encoder.a);
        res[lvl0param::n] = ca[lvl0param::n] - tmp;
    }
}

/**
 * ((a + b) ** 2 + (a - b) ** 2 )/ 4
 * = (a ** 2 + 2ab + b ** 2 - a ** 2 + 2ab - b ** 2)/4
 * = (a ** 2 - a ** 2 + b ** 2 - b ** 2 + 2ab + 2ab) / 4
 * = 4ab / 4
 * = ab
 */
void HomMULT(TLWE<lvl0param> &res, const TLWE<lvl0param> &c1,
             const TLWE<lvl0param> &c0, const TFHEpp::GateKey &gk,
             Encoder &encoder_domain1, Encoder &encoder_domain2,
             Encoder &encoder_target)
{
    auto programable_boostrap_function =
        SquareDividedByFourFunction<lvl1param>();

    // sum = c0 + c1
    TLWE<lvl0param> sum;
    HomADDFixedEncoder(sum, c0, c1, encoder_domain1, encoder_domain2);

    // diff = c0 - c1
    TLWE<lvl0param> diff;
    HomSUBFixedEncoder(diff, c0, c1, encoder_domain1, encoder_domain2);

    // (sum ** 2 )/ 4 = ((c0 + c1) ** 2) / 4
    TLWE<lvl0param> sum_squared_and_divided_by_four;
    ProgrammableBootstrapping(sum_squared_and_divided_by_four, sum, gk,
                              encoder_domain2, encoder_target,
                              programable_boostrap_function);

    // (diff ** 2 )/ 4 = ((c0 - c1) ** 2) / 4
    TLWE<lvl0param> diff_squared_and_divided_by_four;
    ProgrammableBootstrapping(diff_squared_and_divided_by_four, diff, gk,
                              encoder_domain2, encoder_target,
                              programable_boostrap_function);

    // sum_squared - diff_squared
    // = ((c0 + c1) ** 2) / 4 - ((c0 - c1) ** 2) / 4
    // = ab
    HomSUBFixedEncoder(res, sum_squared_and_divided_by_four,
                       diff_squared_and_divided_by_four, encoder_target,
                       encoder_target);
}

void HomMULTCONSTINT(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                     const int &b, Encoder &encoder)
{
    lvl0param::T prev_0 = encoder.encode(0.);

    for (int i = 0; i <= lvl0param::n; i++) {
        lvl0param::T ca_minus_0;
        if (i == lvl0param::n) {
            ca_minus_0 = ca[i] - prev_0;
        }
        else {
            ca_minus_0 = ca[i];
        }
        res[i] = ca_minus_0 * b;
    }
    lvl0param::T after_0 = encoder.encode(0.);
    res[lvl0param::n] += after_0;
}

int find_index(double x, vector<double> y)
{
    double dist = 100000.;
    int res = 0;
    for (int i = 0; i < y.size(); i++) {
        double tmp_dist = abs(x - y[i]);
        if (tmp_dist < dist) {
            res = i;
            dist = tmp_dist;
        }
    }
    return res;
}

void HomMULTCONSTREAL(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                      const double &b, Encoder &encoder)
{

    lvl0param::T prev_0 = encoder.encode(0.);
    double b_abs_decimal = abs(b);

    //lvl0param::T index = Encoder::dtotx(b_abs_decimal, mult_max, mult_bp);
    lvl0param::T index = encoder.encode(b_abs_decimal);

    for (int i = 0; i <= lvl0param::n; i++) {
        lvl0param::T ca_minus_0;
        if (i == lvl0param::n) {
            ca_minus_0 = ca[i] - prev_0;
        }
        else {
            ca_minus_0 = ca[i];
        }
        res[i] = ca_minus_0 * index;
    }
    // encoder.update(encoder.a * mult_max, encoder.b * mult_max,
    //                encoder.bp + mult_bp);

    lvl0param::T after_0 = encoder.encode(0.);
    res[lvl0param::n] += after_0;

    if (b >= 0) {
    }
    else {
        HomMULTCONSTINT(res, res, -1, encoder);
    }
}

void HomMULTCONSTREAL(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                      const double &b, Encoder &encoder, int mult_bp,
                      double mult_max)
{
    assert(b <= mult_max);
    assert(b >= (-1) * mult_max);

    lvl0param::T prev_0 = encoder.encode(0.);
    double b_abs_decimal = abs(b);

    lvl0param::T index = Encoder::dtotx(b_abs_decimal, mult_max, mult_bp);

    for (int i = 0; i <= lvl0param::n; i++) {
        lvl0param::T ca_minus_0;
        if (i == lvl0param::n) {
            ca_minus_0 = ca[i] - prev_0;
        }
        else {
            ca_minus_0 = ca[i];
        }
        res[i] = ca_minus_0 * index;
    }
    encoder.update(encoder.a * mult_max, encoder.b * mult_max,
                   encoder.bp + mult_bp);
    lvl0param::T after_0 = encoder.encode(0.);
    res[lvl0param::n] += after_0;

    if (b >= 0) {
    }
    else {
        HomMULTCONSTINT(res, res, -1, encoder);
    }
}

void HomMULTCONST01(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
                    const double &b, Encoder &encoder, int mult_bp)
{
    assert(b <= 1.);
    assert(b >= -1.);
    lvl0param::T prev_0 = encoder.encode(0.);
    double b_abs_decimal = abs(b);

    vector<double> test2;
    for (int i = 0; i < (1U << mult_bp); i++) {
        test2.push_back(i * 1.0 / pow(2., mult_bp));
    }

    int index = find_index(b_abs_decimal, test2);

    for (int i = 0; i <= lvl0param::n; i++) {
        lvl0param::T ca_minus_0;
        if (i == lvl0param::n) {
            ca_minus_0 = ca[i] - prev_0;
        }
        else {
            ca_minus_0 = ca[i];
        }
        res[i] = ca_minus_0 * index;
    }
    encoder.update(encoder.a * 1, encoder.b * 1, encoder.bp + mult_bp);
    lvl0param::T after_0 = encoder.encode(0.);
    res[lvl0param::n] += after_0;

    if (b >= 0) {
    }
    else {
        HomMULTCONSTINT(res, res, -1, encoder);
    }
}

void HomADD(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca,
            const TRLWE<lvl1param> &cb)
{
    for (int i = 0; i < 2; i++)
        for (int j = 0; j <= lvl1param::n; j++) res[i][j] = ca[i][j] + cb[i][j];
}

void HomADDCONST(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca,
                 const array<double, lvl1param::n> &b, Encoder &encoder)
{
    for (int i = 0; i < lvl1param::n; i++) res[0][i] = ca[0][i];
    for (int i = 0; i < lvl1param::n; i++)
        res[1][i] = ca[1][i] + encoder.dtotx(encoder.encode_0_1(b[i]));
}

void HomSUB(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca,
            const TRLWE<lvl1param> &cb)
{
    for (int i = 0; i < 2; i++)
        for (int j = 0; j <= lvl1param::n; j++) res[i][j] = ca[i][j] - cb[i][j];
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

struct lvl1MulParam {
    using T = u_int32_t;
    static constexpr std::uint32_t n = lvl1param::n;
};

void HomMULTCONST(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &crypt,
                  const array<double, lvl1param::n> &array,
                  const TRLWE<lvl1param> &zeroCrypt, const Encoder &encoder)

{
    // double one = encoder.encode(1);
    double zero = encoder.encode(0);

    std::array<lvl1MulParam::T, lvl1MulParam::n> encoded;
    TRLWE<lvl1MulParam> resl, cryptl;
    TRLWE<lvl1param> tmp, tmp1;

    for (int i = 0; i < lvl1MulParam::n; i++) {
        encoded[i] = array[i];
    }

    for (int i = 0; i < lvl1MulParam::n; i++) {
        cryptl[0][i] = crypt[0][i] - zero;
        cryptl[1][i] = crypt[1][i] - zero;
    }

    Polynomial<lvl1MulParam> poly = encoded;

    PolyMulForMul<lvl1MulParam>(resl[0], cryptl[0], poly, zeroCrypt[0]);
    PolyMulForMul<lvl1MulParam>(resl[1], cryptl[1], poly, zeroCrypt[1]);

    for (int i = 0; i < lvl1MulParam::n; i++) {
        res[0][i] = resl[0][i];
        res[1][i] = resl[1][i];
    }
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
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(
        and1, temp, gk.bkfftlvl01, mupolygen<lvl1param, lvl1param::mu>());
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(
        and0, res, gk.bkfftlvl01, mupolygen<lvl1param, lvl1param::mu>());

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
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(
        and1, temp, gk.bkfftlvl01, mupolygen<lvl1param, lvl1param::mu>());
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(
        and0, res, gk.bkfftlvl01, mupolygen<lvl1param, lvl1param::mu>());

    for (int i = 0; i <= lvl1param::n; i++) and1[i] = -and1[i] - and0[i];
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
    BlindRotate<P>(res, temp1, bkfft,
                   mupolygen<typename P::targetP, P::targetP::mu>());
    BlindRotate<P>(and0, temp0, bkfft,
                   mupolygen<typename P::targetP, P::targetP::mu>());

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
TFHEPP_EXPLICIT_INSTANTIATION_BLIND_ROTATE(INST)
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
    BlindRotate<lvl01param>(res, c1, gk.bkfftlvl01,
                            mupolygen<lvl1param, lvl1param::mu>());
    BlindRotate<lvl01param>(and0, c0, gk.bkfftlvl01,
                            mupolygen<lvl1param, lvl1param::mu>());

    for (int i = 0; i < lvl1param::n; i++) {
        res[0][i] += and0[0][i];
        res[1][i] += and0[1][i];
    };
    res[1][0] += lvl1param::n;
}

}  // namespace TFHEpp