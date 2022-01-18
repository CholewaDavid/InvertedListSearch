#ifndef SCALAR_GALLOPING_BSR_HPP
#define SCALAR_GALLOPING_BSR_HPP

#include "intersectionAlgBsr.hpp"


class ScalarGallopingBsr : public IntersectionAlgBsr{
    public:
        ScalarGallopingBsr();
        virtual int intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_scalargalloping_bsr(int* bases_a, int* states_a, int size_a, int* bases_b, int* states_b, int size_b, int* bases_c, int* states_c);
};


#endif