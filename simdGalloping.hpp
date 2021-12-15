#ifndef SIMD_GALLOPING_HPP
#define SIMD_GALLOPING_HPP

#include "intersectionAlg.hpp"


class SimdGalloping : public IntersectionAlg{
    public:
        SimdGalloping();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_simdgalloping_uint(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
};


#endif