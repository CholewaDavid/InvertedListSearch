#ifndef BSR_ENCODER_HPP_
#define BSR_ENCODER_HPP


class BsrEncoder{
    public:
        static int offline_uint_trans_bsr(int *set_a, int size_a, int *bases_a, int *states_a);
        static int offline_bsr_trans_uint(int *bases_a, int *states_a, int size_a, int *set_a);
};


#endif