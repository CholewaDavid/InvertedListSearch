#include "scalarGallopingBsr.hpp"

ScalarGallopingBsr::ScalarGallopingBsr() : IntersectionAlgBsr(){
    
}

int ScalarGallopingBsr::intersect(BsrArrays* arrays_a, BsrArrays* arrays_b, int* arrayResult){
    int* array_result_bases;
    int* array_result_states;
    int arrayResultCard, arrayResultSize = 0;

    int sizeMin = std::min(arrays_a->size, arrays_b->size);

    align_malloc((void**)&array_result_bases, 32, sizeof(int) * sizeMin);
    align_malloc((void**)&array_result_states, 32, sizeof(int) * sizeMin);

    arrayResultCard = this->intersect_scalargalloping_bsr(arrays_a->bases, arrays_a->states, arrays_a->card, arrays_b->bases, arrays_b->states, arrays_b->card, array_result_bases, array_result_states);
    int size_c = 0;

    align_malloc((void**)&arrayResult, 32, sizeof(int) * sizeMin);

    arrayResultSize = BsrEncoder::offline_bsr_trans_uint(array_result_bases, array_result_states, arrayResultCard, arrayResult);

    free(array_result_bases);
    free(array_result_states);

    return arrayResultSize;
}

std::string ScalarGallopingBsr::getName(){
    return "ScalarGalloping+BSR";
}

int ScalarGallopingBsr::intersect_scalargalloping_bsr(int* bases_a, int* states_a, int size_a,
            int* bases_b, int* states_b, int size_b,
            int* bases_c, int* states_c)
{
    if (size_a > size_b)
        return intersect_scalargalloping_bsr(bases_b, states_b, size_b,
                bases_a, states_a, size_a,
                bases_c, states_c);

    int j = 0, size_c = 0;
    for (int i = 0; i < size_a; ++i) {
        // double-jump:
        int r = 1;
        while (j + r < size_b && bases_a[i] > bases_b[j + r]) r <<= 1;
        // binary search:
        int right = (j + r < size_b) ? (j + r) : (size_b - 1);
        if (bases_b[right] < bases_a[i]) break;        
        int left = j + (r >> 1);
        while (left < right) {
            int mid = (left + right) >> 1;
            if (bases_b[mid] >= bases_a[i]) right = mid;
            else left = mid + 1;
        }
        j = left;

        if (bases_a[i] == bases_b[j]) {
            states_c[size_c] = states_a[i] & states_b[j];
            if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
        }
    }

    return size_c;
}