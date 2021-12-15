#ifndef SCALAR_GALLOPING_HPP
#define SCALAR_GALLOPING_HPP

#include "intersectionAlg.hpp"


class ScalarGalloping : public IntersectionAlg{
    public:
        ScalarGalloping();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_scalargalloping_uint(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
};


#endif