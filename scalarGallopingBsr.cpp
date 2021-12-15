#include "scalarGallopingBsr.hpp"
#include "bsrEncoder.hpp"

ScalarGallopingBsr::ScalarGallopingBsr() : IntersectionAlg(){
    
}

int ScalarGallopingBsr::intersect(int* arrayA, int sizeA, int* arrayB, int sizeB, int* arrayResult){
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

    card_c = this->intersect_scalargalloping_bsr(bases_a, states_a, card_a, bases_b, states_b, card_b, bases_c, states_c);
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