#ifndef SCALAR_MERGE_BSR_HPP
#define SCALAR_MERGE_BSR_HPP

#include "intersectionAlgBsr.hpp"

class ScalarMergeBsr : public IntersectionAlgBsr{
    public:
        ScalarMergeBsr();
        virtual int intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult);               
        virtual std::string getName();

    private:
        int intersect_scalarmerge_bsr(int* bases_a, int* states_a, int size_a,
        int* bases_b, int* states_b, int size_b,
        int* bases_c, int* states_c);
};

#endif