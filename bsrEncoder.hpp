#ifndef BSR_ENCODER_HPP_
#define BSR_ENCODER_HPP

#include <x86intrin.h>
#include <cstdint>


class BsrArrays{
    public:
        BsrArrays();
        BsrArrays(BsrArrays* prototype);
        ~BsrArrays();

        int* bases;
        int* states;
        int card;
        int size;
};

class BsrEncoder{
    public:
        static int offline_uint_trans_bsr(int *set_a, int size_a, int *bases_a, int *states_a);
        static int offline_bsr_trans_uint(int *bases_a, int *states_a, int size_a, int *set_a);
        static void align_malloc(void **memptr, size_t alignment, size_t size);
        static BsrArrays* generateBsrArrays(int* array_a, int size_a);
};


#endif