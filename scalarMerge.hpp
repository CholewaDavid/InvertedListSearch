#ifndef SCALAR_MERGE_HPP
#define SCALAR_MERGE_HPP

#include "intersectionAlg.hpp"

class ScalarMerge : public IntersectionAlg{
    public:
        ScalarMerge();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();

    private:
        int intersect_scalarmerge_uint(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
};

#endif