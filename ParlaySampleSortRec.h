#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/slice.h>
#include "ParlaySampleSortPartition.h"

struct ParlaySampleSortRec {
    template <typename T, typename Sorter>
    static void recSort(BucketInfo<T> bucketInfo, PivotInfo<T> pivotInfo, int level, Sorter& sorter) {
        auto keys = bucketInfo.keys;
        auto offsets = bucketInfo.offsets;
        auto pivots = pivotInfo.pivots;
        size_t num_buckets = pivotInfo.num_buckets;
        std::less<T> less;

        // now recursively sort each bucket
        parlay::parallel_for(0, num_buckets, [&](size_t i) {
            long first = offsets[i];
            long last = offsets[i + 1];
            if (last - first == 0) return; // empty
            
            if (i == 0 || i == num_buckets - 1 || less(pivots[i - 1], pivots[i])) {
                auto slice = parlay::make_slice(keys.cut(first, last));
                sorter.sort(slice, level + 1);  // recursive call on interface
            }
        }, 1);
    }
};