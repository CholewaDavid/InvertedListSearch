#ifndef ROARING_HPP
#define ROARING_HPP

#include "intersectionAlg.hpp"
#include "./roaring/roaring.hh"


class RoaringAlg : public IntersectionAlg{
    public:
        RoaringAlg();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);                     
        virtual std::string getName();    
};


#endif