#include "bsrEncoder.hpp"


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