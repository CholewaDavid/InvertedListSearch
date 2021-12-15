#ifndef BMISS_STTNI_HPP
#define BMISS_STTNI_HPP

#include "intersectionAlg.hpp"


class BMissSTTNI : public IntersectionAlg{
    public:
        BMissSTTNI();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_bmiss_uint_sttni_b8(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
};


#endif