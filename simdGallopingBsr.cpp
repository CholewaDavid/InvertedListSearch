#include "simdGallopingBsr.hpp"


SimdGallopingBsr::SimdGallopingBsr() : IntersectionAlgBsr(){

}

int SimdGallopingBsr::intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult){
    int* array_result_bases;
    int* array_result_states;
    int arrayResultCard, arrayResultSize = 0;

    int sizeMin = std::min(arrays_a->size, arrays_b->size);

    align_malloc((void**)&array_result_bases, 32, sizeof(int) * sizeMin);
    align_malloc((void**)&array_result_states, 32, sizeof(int) * sizeMin);

    arrayResultCard = this->intersect_simdgalloping_bsr(arrays_a->bases, arrays_a->states, arrays_a->card, arrays_b->bases, arrays_b->states, arrays_b->card, array_result_bases, array_result_states);
    arrayResultSize = BsrEncoder::offline_bsr_trans_uint(array_result_bases, array_result_states, arrayResultCard, arrayResult);

    free(array_result_bases);
    free(array_result_states);

    return arrayResultSize;   
}

std::string SimdGallopingBsr::getName(){
    return "SIMDGalloping+BSR";
}

int SimdGallopingBsr::intersect_simdgalloping_bsr(int* bases_a, int* states_a, int size_a,
        int* bases_b, int* states_b, int size_b,
        int* bases_c, int* states_c)
{
    if (size_a > size_b)
        return intersect_simdgalloping_bsr(bases_b, states_b, size_b,
                bases_a, states_a, size_a,
                bases_c, states_c);

    int i = 0, j = 0, size_c = 0;
    int qs_b = size_b - (size_b & 3);
    for (i = 0; i < size_a; ++i) {
        // double-jump:
        int r = 1;
        while (j + (r << 2) < qs_b && bases_a[i] > bases_b[j + (r << 2) + 3]) r <<= 1;
        // binary search:
        int upper = (j + (r << 2) < qs_b) ? (r) : ((qs_b - j - 4) >> 2);
        if (bases_b[j + (upper << 2) + 3] < bases_a[i]) break;        
        int lower = (r >> 1);
        while (lower < upper) {
            int mid = (lower + upper) >> 1;
            if (bases_b[j + (mid << 2) + 3] >= bases_a[i]) upper = mid;
            else lower = mid + 1;
        }
        j += (lower << 2);

        __m128i bv_a = _mm_set_epi32(bases_a[i], bases_a[i], bases_a[i], bases_a[i]);
        __m128i bv_b = _mm_lddqu_si128((__m128i*)(bases_b + j));
        __m128i cmp_mask = _mm_cmpeq_epi32(bv_a, bv_b);
        int mask = _mm_movemask_ps((__m128)cmp_mask);
        if (mask != 0) {
            int p = __builtin_ctz(mask);
            states_c[size_c] = states_a[i] & states_b[j + p];
            if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
        }
    }

    while (i < size_a && j < size_b) {
        if (bases_a[i] == bases_b[j]) {
            states_c[size_c] = states_a[i] & states_b[j];
            if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
            i++; j++;
        } else if (bases_a[i] < bases_b[j]){
            i++;
        } else {
            j++;
        }
    }

    return size_c;
}