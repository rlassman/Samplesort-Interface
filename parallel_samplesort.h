#include "parlay/primitives.h"
#include <parlay/slice.h>

template <typename T,
          typename BaseCase, //takes data, returns bool (if base case triggered or not)
          typename Sampler, //takes data, returns pivots
          typename Partitioner, //takes data & pivots, returns buckets
          typename ParallelSorter> //takes bucket, sorts bucket recursively (in place)

class SampleSort {
    public:
        void sort(parlay::slice<T*, T*> data, int level = 1) { 
            auto isSorted = BaseCase::bcsort(data, level);
            if (isSorted)
                return;

            auto pivotInfo = Sampler::sample(data);
            auto bucketInfo = Partitioner::partition(data, pivotInfo);
                
            ParallelSorter::template recSort<T>(bucketInfo, pivotInfo, level, *this);
            
        }
};