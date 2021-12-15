#include "qFilter.hpp"
#include <iostream>

constexpr int cyclic_shift1 = _MM_SHUFFLE(0,3,2,1); //rotating right
constexpr int cyclic_shift2 = _MM_SHUFFLE(2,1,0,3); //rotating left
constexpr int cyclic_shift3 = _MM_SHUFFLE(1,0,3,2); //between

static const __m128i all_zero_si128 = _mm_setzero_si128();
static const __m128i all_one_si128 = _mm_set_epi32(0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff);

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

int* QFilter::prepare_byte_check_mask_dict2()
{
    int * mask = new int[65536];

    auto trans_c_s = [](const int c) -> int {
        switch (c) {
            case 0: return -1; // no match
            case 1: return 0;
            case 2: return 1;
            case 4: return 2;
            case 8: return 3;
            default: return 4; // multiple matches.
        }
    };

    for (int x = 0; x < 65536; ++x) {        
        int c0 = (x & 0xf), c1 = ((x >> 4) & 0xf);
        int c2 = ((x >> 8) & 0xf), c3 = ((x >> 12) & 0xf);
        int s0 = trans_c_s(c0), s1= trans_c_s(c1);
        int s2 = trans_c_s(c2), s3 = trans_c_s(c3);
        
        bool is_multiple_match = (s0 == 4) || (s1 == 4) ||
                (s2 == 4) || (s3 == 4);
        if (is_multiple_match) {
            mask[x] = -1;
            continue;
        }
        bool is_no_match = (s0 == -1) && (s1 == -1) &&
                (s2 == -1) && (s3 == -1);
        if (is_no_match) {
            mask[x] = -2;
            continue;
        }
        if (s0 == -1) s0 = 0; if (s1 == -1) s1 = 1;
        if (s2 == -1) s2 = 2; if (s3 == -1) s3 = 3;
        mask[x] = (s0) | (s1 << 2) | (s2 << 4) | (s3 << 6);        
    }

    return mask;
}
static const int *byte_check_mask_dict = QFilter::prepare_byte_check_mask_dict2();

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

uint8_t * QFilter::prepare_match_shuffle_dict2()
{
    uint8_t * dict = new uint8_t[4096];

    for (int x = 0; x < 256; ++x) {
        for (int i = 0; i < 4; ++i) {
            uint8_t c = (x >> (i << 1)) & 3; // c = 0, 1, 2, 3
            int pos = x * 16 + i * 4;
            for (uint8_t j = 0; j < 4; ++j)
                dict[pos + j] = c * 4 + j;
        }
    }

    return dict;
}
static const __m128i *match_shuffle_dict = (__m128i*)QFilter::prepare_match_shuffle_dict2();

QFilter::QFilter() : IntersectionAlg(){
    
}

int QFilter::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_qfilter_uint_b4_v2(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string QFilter::getName(){
    return "QFilter";
}


//QFilter intersection
int QFilter::intersect_qfilter_uint_b4_v2(int *set_a, int size_a,
        int *set_b, int size_b, int *set_c)
{
    int i = 0, j = 0, size_c = 0;
    int qs_a = size_a - (size_a & 3);
    int qs_b = size_b - (size_b & 3);
 
    __m128i v_a = _mm_load_si128((__m128i*)set_a);
    __m128i v_b = _mm_load_si128((__m128i*)set_b);
    __m128i byte_group_a = _mm_shuffle_epi8(v_a, byte_check_group_a_order[0]);
    __m128i byte_group_b = _mm_shuffle_epi8(v_b, byte_check_group_b_order[0]);
    __m128i cmp_mask;
    while (i < qs_a && j < qs_b) {
        __m128i byte_check_mask = _mm_cmpeq_epi8(byte_group_a, byte_group_b);
        int bc_mask = _mm_movemask_epi8(byte_check_mask);
        int ms_order = byte_check_mask_dict[bc_mask];

        if (__builtin_expect(ms_order != -2, 0)) {
            if (ms_order > 0) {
                 __m128i sf_v_b = _mm_shuffle_epi8(v_b, match_shuffle_dict[ms_order]);
                cmp_mask = _mm_cmpeq_epi32(v_a, sf_v_b);       
            } else {
                __m128i cmp_mask0 = _mm_cmpeq_epi32(v_a, v_b); // pairwise comparison
                __m128i rot1 = _mm_shuffle_epi32(v_b, cyclic_shift1);   // shuffling
                __m128i cmp_mask1 = _mm_cmpeq_epi32(v_a, rot1);
                __m128i rot2 = _mm_shuffle_epi32(v_b, cyclic_shift2);
                __m128i cmp_mask2 = _mm_cmpeq_epi32(v_a, rot2);
                __m128i rot3 = _mm_shuffle_epi32(v_b, cyclic_shift3);
                __m128i cmp_mask3 = _mm_cmpeq_epi32(v_a, rot3);
                cmp_mask = _mm_or_si128(
                        _mm_or_si128(cmp_mask0, cmp_mask1),
                        _mm_or_si128(cmp_mask2, cmp_mask3));
            } 

            int mask = _mm_movemask_ps((__m128)cmp_mask);
            __m128i p = _mm_shuffle_epi8(v_a, shuffle_mask[mask]);
            _mm_storeu_si128((__m128i*)(set_c + size_c), p);

            size_c += _mm_popcnt_u32(mask);           
        }
    
        int a_max = set_a[i + 3];
        int b_max = set_b[j + 3];
        if (a_max <= b_max) {
            i += 4;
            v_a = _mm_load_si128((__m128i*)(set_a + i));
            byte_group_a = _mm_shuffle_epi8(v_a, byte_check_group_a_order[0]);
            _mm_prefetch((char*) (set_a + i + 16), _MM_HINT_T0);
        }
        if (a_max >= b_max) {
            j += 4;
            v_b = _mm_load_si128((__m128i*)(set_b + j));
            byte_group_b = _mm_shuffle_epi8(v_b, byte_check_group_b_order[0]);
            _mm_prefetch((char*) (set_b + j + 16), _MM_HINT_T0);
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