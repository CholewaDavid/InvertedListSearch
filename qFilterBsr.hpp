#ifndef QFILTER_BSR_HPP
#define QFILTER_BSR_HPP

#include "intersectionAlg.hpp"


class QFilterBsr : public IntersectionAlg{
    public: 
        QFilterBsr();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();
        static int* prepare_byte_check_mask_dict2();
        static uint8_t * prepare_match_shuffle_dict2();
    private:
        int intersect_qfilter_bsr_b4_v2(int* bases_a, int* states_a, int size_a, int* bases_b, int* states_b, int size_b, int* bases_c, int* states_c);
};


#endif