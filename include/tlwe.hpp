#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <cassert>

#include "./key.hpp"
#include "./params.hpp"
#include "./utils.hpp"

namespace TFHEpp {
using namespace std;


class Encoder
{
    public:
        double a;
        double b;
        double d;
        double d2;
        double half;
        double half_d;
        double interp_num;
        bool is_include_negative;
        int bp;

        vector<double> div;
        vector<double> ind;
        vector<double> mul;

        Encoder(double a, double b, int bp){
            if(a<0){
                assert(abs(a) == abs(b));
                this->is_include_negative = true;
            }else{
                this->is_include_negative = false;
            }
            this->a = a;
            this->b = b;
            this->d = b-a;
            this->d2 = b-a;
            this->half_d = (b-a)/2.;
            this->half = (b+a)/2.;

            this->bp = bp;

            this->interp_num = 1.;
                
            this->div.push_back(1.);
            this->ind.push_back(0.);
            this->mul.push_back(0.);
        }

        double encode_0_1(double x) const{
            if(this->is_include_negative){
                if(x-this->a < this->half_d){
                    x = this->b - abs(x);
                    return (x-this->a)/this->d;
                }else{
                    x = x - this->half_d;
                    return (x-this->a)/this->d;
                }
            }
            else{
                return (x-this->a)/this->d;
            } 

        }

        uint32_t dtotx(double d) const{
            return int32_t(int64_t((d - int64_t(d)) * (1LL << this->bp)));
        }

        uint32_t encode(double x) const{
            assert(x >= this->a);
            assert(x <= this->b);
            if (x == this->a) x = encode_sanitize(x);
            if(this->is_include_negative){
                if(x-this->a < this->half_d){
                    x = this->b - abs(x);
                    //return dtot32((x-this->a)/this->d);
                    return dtotx((x-this->a)/this->d);
                }else{
                    x = x - this->half_d;
                    return dtotx((x-this->a)/this->d);
                    //return dtot32((x-this->a)/this->d);
                }
            }
            else{
                //return dtot32((x-this->a)/this->d);
                return dtotx((x-this->a)/this->d);
            } 
        }

        uint32_t encode_second(double x) const{
            //assert(x >= 0.);
            //assert(x <=1.);
            x = x - floor(x);
            x = x * this->d + this->a;
            return dtotx((x-this->a)/this->d);
        }

        uint32_t encode_off_limit(double x) const{
            while(x >= this->b){
                x -= this->b;
            }
            while(x<=this->a){
                x += this->b;
            }
            //printf("here: %f\n", x);
            if (x == this->a) x = encode_sanitize(x);
            if(this->is_include_negative){
                if(x-this->a < this->half_d){
                    x = this->b - abs(x);
                   // return dtot32((x-this->a)/this->d);
                    return dtotx((x-this->a)/this->d);
                }else{
                    x = x - this->half_d;
                    //return dtot32((x-this->a)/this->d);
                    return dtotx((x-this->a)/this->d);
                }
            }
            else{
                //printf("here2\n");
                //printf("x: %f\n", x);
                //return dtot32((x-this->a)/this->d);
                return dtotx((x-this->a)/this->d);
            } 
        }

        double decode_0_1(const double x){
            if(this->is_include_negative){
                double tmp_0_1 = x;
                assert(tmp_0_1 >= 0.);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                if(tmp_0_1 > 0.5){
                    double tmp2 = tmp_0_1 - 0.5;
                    return decode_sanitize(this->a + tmp2 * this->d, this->b);
                }else{
                    return decode_sanitize(tmp_0_1 * this->d, this->b);

                }

            }else{
                double tmp_0_1 = x;
                //printf("tmp_0_1: %f\n", tmp_0_1);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                return decode_sanitize(tmp_0_1 * this->d, this->b);
            }
        }


        double txtod(uint32_t x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, this->bp);
            return tmp_0_1;
        }

        double t32tod(uint32_t x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, 32);
            return tmp_0_1;
        }

        double decode2(const uint32_t x){
            //printf("decrypt: %d\n", x);
            if(this->is_include_negative){
                //double tmp_0_1 = t32tod(x);
                double tmp_0_1 = this->txtod(x);
                //printf("tmp_0_1: %f\n", tmp_0_1);
                assert(tmp_0_1 >= 0.);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                if(tmp_0_1 > 0.5){
                    double tmp2 = tmp_0_1 - 0.5;
                    return decode_sanitize(this->a + tmp2 * this->d, this->b);
                }else{
                    return decode_sanitize(tmp_0_1 * this->d, this->b);

                }

            }else{
                //double tmp_0_1 = t32tod(x);
                double tmp_0_1 = this->txtod(x);
                //printf("tmp_0_1: %f\n", tmp_0_1);
                //tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                return decode_sanitize(tmp_0_1 * this->d, this->b);
            }
        }

        double decode_second(const uint32_t x){
            double tmp_0_1 = this->txtod(x);
            tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
            return tmp_0_1;
        }

        double decode(const uint32_t x){
            //printf("decrypt: %d\n", x);
            if(this->is_include_negative){
                //double tmp_0_1 = t32tod(x);
                double tmp_0_1 = this->txtod(x);
                //printf("tmp_0_1: %f\n", tmp_0_1);
                assert(tmp_0_1 >= 0.);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                if(tmp_0_1 > 0.5){
                    double tmp2 = tmp_0_1 - 0.5;
                    return decode_sanitize(this->a + tmp2 * this->d, this->b);
                }else{
                    return decode_sanitize(tmp_0_1 * this->d, this->b);

                }

            }else{
                //double tmp_0_1 = t32tod(x);
                double tmp_0_1 = this->txtod(x);
                printf("\ntmp_0_1a: %f\n", tmp_0_1);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);

                printf("tmp_0_1b: %f\n", tmp_0_1);
                tmp_0_1 = this->interpret(tmp_0_1);
                return tmp_0_1;
                //return decode_sanitize(tmp_0_1* this->d, this->b);
            }
        }

        double interpret(double x){
            double res = this->a;
            double tmp = 1.;
            for(int i=0; i<this->div.size(); i++){
                printf("\n%d: div: %f, ind: %f, mul: %f\n",i, this->div[i], this->ind[i], this->mul[i]);
                tmp = tmp/this->div[i];
                printf("tmp %f\n", tmp);
                printf("xbefore %f\n", x);
                while(x > tmp){
                    x -= tmp;
                }
                printf("xafter %f\n", x);
                double off = this->mul[i] * this->ind[i];
                printf("off %f\n", off);
                res += off;
                printf("res %f\n", res);
                


            }
            res += x * this->d;

            return res;
        }

};

template <class P>
array<typename P::T, P::n + 1> tlweSymEncodeEncrypt(
    const double &x, const double alpha,
    const array<typename P::T, P::n> &key, const Encoder &encoder);

template <class P>
double tlweSymDecryptDecode(const TLWE<P> &c, const Key<P> &key, Encoder &encoder);

template <class P>
array<typename P::T, P::n + 1> tlweSymEncrypt(
    const typename P::T p, const double alpha,
    const array<typename P::T, P::n> &key);

template <class P>
bool tlweSymDecrypt(const TLWE<P> &c, const Key<P> &key);

template <class P>
typename P::T tlweSymDecryptRaw(const TLWE<P> &c, const Key<P> &key);

vector<TLWE<lvl0param>> bootsSymEncrypt(const vector<uint8_t> &p,
                                        const SecretKey &sk);
vector<uint8_t> bootsSymDecrypt(const vector<TLWE<lvl0param>> &c,
                                const SecretKey &sk);
}  // namespace TFHEpp