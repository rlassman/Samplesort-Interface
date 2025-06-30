#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/slice.h>
#include "ParlaySampleSortPartition.h"

/*
else logic from parlay's samplesort doesn't work, not sure why. did parallel loop to copy at end instead 
*/

struct ParlaySampleSortRec {
    template <typename T, typename Sorter>
    static void recSort(BucketInfo<T>& bucketInfo, const PivotInfo<T>& pivotInfo, absl::Span<T> in, int level, Sorter& sorter) {
        T* keys = bucketInfo.keys.begin();
        //auto keys = bucketInfo.keys;
        const auto& offsets = bucketInfo.offsets;
        const auto& pivots = pivotInfo.pivots;
        size_t num_buckets = pivotInfo.num_buckets;
        std::less<T> less;

        // now recursively sort each bucket
        parlay::parallel_for(0, num_buckets, [&](size_t i) {
            long first = offsets[i];
            long last = offsets[i + 1];
            if (last - first == 0) return; // empty
            
            if (i == 0 || i == num_buckets - 1 || less(pivots[i - 1], pivots[i])) {
                //auto slice = parlay::make_slice(keys.cut(first, last));
                //auto slice = keys.cut(first, last); 
                absl::Span<T> span(keys + first, last - first);
                //absl::Span<T> span(slice.begin(), slice.size());
                sorter.sort(span, level + 1);
            } /*else {
                absl::Span<T> dst(in.data() + first, last - first);
                parlay::copy(keys.cut(first,last), dst);
            }*/
          
                
        }, 1);
        parlay::parallel_for(0, in.size(), [&](size_t i) {
            in[i] = keys[i];
        });
    }
};