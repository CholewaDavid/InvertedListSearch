#ifndef SHUFFLING_HPP
#define SHUFFLING_HPP

#include <x86intrin.h>
#include <cstdint>
#include "intersectionAlg.hpp"


class Shuffling : public IntersectionAlg{
    public:
        Shuffling();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_shuffle_uint_b4(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
};

#endif