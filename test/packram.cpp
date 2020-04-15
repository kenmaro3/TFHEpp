#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <bitset>
#include <tfhe++.hpp>

using namespace std;
using namespace TFHEpp;

template <uint32_t address_bit, uint32_t width_bit>
void URAMUX(TRLWElvl1 &res, const array<TRGSWFFTlvl1, address_bit> &invaddress,
            const array<TRLWElvl1, 1 << (address_bit - width_bit)> &data)
{
    constexpr uint32_t Ubit = address_bit - width_bit;
    constexpr uint32_t num_trlwe = 1 << (Ubit);
    array<TRLWElvl1, num_trlwe / 2> temp;

    for (uint32_t index = 0; index < num_trlwe / 2; index++) {
        CMUXFFTlvl1(temp[index], invaddress[width_bit], data[2 * index],
                    data[2 * index + 1]);
    }

    for (uint32_t bit = 0; bit < (Ubit - 2); bit++) {
        const uint32_t stride = 1 << bit;
        for (uint32_t index = 0; index < (num_trlwe >> (bit + 2)); index++) {
            CMUXFFTlvl1(
                temp[(2 * index) * stride], invaddress[width_bit + bit + 1],
                temp[(2 * index) * stride], temp[(2 * index + 1) * stride]);
        }
    }

    constexpr uint32_t stride = 1 << (Ubit - 2);
    CMUXFFTlvl1(res, invaddress[address_bit - 1], temp[0], temp[stride]);
}

template <uint32_t address_bit, uint32_t width_bit>
void LRAMUX(vector<TLWElvl0> &res,
            const array<TRGSWFFTlvl1, address_bit> &address,
            const TRLWElvl1 &data, const KeySwitchingKey &ksk)
{
    TRLWElvl1 temp, acc;
    PolynomialMulByXaiMinusOnelvl1(temp[0], data[0], 2 * DEF_N - (DEF_N >> 1));
    PolynomialMulByXaiMinusOnelvl1(temp[1], data[1], 2 * DEF_N - (DEF_N >> 1));
    trgswfftExternalProductlvl1(temp, temp, address[width_bit - 1]);
    for (int i = 0; i < DEF_N; i++) {
        acc[0][i] = temp[0][i] + data[0][i];
        acc[1][i] = temp[1][i] + data[1][i];
    }

    for (uint32_t bit = 2; bit <= width_bit; bit++) {
        PolynomialMulByXaiMinusOnelvl1(temp[0], acc[0],
                                       2 * DEF_N - (DEF_N >> bit));
        PolynomialMulByXaiMinusOnelvl1(temp[1], acc[1],
                                       2 * DEF_N - (DEF_N >> bit));
        trgswfftExternalProductlvl1(temp, temp, address[width_bit - bit]);
        for (int i = 0; i < DEF_N; i++) {
            acc[0][i] += temp[0][i];
            acc[1][i] += temp[1][i];
        }
    }

    const uint32_t width = 1 << width_bit;
    array<TLWElvl1, width> reslvl1;
    for (int i = 0; i < width; i++) SampleExtractIndexlvl1(reslvl1[i], acc, i);
    for (int i = 0; i < width; i++)
        IdentityKeySwitchlvl10(res[i], reslvl1[i], ksk);
}

template<uint32_t address_bit, uint32_t width_bit>
void UWRITE(array<TRLWElvl1, 1 << (address_bit - width_bit)> &wumemory,const array<TRLWElvl1, 1 << (address_bit - width_bit)> &packedram, const array<array<TRGSWFFTlvl1, address_bit>, 2> &address, const array<TLWElvl1,1U<<(DEF_Nbit - width_bit)> &controled, const PackingKey &pk){
    constexpr uint32_t width = 1U<<width_bit;
    constexpr uint32_t word = 1U<<(DEF_Nbit - width_bit);
    constexpr uint32_t num_trlwe = 1 << (address_bit - width_bit);

    PrepackTLWElvl1 ptlwe;
    for(int i = 0; i< width; i++) for(int j = 0;j<word;j++) ptlwe[word*i+j] = controled[j];
    Packinglvl1(wumemory[0],ptlwe,pk);
    for(int i = 1;i<num_trlwe;i++) wumemory[i] = wumemory[0];

    for(int i = 0;i<num_trlwe;i++){
        const bitset<address_bit-width_bit> addressbitset(i);
        for (int k = 0; k < address_bit-width_bit; k++)
            CMUXFFTlvl1(wumemory[i], address[addressbitset[k]][k+width_bit], wumemory[i],
                            packedram[i]);
    }
}

