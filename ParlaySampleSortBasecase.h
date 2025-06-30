#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/slice.h>
#include "absl/types/span.h"

struct ParlaySampleSortBaseCase {
    template <typename T>
    static bool bcsort(absl::Span<T> in, int level) {
        long n = in.size();

        // for the base case (small or recursion level greater than 2) use std::sort
        long cutoff = 256;
        if (n <= cutoff || level > 2) {
            std::sort(in.begin(), in.end());
            return true;
        }
        return false;
    }
};