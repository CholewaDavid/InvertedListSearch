#include "intersectionAlg.hpp"

IntersectionAlg::IntersectionAlg(){    
}


void IntersectionAlg::align_malloc(void **memptr, size_t alignment, size_t size)
{
    int malloc_flag = posix_memalign(memptr, alignment, size);
}