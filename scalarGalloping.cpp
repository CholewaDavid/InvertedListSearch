#include "scalarGalloping.hpp"


ScalarGalloping::ScalarGalloping() : IntersectionAlg(){

}

int ScalarGalloping::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_scalargalloping_uint(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string ScalarGalloping::getName(){
    return "ScalarGalloping";
}

int ScalarGalloping::intersect_scalargalloping_uint(int *set_a, int size_a,
        int *set_b, int size_b, int *set_c)
{
    if (size_a > size_b)
        return intersect_scalargalloping_uint(set_b, size_b, set_a, size_a, set_c);
    
    int j = 0, size_c = 0;    
    for (int i = 0; i < size_a; ++i) {
        // double-jump:
        int r = 1;
        while (j + r < size_b && set_a[i] > set_b[j + r]) r <<= 1;
        // binary search:
        int right = (j + r < size_b) ? (j + r) : (size_b - 1);
        if (set_b[right] < set_a[i]) break;        
        int left = j + (r >> 1);
        while (left < right) {
            int mid = (left + right) >> 1;
            if (set_b[mid] >= set_a[i]) right = mid;
            else left = mid + 1;
        }
        j = left;

        if (set_a[i] == set_b[j]) set_c[size_c++] = set_a[i];
    }

    return size_c;
}