template<uint32_t address_bit, uint32_t width_bit>
void LWRITE(array<PrepackTLWElvl1,1 << (address_bit - width_bit)> &encram, const array<TRLWElvl1, 1 << (address_bit - width_bit)> &wumemory, const array<TRLWElvl1, 1 << (address_bit - width_bit)> &packedram, const array<array<TRGSWFFTlvl1, address_bit>, 2> &address, const GateKey &gk){
    constexpr uint32_t width = 1U<<width_bit;
    constexpr uint32_t word = 1U<<(DEF_Nbit - width_bit);
    constexpr uint32_t num_trlwe = 1 << (address_bit - width_bit);

    for(int i = 0;i<num_trlwe;i++){
        array<TRLWElvl1, width> temp;
        CMUXFFTlvl1(temp[0],address[0][0],wumemory[i],packedram[i]);
        CMUXFFTlvl1(temp[1],address[1][0],wumemory[i],packedram[i]);
        for(int j = 1;j<width_bit;j++){
            const int chank = 1<<j;
            for(int k = 0;k<chank;k++){
                CMUXFFTlvl1(temp[chank+k],address[1][j],temp[k],packedram[i]);
                CMUXFFTlvl1(temp[k],address[0][j],temp[k],packedram[i]);
            }
        }
        for(int j = 0;j<width;j++){
            for(int k = 0;k<word; k++){
                TLWElvl1 bufflvl1;
                TLWElvl0 bufflvl0;
                const int index = j*word+k;
                SampleExtractIndexlvl1(bufflvl1,temp[j],index);
                IdentityKeySwitchlvl10(bufflvl0,bufflvl1,gk.ksk);
                GateBootstrappingTLWE2TLWEFFTlvl01(encram[i][index],bufflvl0,gk);
            }
        }
    }
}

