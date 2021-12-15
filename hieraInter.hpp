#ifndef HIERA_INTER_HPP
#define HIERA_INTER_HPP

#include <cstring>
#include "intersectionAlg.hpp"


class HieraInter : public IntersectionAlg{
    public:
        HieraInter();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
    private:
        int intersect_hierainter_uint_sttni(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
        int hierainter_offline_partition(int *set_a, int size_a, uint16_t *hi_set);
        int hierainter_offline_combine(uint16_t *hi_set, int hi_size, int *set_a);
        int hierainter_online_intersect_high16bit(uint16_t *par_a, int size_a, uint16_t *par_b, int size_b, uint16_t *par_c);
        int hierainter_online_intersect_low16bit(uint16_t *set_a, int size_a, uint16_t *set_b, int size_b, uint16_t *set_c);
        uint8_t* prepare_shuffling_dict_u16();
        __m128i *shuffle_mask16;
};


#endif