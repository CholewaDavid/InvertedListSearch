#ifndef SHUFFLING_BSR_HPP
#define SHUFFLING_BSR_HPP

#include <x86intrin.h>
#include <cstdint>
#include "intersectionAlgBsr.hpp"


class ShufflingBsr : public IntersectionAlgBsr{
    public:
        ShufflingBsr();
        virtual int intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult);               
        virtual std::string getName();        
    private:
        int intersect_shuffle_bsr_b4(int* bases_a, int* states_a, int size_a,
            int* bases_b, int* states_b, int size_b,
            int* bases_c, int* states_c);
};

#endif