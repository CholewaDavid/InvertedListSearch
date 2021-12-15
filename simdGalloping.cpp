#include "simdGalloping.hpp"


SimdGalloping::SimdGalloping() : IntersectionAlg(){

}

int SimdGalloping::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_simdgalloping_uint(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string SimdGalloping::getName(){
    return "SIMDGalloping";
}

int SimdGalloping::intersect_simdgalloping_uint(int *set_a, int size_a,
        int *set_b, int size_b, int *set_c)
{
    if (size_a > size_b)
        return intersect_simdgalloping_uint(set_b, size_b, set_a, size_a, set_c);

    int i = 0, j = 0, size_c = 0;
    int qs_b = size_b - (size_b & 3);
    for (i = 0; i < size_a; ++i) {
        // double-jump:
        int r = 1;
        while (j + (r << 2) < qs_b && set_a[i] > set_b[j + (r << 2) + 3]) r <<= 1;
        // binary search:
        int upper = (j + (r << 2) < qs_b) ? (r) : ((qs_b - j - 4) >> 2);
        if (set_b[j + (upper << 2) + 3] < set_a[i]) break;        
        int lower = (r >> 1);
        while (lower < upper) {
            int mid = (lower + upper) >> 1;
            if (set_b[j + (mid << 2) + 3] >= set_a[i]) upper = mid;
            else lower = mid + 1;
        }
        j += (lower << 2);

        __m128i v_a = _mm_set_epi32(set_a[i], set_a[i], set_a[i], set_a[i]);
        __m128i v_b = _mm_lddqu_si128((__m128i*)(set_b + j));
        __m128i cmp_mask = _mm_cmpeq_epi32(v_a, v_b);
        int mask = _mm_movemask_ps((__m128)cmp_mask);
        if (mask != 0) set_c[size_c++] = set_a[i];
    }

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