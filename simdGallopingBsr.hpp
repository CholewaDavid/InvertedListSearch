#ifndef SIMD_GALLOPING_BSR_HPP
#define SIMD_GALLOPING_BSR_HPP

#include "intersectionAlgBsr.hpp"


class SimdGallopingBsr : public IntersectionAlgBsr{
    public:
        SimdGallopingBsr();
        virtual int intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_simdgalloping_bsr(int* bases_a, int* states_a, int size_a, int* bases_b, int* states_b, int size_b, int* bases_c, int* states_c);
};


#endif