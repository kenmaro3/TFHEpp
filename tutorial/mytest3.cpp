#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace TFHEpp;

template <class P>
class ETLWE{
    public:
        Encoder encoder;
        TLWE<P> tlwe;

        ETLWE(Encoder encoder, TLWE<P> tlwe){
            this->encoder = encoder;
            this->tlwe = tlwe;
        }

        void print_encoder(){
            this->encoder.print();
        }

        ETLWE operator+(double x){
            TFHEpp::TLWE<P> tmp_tlwe = this->tlwe;
            Encoder tmp_encoder = this->encoder;
            TFHEpp::HomADDCONST(this->tlwe, tmp_tlwe, x, tmp_encoder);
            return  *this;
        }

        ETLWE operator+(ETLWE<P> x){
            TFHEpp::TLWE<P> tmp_tlwe = this->tlwe;
            TFHEpp::HomADD(this->tlwe, tmp_tlwe, x.tlwe, this->encoder, x.encoder);
            return  *this;
        }

        ETLWE operator-(double x){
            double tmp_x = x*(-1.);
            TFHEpp::TLWE<P> tmp_tlwe = this->tlwe;
            Encoder tmp_encoder = this->encoder;
            TFHEpp::HomADDCONST(this->tlwe, tmp_tlwe, tmp_x, tmp_encoder);
            return  *this;
        }

        ETLWE operator-(ETLWE<P> x){
            TFHEpp::TLWE<P> tmp_tlwe = this->tlwe;
            TFHEpp::HomSUB(this->tlwe, tmp_tlwe, x.tlwe, this->encoder, x.encoder);
            return  *this;
        }

        ETLWE operator*(int x){
            TFHEpp::TLWE<P> tmp_tlwe = this->tlwe;
            Encoder tmp_encoder = this->encoder;
            TFHEpp::HomMULTCONSTINT(this->tlwe, tmp_tlwe, x);
            return  *this;
        }

        ETLWE operator*(double x){
            Encoder const_encoder(-2, 2, 4);
            TFHEpp::TLWE<P> tmp_tlwe = this->tlwe;
            Encoder tmp_encoder = this->encoder;
            TFHEpp::HomMULTCONSTREAL(this->tlwe, tmp_tlwe, x, this->encoder, const_encoder);
            return  *this;
        }
};

template<class P>
class EncryptionService{
    public:
        std::shared_ptr<TFHEpp::SecretKey> sk;
        std::shared_ptr<TFHEpp::GateKey> gk;

        EncryptionService(){
            std::shared_ptr<TFHEpp::SecretKey> sk =
                std::make_shared<TFHEpp::SecretKey>();
            std::shared_ptr<TFHEpp::GateKey> gk =
                std::make_shared<TFHEpp::GateKey>(*sk);

            this->sk = sk;
            this->gk = gk;
        }

        ETLWE<P> encrypt(double x, Encoder encoder){
            TLWE<P> ct = tlweSymEncodeEncrypt<P>(x, P::alpha, this->sk->key.lvl0, encoder);
            return ETLWE<P>(this->encoder, &ct);
        }

        ETLWE<P> encrypt(double x){
            Encoder tmp_encoder(-10, 10, 16);
            TLWE<P> ct = tlweSymEncodeEncrypt<P>(x, P::alpha, this->sk->key.lvl0, tmp_encoder);
            return ETLWE<P>(tmp_encoder, ct);
        }


        double decrypt(ETLWE<P> x){
            double dec = tlweSymDecryptDecode<P>(x.tlwe, this->sk->key.lvl0, x.encoder);
            return dec;
        }


};


int main()
{
    cout << "hello, world" << endl;

    EncryptionService<TFHEpp::lvl0param> enc;
    cout << "debug1" << endl;

    double x1 = 2.;
    double x2 = 3.;

    auto c1 = enc.encrypt(x1);
    auto c2 = enc.encrypt(x2);
    cout << "debug2" << endl;

    //auto c3 = c1 * 0.5;
    auto c3 = c1 - c2;

    double d1 = enc.decrypt(c1);
    cout << d1 << endl;

    double d3 = enc.decrypt(c3);
    cout << d3 << endl;
    c3.print_encoder();






}