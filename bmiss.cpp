#include "bmiss.hpp"

constexpr int word_check_shuffle_a01 = _MM_SHUFFLE(1,1,0,0); 
constexpr int word_check_shuffle_a23 = _MM_SHUFFLE(3,3,2,2); 
constexpr int word_check_shuffle_b01 = _MM_SHUFFLE(1,0,1,0); 
constexpr int word_check_shuffle_b23 = _MM_SHUFFLE(3,2,3,2); 

static const uint8_t byte_check_group_a_pi8[64] = {
    0, 0, 0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12,
    1, 1, 1, 1, 5, 5, 5, 5, 9, 9, 9, 9, 13, 13, 13, 13,
    2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 14, 14, 14, 14,
    3, 3, 3, 3, 7, 7, 7, 7, 11, 11, 11, 11, 15, 15, 15, 15,
};
static const uint8_t byte_check_group_b_pi8[64] = {
    0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12,
    1, 5, 9, 13, 1, 5, 9, 13, 1, 5, 9, 13, 1, 5, 9, 13,
    2, 6, 10, 14, 2, 6, 10, 14, 2, 6, 10, 14, 2, 6, 10, 14,
    3, 7, 11, 15, 3, 7, 11, 15, 3, 7, 11, 15, 3, 7, 11, 15,
};
static const __m128i *byte_check_group_a_order = (__m128i*)(byte_check_group_a_pi8);
static const __m128i *byte_check_group_b_order = (__m128i*)(byte_check_group_b_pi8);

BMiss::BMiss() : IntersectionAlg(){

}

int BMiss::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_bmiss_uint_b4(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string BMiss::getName(){
    return "BMiss";
}

int BMiss::intersect_bmiss_uint_b4(int *set_a, int size_a,
        int *set_b, int size_b, int *set_c)
{
    int i = 0, j = 0, size_c = 0;
    int qs_a = size_a - (size_a & 3);
    int qs_b = size_b - (size_b & 3);

    while (i < qs_a && j < qs_b) {
        __m128i v_a = _mm_lddqu_si128((__m128i*)(set_a + i));
        __m128i v_b = _mm_lddqu_si128((__m128i*)(set_b + j));

        // byte-wise check:
        __m128i byte_group_a0 = _mm_shuffle_epi8(v_a, byte_check_group_a_order[0]);
        __m128i byte_group_b0 = _mm_shuffle_epi8(v_b, byte_check_group_b_order[0]);
        __m128i byte_check_mask0 = _mm_cmpeq_epi8(byte_group_a0, byte_group_b0);
        int bc_mask0 = _mm_movemask_epi8(byte_check_mask0);
        __m128i byte_group_a1 = _mm_shuffle_epi8(v_a, byte_check_group_a_order[1]);
        __m128i byte_group_b1 = _mm_shuffle_epi8(v_b, byte_check_group_b_order[1]);
        __m128i byte_check_mask1 = _mm_cmpeq_epi8(byte_group_a1, byte_group_b1);
        int bc_mask1 = _mm_movemask_epi8(byte_check_mask1);
        if ((bc_mask0 & bc_mask1) == 0) {
            if (set_a[i + 3] == set_b[j + 3]) {
                i += 4;
                j += 4;
            } else if (set_a[i + 3] < set_b[j + 3]) {
                i += 4;
            } else {
                j += 4;
            }
            continue;
        }

        // word-wise check:
        __m128i word_group_a01 = _mm_shuffle_epi32(v_a, word_check_shuffle_a01);
        __m128i word_group_b01 = _mm_shuffle_epi32(v_b, word_check_shuffle_b01);
        __m128i word_group_a23 = _mm_shuffle_epi32(v_a, word_check_shuffle_a23);
        __m128i word_group_b23 = _mm_shuffle_epi32(v_b, word_check_shuffle_b23);

        __m128i work_check_mask0101 = _mm_cmpeq_epi32(word_group_a01, word_group_b01);
        __m128i work_check_mask0123 = _mm_cmpeq_epi32(word_group_a01, word_group_b23);
        __m128i work_check_mask01 = _mm_or_si128(work_check_mask0101, work_check_mask0123);

        __m128i work_check_mask2301 = _mm_cmpeq_epi32(word_group_a23, word_group_b01);
        __m128i work_check_mask2323 = _mm_cmpeq_epi32(word_group_a23, word_group_b23);
        __m128i work_check_mask23 = _mm_or_si128(work_check_mask2301, work_check_mask2323);

        int wc_mask01 = _mm_movemask_ps((__m128)work_check_mask01);
        if ((wc_mask01 & 0x3) != 0) set_c[size_c++] = set_a[i];
        if ((wc_mask01 & 0xc) != 0) set_c[size_c++] = set_a[i + 1];
        int wc_mask23 = _mm_movemask_ps((__m128)work_check_mask23);
        if ((wc_mask23 & 0x3) != 0) set_c[size_c++] = set_a[i + 2];
        if ((wc_mask23 & 0xc) != 0) set_c[size_c++] = set_a[i + 3];

        if (set_a[i + 3] == set_b[j + 3]) {
            i += 4;
            j += 4;
        } else if (set_a[i + 3] < set_b[j + 3]) {
            i += 4;
        } else {
            j += 4;
        }
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