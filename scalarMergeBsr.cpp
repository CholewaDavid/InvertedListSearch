#include "scalarMergeBsr.hpp"


ScalarMergeBsr::ScalarMergeBsr() : IntersectionAlgBsr(){
    
}

int ScalarMergeBsr::intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult){
    int* array_result_bases;
    int* array_result_states;
    int arrayResultCard, arrayResultSize = 0;

    int sizeMin = std::min(arrays_a->size, arrays_b->size);

    align_malloc((void**)&array_result_bases, 32, sizeof(int) * sizeMin);
    align_malloc((void**)&array_result_states, 32, sizeof(int) * sizeMin);

    arrayResultCard = this->intersect_scalarmerge_bsr(arrays_a->bases, arrays_a->states, arrays_a->card, arrays_b->bases, arrays_b->states, arrays_b->card, array_result_bases, array_result_states);
    arrayResultSize = BsrEncoder::offline_bsr_trans_uint(array_result_bases, array_result_states, arrayResultCard, arrayResult);

    free(array_result_bases);
    free(array_result_states);

    return arrayResultSize;
}

std::string ScalarMergeBsr::getName(){
    return "ScalarMerge+BSR";
}

int ScalarMergeBsr::intersect_scalarmerge_bsr(int* bases_a, int* states_a, int size_a,
        int* bases_b, int* states_b, int size_b,
        int* bases_c, int* states_c)
{
    int i = 0, j = 0, size_c = 0;
    while (i < size_a && j < size_b) {
        if (bases_a[i] == bases_b[j]) {
            states_c[size_c] = states_a[i] & states_b[j];
            if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
            i++; j++;
        } else if (bases_a[i] < bases_b[j]){
            i++;
        } else {
            j++;
        }
    }

    return size_c;
}