#ifndef BMISS_HPP
#define BMISS_HPP

#include "intersectionAlg.hpp"


class BMiss : public IntersectionAlg{
    public:
        BMiss();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_bmiss_uint_b4(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
};


#endif