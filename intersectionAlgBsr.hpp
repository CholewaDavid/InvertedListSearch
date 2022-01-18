#ifndef INTERSECTION_ALG_BSR_HPP
#define INTERSECTION_ALG_BSR_HPP

#include <string>
#include <x86intrin.h>
#include <cstdint>

#include "bsrEncoder.hpp"


class IntersectionAlgBsr{
    public:
        IntersectionAlgBsr();

        virtual int intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult) = 0;        
        virtual std::string getName() = 0;            
        void align_malloc(void **memptr, size_t alignment, size_t size);
};

#endif