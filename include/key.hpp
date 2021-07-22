#pragma once

#include <randen.h>

#include <algorithm>
#include <array>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/array.hpp>

#include "lweParams.hpp"
#include "params.hpp"
#include "random"

namespace TFHEpp {
using namespace std;
struct lweKey {
    Key<lvl0param> lvl0;
    Key<lvl1param> lvl1;
    Key<lvl2param> lvl2;
    lweKey();
    template <class P>
    Key<P> get() const;
};

struct SecretKey {
    lweKey key;
    lweParams params;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(key.lvl0, key.lvl1, key.lvl2, params);
    }

    template <class P>
    void print(){
        for (int i=0; i<P::n; i++){
            printf("%d, ", key.get<P>()[i]);
        }
        printf("\n");

    }
};
}  // namespace TFHEpp