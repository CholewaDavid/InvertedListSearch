#include "bmissSttni.hpp"

constexpr int word_check_shuffle_a01 = _MM_SHUFFLE(1,1,0,0); 
constexpr int word_check_shuffle_a23 = _MM_SHUFFLE(3,3,2,2); 
constexpr int word_check_shuffle_b01 = _MM_SHUFFLE(1,0,1,0); 
constexpr int word_check_shuffle_b23 = _MM_SHUFFLE(3,2,3,2); 

static const __m128i all_one_si128 = _mm_set_epi32(0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff);

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

static const uint8_t bmiss_sttni_bc_array[32] = {
    0, 1, 4, 5, 8, 9, 12, 13, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 0, 1, 4, 5, 8, 9, 12, 13,
};
static const __m128i *BMISS_BC_ORD = (__m128i*)(bmiss_sttni_bc_array);


BMissSTTNI::BMissSTTNI() : IntersectionAlg(){

}

int BMissSTTNI::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_bmiss_uint_sttni_b8(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string BMissSTTNI::getName(){
    return "BMiss+STTNI";
}

int BMissSTTNI::intersect_bmiss_uint_sttni_b8(int *set_a, int size_a,
        int *set_b, int size_b, int *set_c)
{
    int i = 0, j = 0, size_c = 0;
    int qs_a = size_a - (size_a & 7);
    int qs_b = size_b - (size_b & 7); 

    while (i < qs_a && j < qs_b) {
        __m128i v_a0 = _mm_lddqu_si128((__m128i*)(set_a + i));
        __m128i v_a1 = _mm_lddqu_si128((__m128i*)(set_a + i + 4));
        __m128i v_b0 = _mm_lddqu_si128((__m128i*)(set_b + j));
        __m128i v_b1 = _mm_lddqu_si128((__m128i*)(set_b + j + 4));

        // byte-wise check by STTNI:
        __m128i byte_group_a0 = _mm_shuffle_epi8(v_a0, BMISS_BC_ORD[0]);
        __m128i byte_group_a1 = _mm_shuffle_epi8(v_a1, BMISS_BC_ORD[1]);
        __m128i byte_group_a = _mm_or_si128(byte_group_a0, byte_group_a1);
        __m128i byte_group_b0 = _mm_shuffle_epi8(v_b0, BMISS_BC_ORD[0]);
        __m128i byte_group_b1 = _mm_shuffle_epi8(v_b1, BMISS_BC_ORD[1]);
        __m128i byte_group_b = _mm_or_si128(byte_group_b0, byte_group_b1);

        __m128i bc_mask = _mm_cmpestrm(byte_group_b, 8, byte_group_a, 8,
                _SIDD_UWORD_OPS|_SIDD_CMP_EQUAL_ANY|_SIDD_BIT_MASK);
        int r = _mm_extract_epi32(bc_mask, 0);

        // word-wise check:
        while (r) {
            int p = _mm_popcnt_u32((~r) & (r - 1));
            r &= (r - 1);
            __m128i wc_a = _mm_set_epi32(set_a[i + p], set_a[i + p],
                            set_a[i + p], set_a[i + p]);
            if (!_mm_test_all_zeros(_mm_cmpeq_epi32(wc_a, v_b0),
                                all_one_si128) ||
                !_mm_test_all_zeros(_mm_cmpeq_epi32(wc_a, v_b1),
                                all_one_si128))
                set_c[size_c++] = set_a[i + p];
        }

        if (set_a[i + 7] == set_b[j + 7]) {
            i += 8;
            j += 8;
        } else if (set_a[i + 7] < set_b[j + 7]) {
            i += 8;
        } else {
            j += 8;
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