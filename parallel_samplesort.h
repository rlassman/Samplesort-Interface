#include "parlay/primitives.h"
#include <parlay/slice.h>
#include "absl/types/span.h"

template <typename T,
          typename BaseCase, // takes data & recursion level, returns bool (if base case triggered or not)
          typename Sampler, // takes data, returns pivot info
          typename Partitioner, // takes data & pivot info, returns bucket info
          typename ParallelSorter> // takes bucket info, pivot info, data & recursion level, sorts bucket recursively (in place)
                                    

class SampleSort {
    public:
        void sort(absl::Span<T> data, int level = 1) {
            auto isSorted = BaseCase::bcsort(data, level);
            if (isSorted)
                return;

            auto pivotInfo = Sampler::sample(data);
            auto bucketInfo = Partitioner::partition(data, pivotInfo);
                
            ParallelSorter::template recSort<T>(bucketInfo, pivotInfo, data, level, *this); 
            
        }
};