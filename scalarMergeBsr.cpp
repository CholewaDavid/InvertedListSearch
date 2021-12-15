#include "scalarMergeBsr.hpp"
#include "bsrEncoder.hpp"


ScalarMergeBsr::ScalarMergeBsr() : IntersectionAlg(){
    
}

int ScalarMergeBsr::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
    int *bases_a, *states_a, *bases_b, *states_b, *bases_c, *states_c;
    int card_a = 0, card_b = 0, card_c = 0;
    align_malloc((void**)&bases_a, 32, sizeof(int) * sizeA);
    align_malloc((void**)&states_a, 32, sizeof(int) * sizeA);
    align_malloc((void**)&bases_b, 32, sizeof(int) * sizeB);
    align_malloc((void**)&states_b, 32, sizeof(int) * sizeB);
    align_malloc((void**)&bases_c, 32, sizeof(int) * std::min(sizeA, sizeB));
    align_malloc((void**)&states_c, 32, sizeof(int) * std::min(sizeA, sizeB));
    card_a = BsrEncoder::offline_uint_trans_bsr(arrayA, sizeA, bases_a, states_a);
    card_b = BsrEncoder::offline_uint_trans_bsr(arrayB, sizeB, bases_b, states_b);

    card_c = this->intersect_scalarmerge_bsr(bases_a, states_a, card_a, bases_b, states_b, card_b, bases_c, states_c);
    int size_c = 0;
    align_malloc((void**)&arrayResult, 32, sizeof(int) * std::min(sizeA, sizeB));

    size_c = BsrEncoder::offline_bsr_trans_uint(bases_c, states_c, card_c, arrayResult);

    free(bases_a);
    free(states_a);
    free(bases_b);
    free(states_b);
    free(bases_c);
    free(states_c);
    return size_c;  
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