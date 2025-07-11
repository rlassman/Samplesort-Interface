#include <parlay/primitives.h>
#include <parlay/slice.h>
#include <parlay/random.h>
#include "ParlaySampleSortPivot.h"

template <typename T>
struct BucketInfo {
    parlay::sequence<T> keys;
    parlay::sequence<size_t> offsets;
};

struct ParlaySampleSortPartition {
    
    template <typename T>
    static BucketInfo<T> partition(absl::Span<T> in, const PivotInfo<T>& pivotInfo) {
        long n = in.size();
        long num_buckets = pivotInfo.num_buckets;
        const auto& pivots = pivotInfo.pivots;
        std::less<T> less;
        parlay::slice<int*, int*> slice = parlay::make_slice(in.data(), in.data() + in.size());

        // put pivots into efficient search tree and find buckets id for the input keys
        heap_tree tree(pivots);
        auto bucket_ids = parlay::tabulate(n, [&] (long i) -> unsigned char {
            return tree.find(in[i], less);});

        // sort into the buckets
        auto [keys,offsets] = parlay::internal::count_sort(slice, bucket_ids, num_buckets);
        return BucketInfo<T>{std::move(keys), std::move(offsets)};
        
    }

};