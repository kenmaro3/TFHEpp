#include <stdio.h>

#include <cassert>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>

using namespace std;
using namespace TFHEpp;

#include <chrono>
using namespace std::chrono;
inline double get_time_sec(void)
{
    return static_cast<double>(duration_cast<nanoseconds>(
                                   steady_clock::now().time_since_epoch())
                                   .count()) /
           1000000000;
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

int main()
{
    printf("hello, world\n\n");
    double encoder_a = -2.;
    double encoder_b = 2.;
    int bs_bp = 31;

    TFHEpp::Encoder encoder_bs(encoder_a, encoder_b, bs_bp);
    ReLUFunction relu_function = ReLUFunction<lvl1param>();

    // generate a random key
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder_bs);

    int dim1 = 4;
    int dim2 = 3;
    vector<vector<double>> w(dim1, vector<double>(dim2));
    vector<double> b(dim2);
    vector<double> x(dim1);

    printf("\n=============================================================\n");
    cout << "settings" << endl;
    cout << "dim1: " << w.size() << endl;
    cout << "dim2: " << w[0].size() << endl;

    assert(w.size() == dim1);
    assert(w[0].size() == dim2);
    assert(b.size() == dim2);
    assert(x.size() == dim1);

    for (int i = 0; i < w.size(); i++) {
        for (int j = 0; j < w[0].size(); j++) {
            w[i][j] = (double)i * 0.1;
        }
    }
    for (int i = 0; i < b.size(); i++) {
        b[i] = (double)i * (-0.1);
    }
    for (int i = 0; i < x.size(); i++) {
        x[i] = (double)i * 0.1;
    }

    cout << "vec x" << endl;
    print_vec_1d(x);

    cout << "vec w" << endl;
    print_vec_2d(w);

    cout << "vec b" << endl;
    print_vec_1d(b);

    Encoder encoder(encoder_a, encoder_b, 31);
    vector<TLWE<lvl0param>> cs(dim1);
    vector<Encoder> encoders(dim1);

    for (int i = 0; i < dim1; i++) {
        Encoder encoder(encoder_a, encoder_b, 31);
        cs[i] = TFHEpp::tlweSymEncodeEncrypt<lvl0param>(x[i], lvl0param::alpha,
                                                        sk->key.lvl0, encoder);
        encoders[i] = encoder;
    }
    vector<double> decs;
    for (int i = 0; i < cs.size(); i++) {
        decs.push_back(TFHEpp::tlweSymDecryptDecode<lvl0param>(
            cs[i], sk->key.lvl0, encoders[i]));
    }
    cout << "decs" << endl;
    print_vec_1d(decs);

    //####################################################################################################
    // cipher calc
    vector<vector<TLWE<lvl0param>>> cs_copy(dim2,
                                            vector<TLWE<lvl0param>>(dim1));
    for (int i = 0; i < dim2; i++) {
        cs_copy[i] = cs;
    }
    vector<vector<Encoder>> encoder_copy(dim2, vector<Encoder>(dim1));
    for (int i = 0; i < dim2; i++) {
        encoder_copy[i] = encoders;
    }

    assert(cs_copy.size() == dim2);
    assert(cs_copy[0].size() == dim1);

    vector<TLWE<lvl0param>> csw(dim2);
    vector<Encoder> encodersw(dim2);
    for (int j = 0; j < dim2; j++) {
        TLWE<lvl0param> tmp;
        TFHEpp::HomMULTCONSTREAL(tmp, cs_copy[j][0], w[0][j],
                                 encoder_copy[j][0], 8, 1);
        for (int i = 1; i < dim1; i++) {
            TLWE<lvl0param> tmp2;
            TFHEpp::HomMULTCONSTREAL(tmp2, cs_copy[j][i], w[i][j],
                                     encoder_copy[j][i], 8, 1);
            if (i == 1) {
                TFHEpp::HomADDFixedEncoder(tmp, tmp, tmp2, encoder_copy[j][0],
                                           encoder_copy[j][i]);
            }
            else {
                TFHEpp::HomADDFixedEncoder(tmp, tmp, tmp2, encoder_copy[j][0],
                                           encoder_copy[j][i]);
            }
        }
        csw[j] = tmp;
        encodersw[j] = encoder_copy[j][0];
    }
    assert(csw.size() == dim2);

    vector<TLWE<lvl0param>> cswb(dim2);
    vector<Encoder> encoderswb(dim2);
    for (int i = 0; i < dim2; i++) {
        TFHEpp::HomADDCONST(cswb[i], csw[i], b[i], encodersw[i]);
        encoderswb[i] = encodersw[i];
    }
    assert(cswb.size() == dim2);

    vector<TLWE<lvl0param>> cswbr(dim2);
    vector<Encoder> encoderswbr(dim2);
    encoder_bs.update(encoderswb[0].a, encoderswb[0].b, encoder_bs.bp);
    for (int i = 0; i < dim2; i++) {
        TFHEpp::ProgrammableBootstrapping(cswbr[i], cswb[i], *gk.get(),
                                          encoderswb[i], encoder_bs,
                                          relu_function);
        encoderswbr[i] = encoder_bs;
    }

    printf("\n=============================================================\n");
    cout << "res_cipher" << endl;
    vector<double> decsw;
    for (int i = 0; i < csw.size(); i++) {
        decsw.push_back(TFHEpp::tlweSymDecryptDecode<lvl0param>(
            csw[i], sk->key.lvl0, encodersw[i]));
    }

    cout << "decsw" << endl;
    print_vec_1d(decsw);

    vector<double> decswb;
    for (int i = 0; i < cswb.size(); i++) {
        decswb.push_back(TFHEpp::tlweSymDecryptDecode<lvl0param>(
            cswb[i], sk->key.lvl0, encoderswb[i]));
    }

    cout << "decswb" << endl;
    print_vec_1d(decswb);

    vector<double> decswbr;
    for (int i = 0; i < cswbr.size(); i++) {
        decswbr.push_back(TFHEpp::tlweSymDecryptDecode<lvl0param>(
            cswbr[i], sk->key.lvl0, encoderswbr[i]));
    }

    cout << "decswbr" << endl;
    print_vec_1d(decswbr);

    //####################################################################################################
    // validation with raw calc
    vector<double> raww(dim2);
    for (int j = 0; j < dim2; j++) {
        double tmp = x[0] * w[0][j];
        for (int i = 1; i < dim1; i++) {
            tmp += x[i] * w[i][j];
        }
        raww[j] = tmp;
    }
    printf("\n=============================================================\n");
    cout << "res_raw" << endl;
    cout << "raww" << endl;
    print_vec_1d(raww);

    vector<double> rawwb(dim2);
    for (int i = 0; i < dim2; i++) {
        rawwb[i] = raww[i] + b[i];
    }

    cout << "rawwb" << endl;
    print_vec_1d(rawwb);

    vector<double> rawwbr(dim2);
    for (int i = 0; i < dim2; i++) {
        rawwbr[i] = relu_function.run(rawwb[i]);
    }

    cout << "rawwbr" << endl;
    print_vec_1d(rawwbr);
}