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
        double half;
        double half_d;
        double interp_num;
        bool is_include_negative;

        Encoder(double a, double b){
            if(a<0){
                assert(abs(a) == abs(b));
                this->is_include_negative = true;
            }else{
                this->is_include_negative = false;
            }
            this->a = a;
            this->b = b;
            this->d = b-a;
            this->half_d = (b-a)/2.;
            this->half = (b+a)/2.;

            this->interp_num = 1.;
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

        uint32_t encode(double x) const{
            assert(x >= this->a);
            assert(x <= this->b);
            if (x == this->a) x = encode_sanitize(x);
            if(this->is_include_negative){
                if(x-this->a < this->half_d){
                    x = this->b - abs(x);
                    return dtot32((x-this->a)/this->d);
                }else{
                    x = x - this->half_d;
                    return dtot32((x-this->a)/this->d);
                }
            }
            else{
                return dtot32((x-this->a)/this->d);
            } 
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
                    return dtot32((x-this->a)/this->d);
                }else{
                    x = x - this->half_d;
                    return dtot32((x-this->a)/this->d);
                }
            }
            else{
                //printf("here2\n");
                //printf("x: %f\n", x);
                return dtot32((x-this->a)/this->d);
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

        double decode2(const uint32_t x){
            //printf("decrypt: %d\n", x);
            if(this->is_include_negative){
                double tmp_0_1 = t32tod(x);
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
                double tmp_0_1 = t32tod(x);
                //printf("tmp_0_1: %f\n", tmp_0_1);
                //tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                return decode_sanitize(tmp_0_1 * this->d, this->b);
            }
        }

        double decode(const uint32_t x){
            //printf("decrypt: %d\n", x);
            if(this->is_include_negative){
                double tmp_0_1 = t32tod(x);
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
                double tmp_0_1 = t32tod(x);

                printf("tmp_0_1: %f\n", tmp_0_1);
                //tmp_0_1 = this->interpret(tmp_0_1);
                return decode_sanitize(tmp_0_1* this->d, this->b);
            }
        }

        double interpret(double x){
            x *= this->interp_num;
            x = x - floor(x);

            x = x/this->interp_num;

            //if (this->interp_num != 1.){
            //    x = x + 1./this->interp_num;
            //}

            return x;
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