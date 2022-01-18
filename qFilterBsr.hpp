#ifndef QFILTER_BSR_HPP
#define QFILTER_BSR_HPP

#include "intersectionAlgBsr.hpp"


class QFilterBsr : public IntersectionAlgBsr{
    public: 
        QFilterBsr();
        virtual int intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult);               
        virtual std::string getName();
        static int* prepare_byte_check_mask_dict2();
        static uint8_t * prepare_match_shuffle_dict2();
    private:
        int intersect_qfilter_bsr_b4_v2(int* bases_a, int* states_a, int size_a, int* bases_b, int* states_b, int size_b, int* bases_c, int* states_c);
};


#endif