int main()
{
    constexpr uint32_t address_bit = 9;  // Address by words.
    constexpr uint32_t word_bit = 3;
    constexpr uint32_t width_bit =
        DEF_Nbit -
        word_bit;  // log_2 of how many words are in one TRLWElvl1 message.
    static_assert(static_cast<int>(width_bit)>0);
    constexpr uint32_t width = 1 << width_bit;
    constexpr uint32_t num_trlwe = 1 << (address_bit - width_bit);
    constexpr uint32_t word = 1U<<(DEF_Nbit - width_bit);

    random_device seeder;
    default_random_engine engine(seeder());
    uniform_int_distribution<uint8_t> binary(0, 1);

    SecretKey *sk = new SecretKey;
    CloudKey *ck = new CloudKey(*sk);
    vector<array<uint8_t, DEF_N>> pmemory(num_trlwe);
    vector<array<uint32_t, DEF_N>> pmu(num_trlwe);
    vector<uint8_t> address(address_bit);
    vector<uint8_t> pres(width);

    array<array<TRGSWFFTlvl1, address_bit>, 2> *bootedTGSW =
        new array<array<TRGSWFFTlvl1, address_bit>, 2>;
    vector<TLWElvl0> encaddress(address_bit);
    array<PrepackTLWElvl1, num_trlwe> encmemory;
    vector<TLWElvl0> encreadres(width);

    vector<uint8_t> writep(word);

    chrono::system_clock::time_point start, end;

    for(int wrflag = 0;wrflag<=1;wrflag++){

        for (array<uint8_t, DEF_N> &i : pmemory)
            for (uint8_t &p : i) p = binary(engine);
        for (int i = 0; i < num_trlwe; i++)
            for (int j = 0; j < DEF_N; j++)
                pmu[i][j] = pmemory[i][j] ? DEF_μ : -DEF_μ;
        for (uint8_t &p : address) p = binary(engine);

        uint32_t uaddress = 0;
        uint32_t laddress = 0;
        for (int i = 0; i < (address_bit - width_bit); i++)
            uaddress += address[i + width_bit] << i;
        for (int i = 0; i < width_bit; i++)
            laddress += static_cast<uint32_t>(address[i]) << (i + word_bit);

        encaddress = bootsSymEncrypt(address, *sk);
        for (int i = 0; i < num_trlwe; i++)
            for(int j = 0; j<DEF_N;j++)
                encmemory[i][j] = tlweSymEncryptlvl1(pmu[i][j], DEF_αbk, (*sk).key.lvl1);

        for (int i = 0; i < word; i++)
                    writep[i] = (pmemory[uaddress][laddress + i] > 0) ? 0 : 1;
        

        TLWElvl0 encwrflag;
        encwrflag = tlweSymEncryptlvl0((wrflag > 0) ? DEF_μ : -DEF_μ, DEF_α,
                                            (*sk).key.lvl0);
        vector<TLWElvl0> encwritep(word);
        encwritep = bootsSymEncrypt(writep,*sk);


        start = chrono::system_clock::now();

        // Addres CB
        for (int i = 0; i < address_bit; i++) {
            CircuitBootstrappingFFTwithInv((*bootedTGSW)[1][i], (*bootedTGSW)[0][i],
                                        encaddress[i], (*ck).ck);
        }

        //Packing
        array<TRLWElvl1,num_trlwe> packedmem;
        for(int i = 0; i<num_trlwe;i++) Packinglvl1(packedmem[i],encmemory[i],(*ck).pack);
        TRLWElvl1 encumemory;

        //Read
        URAMUX<address_bit, width_bit>(encumemory, (*bootedTGSW)[0], packedmem);
        LRAMUX<address_bit, width_bit>(encreadres, (*bootedTGSW)[1], encumemory,
                                    (*ck).gk.ksk);
        //Control
        array<TLWElvl1,word> writed;
        for (int i = 0; i < word; i++){
            TRLWElvl1 temp;
                    HomMUXwoSE(temp, encwrflag, encwritep[i], encreadres[i],
                            (*ck).gk);
            SampleExtractIndexlvl1(writed[i],temp,0);
        }

        //write
        array<TRLWElvl1, num_trlwe> wumemory;
        UWRITE<address_bit, width_bit>(wumemory,packedmem,(*bootedTGSW),writed,(*ck).pack);
        LWRITE<address_bit, width_bit>(encmemory,wumemory,packedmem,*bootedTGSW,(*ck).gk);

        end = chrono::system_clock::now();

        //readcheck
        pres = bootsSymDecrypt(encreadres, *sk);
        
        array<bool, DEF_N> umemory;
        umemory = trlweSymDecryptlvl1(encumemory, (*sk).key.lvl1);

        for (uint32_t i = 0; i < (1 << word_bit); i++)
            assert(static_cast<int>(pres[i]) ==
                static_cast<int>(pmemory[uaddress][laddress + i]));

        //writecheck
        array<bool, word> pwriteres;
        for (int i = 0; i < word; i++)
                    pwriteres[i] =
                        tlweSymDecryptlvl1(encmemory[uaddress][laddress+i], (*sk).key.lvl1);
        for (int i = 0; i < word; i++)
                assert(static_cast<int>(writep[i]) ==
                       static_cast<int>(
                           tlweSymDecryptlvl0(encwritep[i], (*sk).key.lvl0)));

        for (int i = 0; i < word; i++)
                    assert(static_cast<int>(tlweSymDecryptlvl1(writed[i],(*sk).key.lvl1)) ==
                        static_cast<int>((wrflag>0) ? writep[i]
                                                : pmemory[uaddress][laddress+i]));

        for (int i = 0; i < word; i++)
                    cout<<static_cast<int>(pwriteres[i]) <<
                        static_cast<int>((wrflag>0) ? writep[i]
                                                : pmemory[uaddress][laddress+i])<<endl;
        for (int i = 0; i < word; i++)
                    assert(static_cast<int>(pwriteres[i]) ==
                        static_cast<int>((wrflag>0) ? writep[i]
                                                : pmemory[uaddress][laddress+i]));

        cout << "Passed" << endl;
        double elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
        cout << elapsed << "ms" << endl;
    }
}