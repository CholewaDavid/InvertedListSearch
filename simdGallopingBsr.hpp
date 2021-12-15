#ifndef SIMD_GALLOPING_BSR_HPP
#define SIMD_GALLOPING_BSR_HPP

#include "intersectionAlg.hpp"


class SimdGallopingBsr : public IntersectionAlg{
    public:
        SimdGallopingBsr();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_simdgalloping_bsr(int* bases_a, int* states_a, int size_a, int* bases_b, int* states_b, int size_b, int* bases_c, int* states_c);
};


#endif