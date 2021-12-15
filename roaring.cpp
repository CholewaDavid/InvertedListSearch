#include "roaring.hpp"
#include "./roaring/roaring.hh"


RoaringAlg::RoaringAlg() : IntersectionAlg(){

}

int RoaringAlg::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    Roaring roar_a(sizeA, (const uint32_t*)arrayA);
    Roaring roar_b(sizeB, (const uint32_t*)arrayB);
    // roar_a.runOptimize(); roar_a.shrinkToFit();
    // roar_b.runOptimize(); roar_b.shrinkToFit();
    Roaring roar_c;
    roar_c = roar_a & roar_b;

    int size_c = roar_c.cardinality();
    roar_c.toUint32Array((uint32_t*)arrayResult);
    return size_c;
}

std::string RoaringAlg::getName(){
    return "Roaring";
}