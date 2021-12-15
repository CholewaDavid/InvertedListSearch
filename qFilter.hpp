#ifndef QFILTER_HPP
#define QFILTER_HPP

#include "intersectionAlg.hpp"

class QFilter : public IntersectionAlg{
    public: 
        QFilter();
        virtual int intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult);               
        virtual std::string getName();

        static uint8_t * prepare_match_shuffle_dict2(); 
        static int* prepare_byte_check_mask_dict2();
    private:           
        int intersect_qfilter_uint_b4_v2(int *set_a, int size_a, int *set_b, int size_b, int *set_c);        
};

#endif