#include "scalarMerge.hpp"

ScalarMerge::ScalarMerge() : IntersectionAlg(){
    
}

int ScalarMerge::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_scalarmerge_uint(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string ScalarMerge::getName(){
    return "ScalarMerge";
}

int ScalarMerge::intersect_scalarmerge_uint(int *set_a, int size_a, int *set_b, int size_b, int *set_c)
{
    int i = 0, j = 0, size_c = 0;
    while (i < size_a && j < size_b) {
        if (set_a[i] == set_b[j]) {
            set_c[size_c++] = set_a[i];
            i++; j++;
        } else if (set_a[i] < set_b[j]) {
            i++;
        } else {
            j++;
        }
    }

    return size_c;    
}