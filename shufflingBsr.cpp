#include "shufflingBsr.hpp"

constexpr int cyclic_shift1 = _MM_SHUFFLE(0,3,2,1); //rotating right
constexpr int cyclic_shift2 = _MM_SHUFFLE(2,1,0,3); //rotating left
constexpr int cyclic_shift3 = _MM_SHUFFLE(1,0,3,2); //between

static const __m128i all_zero_si128 = _mm_setzero_si128();

static const uint8_t shuffle_pi8_array[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    4, 5, 6, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 4, 5, 6, 7, 255, 255, 255, 255, 255, 255, 255, 255, 
    8, 9, 10, 11, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 8, 9, 10, 11, 255, 255, 255, 255, 255, 255, 255, 255, 
    4, 5, 6, 7, 8, 9, 10, 11, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 255, 255, 255, 255, 
    12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 
    4, 5, 6, 7, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 14, 15, 255, 255, 255, 255, 
    8, 9, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 
    0, 1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
};
static const __m128i *shuffle_mask = (__m128i*)(shuffle_pi8_array);



ShufflingBsr::ShufflingBsr() : IntersectionAlgBsr(){
    
}

int ShufflingBsr::intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult){
    int* array_result_bases;
    int* array_result_states;
    int arrayResultCard, arrayResultSize = 0;

    int sizeMin = std::min(arrays_a->size, arrays_b->size);

    align_malloc((void**)&array_result_bases, 32, sizeof(int) * sizeMin);
    align_malloc((void**)&array_result_states, 32, sizeof(int) * sizeMin);

    arrayResultCard = this->intersect_shuffle_bsr_b4(arrays_a->bases, arrays_a->states, arrays_a->card, arrays_b->bases, arrays_b->states, arrays_b->card, array_result_bases, array_result_states);
    arrayResultSize = BsrEncoder::offline_bsr_trans_uint(array_result_bases, array_result_states, arrayResultCard, arrayResult);

    free(array_result_bases);
    free(array_result_states);

    return arrayResultSize;
}

std::string ShufflingBsr::getName(){
    return "Shuffling+BSR";
}

int ShufflingBsr::intersect_shuffle_bsr_b4(int* bases_a, int* states_a, int size_a,
            int* bases_b, int* states_b, int size_b,
            int* bases_c, int* states_c)
{
    int i = 0, j = 0, size_c = 0;
    int qs_a = size_a - (size_a & 3);
    int qs_b = size_b - (size_b & 3);
    
    while (i < qs_a && j < qs_b) {
        __m128i base_a = _mm_lddqu_si128((__m128i*)(bases_a + i));
        __m128i base_b = _mm_lddqu_si128((__m128i*)(bases_b + j));
        __m128i state_a = _mm_lddqu_si128((__m128i*)(states_a + i));
        __m128i state_b = _mm_lddqu_si128((__m128i*)(states_b + j));
        
        int a_max = bases_a[i + 3];
        int b_max = bases_b[j + 3];
        if (a_max == b_max) {
            i += 4;
            j += 4;
            _mm_prefetch((char*) (bases_a + i), _MM_HINT_NTA);
            _mm_prefetch((char*) (states_a + i), _MM_HINT_NTA);
            _mm_prefetch((char*) (bases_b + j), _MM_HINT_NTA);
            _mm_prefetch((char*) (states_b + j), _MM_HINT_NTA);
        } else if (a_max < b_max) {
            i += 4;
            _mm_prefetch((char*) (bases_a + i), _MM_HINT_NTA);
            _mm_prefetch((char*) (states_a + i), _MM_HINT_NTA);
        } else {
            j += 4;
            _mm_prefetch((char*) (bases_b + j), _MM_HINT_NTA);
            _mm_prefetch((char*) (states_b + j), _MM_HINT_NTA);
        }        

        // shift0:
        __m128i cmp_mask0 = _mm_cmpeq_epi32(base_a, base_b);
        __m128i state_c0 = _mm_and_si128(
                _mm_and_si128(state_a, state_b), cmp_mask0);

        // shift1:
        __m128i base_sf1 = _mm_shuffle_epi32(base_b, cyclic_shift1);
        __m128i state_sf1 = _mm_shuffle_epi32(state_b, cyclic_shift1);
        __m128i cmp_mask1 = _mm_cmpeq_epi32(base_a, base_sf1);
        __m128i state_c1 = _mm_and_si128(
                _mm_and_si128(state_a, state_sf1), cmp_mask1);

        // shift2:
        __m128i base_sf2 = _mm_shuffle_epi32(base_b, cyclic_shift2);
        __m128i state_sf2 = _mm_shuffle_epi32(state_b, cyclic_shift2);
        __m128i cmp_mask2 = _mm_cmpeq_epi32(base_a, base_sf2);
        __m128i state_c2 = _mm_and_si128(
                _mm_and_si128(state_a, state_sf2), cmp_mask2);

        // shift3:
        __m128i base_sf3 = _mm_shuffle_epi32(base_b, cyclic_shift3);
        __m128i state_sf3 = _mm_shuffle_epi32(state_b, cyclic_shift3);
        __m128i cmp_mask3 = _mm_cmpeq_epi32(base_a, base_sf3);
        __m128i state_c3 = _mm_and_si128(
                _mm_and_si128(state_a, state_sf3), cmp_mask3);

        __m128i state_all = _mm_or_si128(
                _mm_or_si128(state_c0, state_c1),
                _mm_or_si128(state_c2, state_c3)
                );
        __m128i cmp_mask = _mm_or_si128(
                _mm_or_si128(cmp_mask0, cmp_mask1),
                _mm_or_si128(cmp_mask2, cmp_mask3)
                );
        __m128i state_mask = _mm_cmpeq_epi32(state_all, all_zero_si128);
        int mask = (_mm_movemask_ps((__m128)cmp_mask) &
                ~(_mm_movemask_ps((__m128)state_mask)));

        __m128i res_b = _mm_shuffle_epi8(base_a, shuffle_mask[mask]);
        __m128i res_s = _mm_shuffle_epi8(state_all, shuffle_mask[mask]);
        _mm_storeu_si128((__m128i*)(bases_c + size_c), res_b);
        _mm_storeu_si128((__m128i*)(states_c + size_c), res_s);

        size_c += _mm_popcnt_u32(mask);
    }

    while (i < size_a && j < size_b) {
        if (bases_a[i] == bases_b[j]) {
            bases_c[size_c] = bases_a[i];
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