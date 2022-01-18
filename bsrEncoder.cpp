#include "bsrEncoder.hpp"
#include <iostream>


const int BSR_WIDTH = sizeof(int) * 8;
const int BSR_SHIFT = __builtin_ctzll(BSR_WIDTH);
const int BSR_MASK = BSR_WIDTH - 1;

int BsrEncoder::offline_uint_trans_bsr(int *set_a, int size_a, int *bases_a, int *states_a)
{
    int cnt = -1;
    for (int i = 0; i < size_a; ++i) {
        int u = set_a[i];
        int u_base = (u >> BSR_SHIFT);
        int u_bit = (1 << (u & BSR_MASK));
        if (cnt == -1 || bases_a[cnt] != u_base) {
            bases_a[++cnt] = u_base;
            states_a[cnt] = u_bit;
        } else {
            states_a[cnt] |= u_bit;
        }
    }
    return ++cnt;    
}

int BsrEncoder::offline_bsr_trans_uint(int *bases_a, int *states_a, int size_a, int *set_a)
{
    int cnt = 0;
    for (int i = 0; i < size_a; ++i) {
        int u_high = (bases_a[i] << BSR_SHIFT);
        int state = states_a[i];
        while (state) {
            int u = (u_high | __builtin_ctz(state));
            set_a[cnt++] = u;
            state &= (state - 1);
        }
    }
    return cnt;
}

void BsrEncoder::align_malloc(void **memptr, size_t alignment, size_t size)
{
    int malloc_flag = posix_memalign(memptr, alignment, size);
}

BsrArrays* BsrEncoder::generateBsrArrays(int* array_a, int size_a){
    BsrArrays* output = new BsrArrays();
    align_malloc((void**)&(output->bases), 32, sizeof(int) * size_a);
    align_malloc((void**)&(output->states), 32, sizeof(int) * size_a);
    output->card = BsrEncoder::offline_uint_trans_bsr(array_a, size_a, output->bases, output->states);
    output->size = size_a;
    return output;
}

BsrArrays::BsrArrays(){

}

BsrArrays::BsrArrays(BsrArrays* prototype){
    this->size = prototype->size;
    this->card = prototype->card;
    
    BsrEncoder::align_malloc((void**)&(this->bases), 32, sizeof(int)*(this->size));
    BsrEncoder::align_malloc((void**)&(this->states), 32, sizeof(int)*(this->size));
    for(int i = 0; i < this->size; i++){
        this->bases[i] = prototype->bases[i];
        this->states[i] = prototype->states[i];
    }
}

BsrArrays::~BsrArrays(){
    free(this->bases);
    free(this->states);
}