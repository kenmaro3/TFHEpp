#include <tfhe++.hpp>

int main()
{
    double encoder_a = -20.;
    double encoder_b = 20.;
    int bs_bp = 32;

    TFHEpp::Encoder encoder(encoder_a, encoder_b, bs_bp);
    auto identity_function = TFHEpp::IdentityFunction();

    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    std::unique_ptr<TFHEpp::GateKey> gk =
        std::make_unique<TFHEpp::GateKey>(*sk, encoder);

    double x = 10.6;
    double x2 = 5.6;
    double d, mult;

    auto c1 = TFHEpp::tlweSymEncodeEncrypt<TFHEpp::lvl0param>(
        x, TFHEpp::lvl0param::alpha, sk->key.lvl0, encoder);

    std::stringstream ss;

    {
        cereal::PortableBinaryOutputArchive ar(ss);
        gk->serialize(ar);
    }

    std::unique_ptr<TFHEpp::GateKey> gki = std::make_unique<TFHEpp::GateKey>();
    {
        cereal::PortableBinaryInputArchive ar(ss);
        gki->serialize(ar);
    }

    {
        cereal::PortableBinaryOutputArchive ar(ss);
        sk->serialize(ar);
    }

    std::unique_ptr<TFHEpp::SecretKey> ski =
        std::make_unique<TFHEpp::SecretKey>();
    {
        cereal::PortableBinaryInputArchive ar(ss);
        ski->serialize(ar);
    }

    {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(c1);
    }

    printf("pass");
}
