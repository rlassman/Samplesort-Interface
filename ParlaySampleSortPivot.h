#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/slice.h>
#include <parlay/random.h>
#include "../parlaylib/examples/helper/heap_tree.h" // path to parlaylib, may need to change
#include "absl/types/span.h"

template <typename T>
struct PivotInfo {
    long num_buckets;
    parlay::sequence<T> pivots;
};

struct ParlaySampleSortPivot {
    template <typename T>
    static PivotInfo<T> sample(absl::Span<T> in) {
        long n = in.size();
        long cutoff = 256;

        // number of bits in bucket count (e.g. 8 would mean 256 buckets)
        int bits = std::min<long>(8, parlay::log2_up(n)-parlay::log2_up(cutoff)+1);
        long num_buckets = 1 << bits;

        // over-sampling ratio: keeps the buckets more balanced
        int over_ratio = 8;

        // create an over sample and sort it using std:sort
        parlay::random_generator gen;
        std::uniform_int_distribution<long> dis(0, n-1);
        auto oversample = parlay::tabulate(num_buckets * over_ratio, [&] (long i) {
            auto r = gen[i];
            return in[dis(r)];});
        std::sort(oversample.begin(), oversample.end());

        // sub sample to pick final pivots (num_buckets - 1 of them)
        auto pivots = parlay::tabulate(num_buckets-1, [&] (long i) {
            return oversample[(i+1)*over_ratio];});

        return PivotInfo<T>{num_buckets, std::move(pivots)}; // idk about move (it's passing by value), might want to use a pointer instead
    }

};