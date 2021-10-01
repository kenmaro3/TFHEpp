#include <omp.h>
#include <stdio.h>

#include <cassert>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tfhe++.hpp>
#include <vector>

using namespace std;
using namespace TFHEpp;

#include <chrono>
using namespace std::chrono;
inline double get_time_msec(void)
{
    return static_cast<double>(duration_cast<nanoseconds>(
                                   steady_clock::now().time_since_epoch())
                                   .count()) /
           1000000;
}

void print_vec_1d(vector<double> x)
{
    for (int i = 0; i < x.size(); i++) {
        printf("%f, ", x[i]);
    }
    printf("\n");
}

void print_vec_2d(vector<vector<double>> x)
{
    for (int i = 0; i < x.size(); i++) {
        print_vec_1d(x[i]);
    }
    printf("\n");
}

double my_batch_normalization_function(double x, double gamma, double mean,
                                       double var, double beta)
{
    // from keras
    // gamma * (batch - self.moving_mean) / sqrt(self.moving_var + epsilon) +
    // beta
    return gamma * (x - mean) / std::sqrt(var + 0.0000001) + beta;
}

vector<double> give_me_vector(int size)
{
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::normal_distribution<> dist(0.0, 1.0);
    vector<double> res;
    for (std::size_t n = 0; n < size; ++n) {
        double tmp = dist(engine);
        res.push_back(tmp);
    }
    return res;
}

vector<vector<double>> give_me_weight(int dout, int din)
{
    vector<vector<double>> res;
    for (std::size_t n = 0; n < dout; ++n) {
        res.push_back(give_me_vector(din));
    }
    return res;
}

double inner(const vector<double> x, const vector<double> y,
             bool is_print = true)
{
    assert(x.size() == y.size());
    double res = 0;
    for (std::size_t i = 0; i < x.size(); i++) {
        res += x[i] * y[i];
    }
    if (is_print) {
        printf("%f\n", res);
    }
    return res;
}

template <class P>
vector<TLWE<P>> encrypt_vector(const vector<double> xs,
                               const array<typename P::T, P::n> sk,
                               Encoder &encoder)
{
    vector<TLWE<P>> res;
    for (int i = 0; i < xs.size(); i++) {
        res.push_back(
            TFHEpp::tlweSymEncodeEncrypt<P>(xs[i], P::alpha, sk, encoder));
    }

    return res;
}

template <class P>
vector<TLWE<P>> encrypt_vector_omp(const vector<double> xs,
                                   const array<typename P::T, P::n> sk,
                                   Encoder &encoder)
{
    vector<TLWE<P>> res(xs.size());
    int i;
#pragma omp parallel for private(i)
    for (i = 0; i < xs.size(); i++) {
        res[i] = TFHEpp::tlweSymEncodeEncrypt<P>(xs[i], P::alpha, sk, encoder);
    }
    return res;
}

