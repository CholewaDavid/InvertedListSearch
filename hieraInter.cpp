#include "hieraInter.hpp"

HieraInter::HieraInter() : IntersectionAlg(){
    shuffle_mask16 = (__m128i *)prepare_shuffling_dict_u16();
}

int HieraInter::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    return this->intersect_hierainter_uint_sttni(arrayA, sizeA, arrayB, sizeB, arrayResult);
}

std::string HieraInter::getName(){
    return "HieraInter";
}

int HieraInter::intersect_hierainter_uint_sttni(int *set_a, int size_a, int *set_b, int size_b, int *set_c){
    uint16_t *hi_set_a = NULL, *hi_set_b = NULL, *hi_set_c = NULL;
    align_malloc((void**)&hi_set_a, 32, sizeof(uint16_t) * size_a * 3);
    align_malloc((void**)&hi_set_b, 32, sizeof(uint16_t) * size_b * 3);
    align_malloc((void**)&hi_set_c, 32, sizeof(uint16_t) * std::min(size_a, size_b) * 3);
    int hi_size_a = hierainter_offline_partition(set_a, size_a, hi_set_a);
    int hi_size_b = hierainter_offline_partition(set_b, size_b, hi_set_b);

    int hi_size_c = hierainter_online_intersect_high16bit(hi_set_a, hi_size_a,
                hi_set_b, hi_size_b, hi_set_c);

    int size_c = hierainter_offline_combine(hi_set_c, hi_size_c, set_c);

    free(hi_set_a);
    free(hi_set_b);
    free(hi_set_c);
    return size_c;
}

int HieraInter::hierainter_offline_partition(int *set_a, int size_a, uint16_t *hi_set)
{
    uint16_t high = 0;
    int partition_len = 0, partition_pos = 1;
    int hi_size = 0;

    for (int i = 0; i < size_a; ++i) {
        uint16_t chigh = (uint16_t)(set_a[i] >> 16); // upper 16-bit part
        uint16_t clow = (uint16_t)(set_a[i] & 0xffff); // lower 16-bit part
        if (chigh == high && i != 0) { // add element to the current partition
            hi_set[hi_size++] = clow;
            partition_len++;
        } else { // start new partition
            hi_set[hi_size++] = chigh; // partition prefix
            hi_set[hi_size++] = 0;     // reserve place for partition size
            hi_set[hi_size++] = clow;  // write the first element
            hi_set[partition_pos] = partition_len;
            partition_len = 1; // reset counters
            partition_pos = hi_size - 2;
            high = chigh;            
        }
    }
    hi_set[partition_pos] = partition_len;

    return hi_size;
}

int HieraInter::hierainter_offline_combine(uint16_t *hi_set, int hi_size, int *set_a)
{
    int i = 0, size_a = 0;
    while (i < hi_size) {
        int chigh = (int)hi_set[i++]; chigh <<= 16;
        int par_len = (int)hi_set[i++];
        for (int j = 0; j < par_len; ++j) {
            int clow = (int)hi_set[i + j];
            set_a[size_a++] = (chigh | clow);
        }
        i += par_len;
    }
    return size_a;
}

int HieraInter::hierainter_online_intersect_high16bit(uint16_t *par_a, int size_a,
        uint16_t *par_b, int size_b, uint16_t *par_c)
{
    int i = 0, j = 0, size_c = 0;

    while (i < size_a && j < size_b) {
        if (par_a[i] < par_b[j]) {
            i += par_a[i + 1] + 2;
        } else if (par_b[j] < par_a[i]) {
            j += par_b[j + 1] + 2;
        } else {
            par_c[size_c++] = par_a[i];  // write partition prefix            
            uint16_t partition_len = hierainter_online_intersect_low16bit(
                    par_a + i + 2, par_a[i + 1], par_b + j + 2, par_b[j + 1],
                    par_c + size_c + 1);
            par_c[size_c++] = partition_len;
            size_c += partition_len;
            i += par_a[i + 1] + 2;
            j += par_b[j + 1] + 2;
        }
    }
    return size_c;
}

int HieraInter::hierainter_online_intersect_low16bit(uint16_t *set_a, int size_a,
        uint16_t *set_b, int size_b, uint16_t *set_c)
{
    int i = 0, j = 0, size_c = 0;
    int qs_a = size_a - (size_a & 7);
    int qs_b = size_b - (size_b & 7);

    while (i < qs_a && j < qs_b) {
        __m128i v_a = _mm_lddqu_si128((__m128i*)(set_a + i));
        __m128i v_b = _mm_lddqu_si128((__m128i*)(set_b + j));
 
        __m128i res_v = _mm_cmpestrm(v_b, 8, v_a, 8,
                _SIDD_UWORD_OPS|_SIDD_CMP_EQUAL_ANY|_SIDD_BIT_MASK);
        int r = _mm_extract_epi32(res_v, 0);
        __m128i p = _mm_shuffle_epi8(v_a, shuffle_mask16[r]);
        _mm_storeu_si128((__m128i*)(set_c + size_c), p);
        size_c += _mm_popcnt_u32(r);
 
        uint16_t a_max = _mm_extract_epi16(v_a, 7);
        uint16_t b_max = _mm_extract_epi16(v_b, 7);
        i += (a_max <= b_max) * 8;
        j += (a_max >= b_max) * 8;
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

uint8_t* HieraInter::prepare_shuffling_dict_u16()
{
    uint8_t* mask = new uint8_t[4096];
    memset(mask, 0xff, sizeof(uint8_t) * 4096);
    int size = 0;
    for (int i = 0; i < 256; ++i) {
        int counter = 0;
        for (int j = 0; j < 8; ++j) {
            if (i & (1 << j)) {
                mask[size + counter * 2    ] = 2 * j;
                mask[size + counter * 2 + 1] = 2 * j + 1;
                ++counter;             
             }              
        }
        size += 16;
    }
    return mask;
}