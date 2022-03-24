#include <stdio.h>
#include <memory>
#include <tfhe++.hpp>
#include <cassert>
#include <iostream>
#include <random>

using namespace std;
using namespace TFHEpp;

int main(){
  printf("hello, world\n");



  random_device seed_gen;
  default_random_engine engine(seed_gen());
  uniform_int_distribution<uint32_t> binary(0, 1);

  lweKey key;

  array<bool, lvl1param::n> p;
  for(int i=0; i<3; i++){
    p[i] = 1;
  }
  
  printf("\nhere\n");
  for(int i=0; i<10; i++){
    printf("%d, ", p[i]);
  }
  printf("\n");

  Polynomial<lvl1param> pmu;
  for (int i = 0; i < lvl1param::n; i++)
      pmu[i] = p[i] ? lvl1param::mu : -lvl1param::mu;
  TRLWE<lvl1param> c =
      trlweSymEncrypt<lvl1param>(pmu, lvl1param::alpha, key.lvl1);

  const Polynomial<TFHEpp::lvl1param> plainpoly = {
      static_cast<typename lvl1param::T>(1)};

  TRGSWFFT<lvl1param> trgswfft =
      //trgswfftSymEncryptFake<lvl1param>(plainpoly, lvl1param::alpha, key.lvl1, encoder);
      trgswfftSymEncrypt<lvl1param>(plainpoly, lvl1param::alpha, key.lvl1);
  trgswfftExternalProduct<lvl1param>(c, c, trgswfft);
  array<bool, lvl1param::n> p2 = trlweSymDecrypt<lvl1param>(c, key.lvl1);
  //for (int i = 0; i < 10; i++) assert(p[i] == p2[i]);

  printf("\ntest1\n");
  for(int i=0; i<10; i++){
    printf("%d, ", p[i]);
  }
  printf("\n");

  printf("\ntest2\n");
  for(int i=0; i<10; i++){
    printf("%d, ", p2[i]);
  }
  printf("\n");

  return 0;


  
  // array<double, TFHEpp::lvl1param::n> tmp1;
  // for(int i=0; i<3; i++){
  //   tmp1[i] = i;
  // }
  // TRLWE<TFHEpp::lvl1param> c1 = TFHEpp::trlweSymEncodeEncrypt<TFHEpp::lvl1param>(
  //   tmp1, TFHEpp::lvl1param::alpha, key.lvl1, encoder
  //   );
  // array<double, TFHEpp::lvl1param::n> d_original = trlweSymDecryptDecode<lvl1param>(c1, key.lvl1, encoder);

  // TRGSWFFT<lvl1param> cr1 =
  //     trgswfftSymEncryptFake<lvl1param>(plainpoly, lvl1param::alpha, key.lvl1, encoder);
  // trgswfftExternalProduct<lvl1param>(c1, c1, cr1);

  // array<double, TFHEpp::lvl1param::n> d;
  // d = TFHEpp::trlweSymDecryptDecode<TFHEpp::lvl1param>(c1, key.lvl1, encoder);


  // printf("\noriginal\n");
  // for(int i=0; i<10; i++){
  //   printf("%lf, ", d_original[i]);
  // }
  // printf("\n");

  // printf("\nafter mult\n");
  // for(int i=0; i<10; i++){
  //   printf("%lf, ", d[i]);
  // }
  // printf("\n");
}
