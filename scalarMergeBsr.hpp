#ifndef SCALAR_MERGE_BSR_HPP
#define SCALAR_MERGE_BSR_HPP

#include "intersectionAlg.hpp"

class ScalarMergeBsr : public IntersectionAlg{
    public:
        ScalarMergeBsr();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();

    private:
        int intersect_scalarmerge_bsr(int* bases_a, int* states_a, int size_a,
        int* bases_b, int* states_b, int size_b,
        int* bases_c, int* states_c);
};

#endif