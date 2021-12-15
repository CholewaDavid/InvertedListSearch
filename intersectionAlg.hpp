#ifndef INTERSECTION_ALG_HPP
#define INTERSECTION_ALG_HPP

#include <string>
#include <x86intrin.h>
#include <cstdint>



class IntersectionAlg{
    public:
        IntersectionAlg();

        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult) = 0;        
        virtual std::string getName() = 0;            
        void align_malloc(void **memptr, size_t alignment, size_t size);
};

#endif