template <class P>
vector<double> decrypt_vector(const vector<TLWE<P>> cs,
                              const array<typename P::T, P::n> sk,
                              Encoder &encoder, bool is_print = true)
{
    vector<double> res;
    for (int i = 0; i < cs.size(); i++) {
        res.push_back(TFHEpp::tlweSymDecryptDecode<P>(cs[i], sk, encoder));
    }

    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

template <class P>
vector<double> decrypt_vector_omp(const vector<TLWE<P>> cs,
                                  const array<typename P::T, P::n> sk,
                                  Encoder &encoder, bool is_print = true)
{
    vector<double> res(cs.size());

    int i;
#pragma omp parallel for private(i)
    for (i = 0; i < cs.size(); i++) {
        res[i] = TFHEpp::tlweSymDecryptDecode<P>(cs[i], sk, encoder);
    }

    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

template <class P>
double decrypt(const TLWE<P> cs, const array<typename P::T, P::n> sk,
               Encoder &encoder, bool is_print = true)
{
    double res = TFHEpp::tlweSymDecryptDecode<P>(cs, sk, encoder);
    if (is_print) {
        printf("%f\n", res);
    }
    return res;
}

double get_max(vector<double> x)
{
    double res = abs(x[0]);
    for (int i = 1; i < x.size(); i++) {
        if (abs(x[i]) > res) {
            res = abs(x[i]);
        }
    }
    return res;
}

double get_wider(double x)
{
    if (x >= 0) {
        // return ceil(x);
        return x + 0.2;
    }
    else {
        // double tmp = ceil(abs(x));
        double tmp = abs(x) + 0.2;
        return tmp * (-1.0);
    }
}
class MultFunction : public AbstructBasicFunction<TFHEpp::lvl1param> {
public:
    double y;
    MultFunction(double y) { this->y = y; }
    double run(double x) { return x * y; }
};

template <class p>
vector<TLWE<p>> multd(const vector<TLWE<p>> cs, const vector<double> ds,
                      const GateKey &gk, Encoder &encoder_domain,
                      Encoder &encoder_target, double expansion = 1.)
{
    assert(cs.size() == ds.size());
    vector<TLWE<p>> res(cs.size());

    for (int i = 0; i < cs.size(); i++) {
        MultFunction mult_function = MultFunction(ds[i]);
        TFHEpp::ProgrammableBootstrapping(res[i], cs[i], gk, encoder_domain,
                                          encoder_target, mult_function);
    }

    Encoder encoder = encoder_target;
    return res;
}

template <class p>
vector<TLWE<p>> multd(const vector<TLWE<p>> cs, const vector<double> ds,
                      const GateKey &gk, Encoder &encoder,
                      double expansion = 1.)
{
    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder::copy(encoder);

    assert(cs.size() == ds.size());
    vector<TLWE<p>> res(cs.size());
    double max_ds = get_max(ds);
    max_ds = get_wider(max_ds);
    max_ds *= expansion;

    encoder_target.update(max_ds);

    for (int i = 0; i < cs.size(); i++) {
        MultFunction mult_function = MultFunction(ds[i]);
        TFHEpp::ProgrammableBootstrapping(res[i], cs[i], gk, encoder_domain,
                                          encoder_target, mult_function);
    }

    encoder = encoder_target;
    return res;
}

template <class p>
vector<TLWE<p>> relu(const vector<TLWE<p>> cs, const GateKey &gk,
                     Encoder &encoder, double expansion = 1.)
{
    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder::copy(encoder);
    ReLUFunction relu_function = ReLUFunction<lvl1param>();

    vector<TLWE<p>> res(cs.size());

    for (int i = 0; i < cs.size(); i++) {
        TFHEpp::ProgrammableBootstrapping(res[i], cs[i], gk, encoder_domain,
                                          encoder_target, relu_function);
    }

    encoder = encoder_target;
    return res;
}

template <class p>
vector<TLWE<p>> relu_omp(const vector<TLWE<p>> cs, const GateKey &gk,
                         Encoder &encoder, double expansion = 1.)
{
    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder::copy(encoder);
    ReLUFunction relu_function = ReLUFunction<lvl1param>();

    vector<TLWE<p>> res(cs.size());

    int i;
#pragma omp parallel for private(i)
    for (i = 0; i < cs.size(); i++) {
        TFHEpp::ProgrammableBootstrapping(res[i], cs[i], gk, encoder_domain,
                                          encoder_target, relu_function);
    }

    encoder = encoder_target;
    return res;
}

vector<double> relu(const vector<double> x, bool is_print = true)
{
    vector<double> res;
    for (int i = 0; i < x.size(); i++) {
        if (x[i] >= 0) {
            res.push_back(x[i]);
        }
        else {
            res.push_back(0);
        }
    }
    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

template <class p>
vector<TLWE<p>> sigmoid(const vector<TLWE<p>> cs, const GateKey &gk,
                        Encoder &encoder, double expansion = 1.)
{
    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder(-2, 2, encoder.bp);
    SigmoidFunction sigmoid_function = SigmoidFunction<lvl1param>();

    vector<TLWE<p>> res(cs.size());

    for (int i = 0; i < cs.size(); i++) {
        TFHEpp::ProgrammableBootstrapping(res[i], cs[i], gk, encoder_domain,
                                          encoder_target, sigmoid_function);
    }

    encoder = encoder_target;
    return res;
}

template <class p>
vector<TLWE<p>> sigmoid_omp(const vector<TLWE<p>> cs, const GateKey &gk,
                            Encoder &encoder, double expansion = 1.)
{
    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder(-2, 2, encoder.bp);
    SigmoidFunction sigmoid_function = SigmoidFunction<lvl1param>();

    vector<TLWE<p>> res(cs.size());

    int i;
#pragma omp parallel for private(i)
    for (i = 0; i < cs.size(); i++) {
        TFHEpp::ProgrammableBootstrapping(res[i], cs[i], gk, encoder_domain,
                                          encoder_target, sigmoid_function);
    }

    encoder = encoder_target;
    return res;
}

vector<double> sigmoid(const vector<double> x, bool is_print = true)
{
    SigmoidFunction sigmoid_function = SigmoidFunction<lvl1param>();
    vector<double> res;
    for (int i = 0; i < x.size(); i++) {
        res.push_back(sigmoid_function.run(x[i]));
    }
    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

template <class P>
TLWE<P> inner(const vector<TLWE<P>> cs, const vector<double> ds,
              const GateKey &gk, Encoder &encoder_domain,
              Encoder &encoder_target, double expansion = 1.0)
{
    vector<TLWE<P>> tmp =
        multd<P>(cs, ds, gk, encoder_domain, encoder_target, expansion);
    TLWE<P> res = tmp[0];
    for (int i = 1; i < cs.size(); i++) {
        TFHEpp::HomADDFixedEncoder(res, res, tmp[i], encoder_target,
                                   encoder_target);
    }
    return res;
}

template <class P>
TLWE<P> inner(const vector<TLWE<P>> cs, const vector<double> ds,
              const GateKey &gk, Encoder &encoder, double expansion = 1.0)
{
    vector<TLWE<P>> tmp = multd<P>(cs, ds, gk, encoder, expansion);
    TLWE<P> res = tmp[0];
    for (int i = 1; i < cs.size(); i++) {
        TFHEpp::HomADDFixedEncoder(res, res, tmp[i], encoder, encoder);
    }

    return res;
}

template <class P>
vector<TLWE<P>> wtx(const vector<TLWE<P>> cs, const vector<vector<double>> w,
                    const GateKey &gk, Encoder &encoder, double expansion)
{
    assert(cs.size() == w[0].size());

    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder::copy(encoder);
    double max_ds = 0.;
    for (int i = 0; i < w.size(); i++) {
        max_ds = max(max_ds, get_max(w[i]));
    }
    max_ds = get_wider(max_ds);
    max_ds *= expansion;
    encoder_target.update(max_ds);

    vector<TLWE<P>> res;
    for (int i = 0; i < w.size(); i++) {
        res.push_back(inner<P>(cs, w[i], gk, encoder_domain, encoder_target));
    }

    encoder = encoder_target;

    return res;
}

template <class P>
vector<TLWE<P>> wtx_omp(const vector<TLWE<P>> cs,
                        const vector<vector<double>> w, const GateKey &gk,
                        Encoder &encoder, double expansion)
{
    assert(cs.size() == w[0].size());

    Encoder encoder_domain = Encoder::copy(encoder);
    Encoder encoder_target = Encoder::copy(encoder);
    double max_ds = 0.;
    for (int i = 0; i < w.size(); i++) {
        max_ds = max(max_ds, get_max(w[i]));
    }
    max_ds = get_wider(max_ds);
    max_ds *= expansion;
    encoder_target.update(max_ds);

    vector<TLWE<P>> res(w.size());

    int i;
#pragma omp parallel for private(i)
    for (i = 0; i < w.size(); i++) {
        res[i] = inner<P>(cs, w[i], gk, encoder_domain, encoder_target);
    }

    encoder = encoder_target;

    return res;
}

vector<double> wtx(const vector<double> x, const vector<vector<double>> w,
                   bool is_print = true)
{
    assert(x.size() == w[0].size());
    vector<double> res;
    for (int i = 0; i < w.size(); i++) {
        res.push_back(inner(x, w[i], false));
    }
    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

template <class P>
vector<TLWE<P>> btx(const vector<TLWE<P>> cs, const vector<double> b,
                    Encoder &encoder)
{
    assert(cs.size() == b.size());

    vector<TLWE<P>> res;
    for (int i = 0; i < b.size(); i++) {
        TLWE<P> tmp;
        TFHEpp::HomADDCONST(tmp, cs[i], b[i], encoder);
        res.push_back(tmp);
    }
    return res;
}

template <class P>
vector<TLWE<P>> btx_omp(const vector<TLWE<P>> cs, const vector<double> b,
                        Encoder &encoder)
{
    assert(cs.size() == b.size());

    vector<TLWE<P>> res(b.size());
    int i;
#pragma omp parallel for private(i)
    for (i = 0; i < b.size(); i++) {
        TFHEpp::HomADDCONST(res[i], cs[i], b[i], encoder);
    }
    return res;
}

vector<double> btx(const vector<double> x, const vector<double> b,
                   bool is_print = true)
{
    assert(x.size() == b.size());
    vector<double> res;
    for (int i = 0; i < x.size(); i++) {
        res.push_back(x[i] + b[i]);
    }
    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

vector<double> multd(vector<double> x, vector<double> b, bool is_print = true)
{
    vector<double> res;
    for (int i = 0; i < x.size(); i++) {
        res.push_back(x[i] * b[i]);
    }
    if (is_print) {
        print_vec_1d(res);
    }
    return res;
}

template <class P>
vector<TLWE<P>> linear(const vector<TLWE<P>> cs, const vector<vector<double>> w,
                       const vector<double> b, const GateKey &gk,
                       Encoder &encoder)
{
    vector<TLWE<lvl0param>> c2 = wtx<lvl0param>(cs, w, gk, encoder, 2.);
    printf("multed\n");
    vector<TLWE<lvl0param>> c3 = btx<lvl0param>(c2, b, encoder);
    printf("added\n");
    return c3;
}

template <class P>
vector<TLWE<P>> linear_omp(const vector<TLWE<P>> cs,
                           const vector<vector<double>> w,
                           const vector<double> b, const GateKey &gk,
                           Encoder &encoder)
{
    vector<TLWE<lvl0param>> c2 = wtx_omp<lvl0param>(cs, w, gk, encoder, 2.);
    printf("multed\n");
    vector<TLWE<lvl0param>> c3 = btx_omp<lvl0param>(c2, b, encoder);
    printf("added\n");
    return c3;
}

template <class P>
vector<TLWE<P>> nonlinear(const vector<TLWE<P>> cs, const GateKey gk,
                          Encoder &encoder, string nonlinear_type)
{
    if (nonlinear_type == "sigmoid") {
        vector<TLWE<lvl0param>> c2 = sigmoid<lvl0param>(cs, gk, encoder);
        return c2;
    }
    else if (nonlinear_type == "relu") {
        vector<TLWE<lvl0param>> c2 = relu<lvl0param>(cs, gk, encoder);
        return c2;
    }
    else {
        cerr << "not known nl_type" << endl;
    }
}

template <class P>
vector<TLWE<P>> nonlinear_omp(const vector<TLWE<P>> cs, const GateKey gk,
                              Encoder &encoder, string nonlinear_type)
{
    if (nonlinear_type == "sigmoid") {
        vector<TLWE<lvl0param>> c2 = sigmoid_omp<lvl0param>(cs, gk, encoder);
        return c2;
    }
    else if (nonlinear_type == "relu") {
        vector<TLWE<lvl0param>> c2 = relu_omp<lvl0param>(cs, gk, encoder);
        return c2;
    }
    else {
        cerr << "not known nl_type" << endl;
    }
}

vector<double> linear(const vector<double> x, const vector<vector<double>> w,
                      vector<double> b)
{
    vector<double> x2 = wtx(x, w, false);
    vector<double> x3 = btx(x2, b, true);
    return x3;
}

vector<double> nonlinear(const vector<double> x, string nonlinear_type)
{
    if (nonlinear_type == "sigmoid") {
        vector<double> res = sigmoid(x);
        return res;
    }
    else if (nonlinear_type == "relu") {
        vector<double> res = relu(x);
        return res;
    }
    else {
        cerr << "not known nl_type" << endl;
    }
}

double avg(vector<double> x)
{
    double res = 0;
    for (int i = 0; i < x.size(); i++) {
        res += x[i];
    }
    return res / double(x.size());
}

void test()
{
    printf("hello, world\n\n");

    // ###########################################
    // model side parameters
    printf(
        "\nprepare_model======================================================="
        "======\n");
    int d1 = 5;
    int d2 = 3;
    int d3 = 1;
    string nl_type1 = "sigmoid";
    string nl_type2 = "sigmoid";
    vector<double> d;

    vector<double> x1 = give_me_vector(d1);
    vector<vector<double>> w = give_me_weight(d2, d1);
    vector<double> b = give_me_vector(d2);
    vector<vector<double>> w2 = give_me_weight(d3, d2);
    vector<double> b2 = give_me_vector(d3);

    printf("\n=============================================================\n");
    printf("x1\n");
    print_vec_1d(x1);
    printf("w\n");
    print_vec_2d(w);
    printf("b\n");
    print_vec_1d(b);

    printf("\n=============================================================\n");
    printf("w\n");
    print_vec_2d(w2);
    printf("b\n");
    print_vec_1d(b2);

    // ###########################################
    // encoder side parameters
    printf("\n=============================================================\n");
    double encoder_a = -3.;
    double encoder_b = 3.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder(encoder_a, encoder_b, bs_bp);
    printf(
        "\nparepare "
        "encoder============================================================="
        "\n");

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    printf(
        "\nencryption=========================================================="
        "===\n");
    vector<TLWE<lvl0param>> c1 =
        encrypt_vector<lvl0param>(x1, sk->key.lvl0, encoder);
    decrypt_vector<lvl0param>(c1, sk->key.lvl0, encoder);

    double start, end;
    vector<double> ts;
    bool is_decrypt = false;
    for (int i = 0; i < 3; i++) {
        start = get_time_msec();

        printf(
            "\ncipher=========================================================="
            "===\n");
        vector<TLWE<lvl0param>> a1 =
            linear<lvl0param>(c1, w, b, *gk.get(), encoder);
        if (is_decrypt) printf("\nlineared \n");
        decrypt_vector<lvl0param>(a1, sk->key.lvl0, encoder);
        // vector<TLWE<lvl0param>> y1 = nonlinear<lvl0param>(a1, *gk.get(),
        // encoder, nl_type1);
        printf("\nnon-lineared\n");
        vector<TLWE<lvl0param>> y1 = sigmoid<lvl0param>(a1, *gk.get(), encoder);
        if (is_decrypt) decrypt_vector<lvl0param>(y1, sk->key.lvl0, encoder);

        printf(
            "\nraw_debug======================================================="
            "======\n");
        vector<double> tmp = linear(x1, w, b);
        tmp = nonlinear(tmp, nl_type1);

        printf(
            "\ncipher=========================================================="
            "===\n");
        vector<TLWE<lvl0param>> a2 =
            linear<lvl0param>(y1, w2, b2, *gk.get(), encoder);
        printf("\nlineared\n");
        if (is_decrypt) decrypt_vector<lvl0param>(a2, sk->key.lvl0, encoder);
        // vector<TLWE<lvl0param>> y2 = nonlinear<lvl0param>(a2, *gk.get(),
        // encoder, nl_type2);
        vector<TLWE<lvl0param>> y2 = sigmoid<lvl0param>(a2, *gk.get(), encoder);
        printf("\nnon-lineared\n");
        if (is_decrypt) decrypt_vector<lvl0param>(y2, sk->key.lvl0, encoder);

        printf(
            "\nraw_debug======================================================="
            "======\n");
        tmp = linear(tmp, w2, b2);
        tmp = nonlinear(tmp, nl_type2);

        end = get_time_msec();
        ts.push_back(end - start);
    }

    double ts_avg = avg(ts);
    printf("time avg: %f\n", ts_avg);
}

void test_omp()
{
    printf("hello, world\n\n");

    // ###########################################
    // model side parameters
    printf(
        "\nprepare_model======================================================="
        "======\n");
    int d1 = 5;
    int d2 = 3;
    int d3 = 1;
    string nl_type1 = "sigmoid";
    string nl_type2 = "sigmoid";
    vector<double> d;

    vector<double> x1 = give_me_vector(d1);
    vector<vector<double>> w = give_me_weight(d2, d1);
    vector<double> b = give_me_vector(d2);
    vector<vector<double>> w2 = give_me_weight(d3, d2);
    vector<double> b2 = give_me_vector(d3);

    printf("\n=============================================================\n");
    printf("x1\n");
    print_vec_1d(x1);
    printf("w\n");
    print_vec_2d(w);
    printf("b\n");
    print_vec_1d(b);

    printf("\n=============================================================\n");
    printf("w\n");
    print_vec_2d(w2);
    printf("b\n");
    print_vec_1d(b2);

    // ###########################################
    // encoder side parameters
    printf("\n=============================================================\n");
    double encoder_a = -3.;
    double encoder_b = 3.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder(encoder_a, encoder_b, bs_bp);
    printf(
        "\nparepare "
        "encoder============================================================="
        "\n");

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    printf(
        "\nencryption=========================================================="
        "===\n");
    vector<TLWE<lvl0param>> c1 =
        encrypt_vector_omp<lvl0param>(x1, sk->key.lvl0, encoder);
    decrypt_vector_omp<lvl0param>(c1, sk->key.lvl0, encoder);

    double start, end;
    vector<double> ts;
    bool is_decrypt = false;
    for (int i = 0; i < 3; i++) {
        start = get_time_msec();

        printf(
            "\ncipher=========================================================="
            "===\n");
        vector<TLWE<lvl0param>> a1 =
            linear_omp<lvl0param>(c1, w, b, *gk.get(), encoder);
        if (is_decrypt) printf("\nlineared \n");
        decrypt_vector<lvl0param>(a1, sk->key.lvl0, encoder);
        // vector<TLWE<lvl0param>> y1 = nonlinear<lvl0param>(a1, *gk.get(),
        // encoder, nl_type1);
        printf("\nnon-lineared\n");
        vector<TLWE<lvl0param>> y1 =
            sigmoid_omp<lvl0param>(a1, *gk.get(), encoder);
        if (is_decrypt) decrypt_vector<lvl0param>(y1, sk->key.lvl0, encoder);

        printf(
            "\nraw_debug======================================================="
            "======\n");
        vector<double> tmp = linear(x1, w, b);
        tmp = nonlinear(tmp, nl_type1);

        printf(
            "\ncipher=========================================================="
            "===\n");
        vector<TLWE<lvl0param>> a2 =
            linear_omp<lvl0param>(y1, w2, b2, *gk.get(), encoder);
        printf("\nlineared\n");
        if (is_decrypt) decrypt_vector<lvl0param>(a2, sk->key.lvl0, encoder);
        // vector<TLWE<lvl0param>> y2 = nonlinear<lvl0param>(a2, *gk.get(),
        // encoder, nl_type2);
        vector<TLWE<lvl0param>> y2 =
            sigmoid_omp<lvl0param>(a2, *gk.get(), encoder);
        printf("\nnon-lineared\n");
        if (is_decrypt) decrypt_vector<lvl0param>(y2, sk->key.lvl0, encoder);

        printf(
            "\nraw_debug======================================================="
            "======\n");
        tmp = linear(tmp, w2, b2);
        tmp = nonlinear(tmp, nl_type2);

        end = get_time_msec();
        ts.push_back(end - start);
    }

    double ts_avg = avg(ts);
    printf("time avg: %f\n", ts_avg);
}

int main()
{
    // test();
    test_omp();
    return 0;
}
