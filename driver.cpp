#include "parallel_samplesort.h"
#include "ParlaySampleSortBasecase.h"
//#include "ParlaySampleSortPivot.h"
//#include "ParlaySampleSortPartition.h"
#include "ParlaySampleSortRec.h"


int main() {
    size_t range = 1'000'000'000;
    size_t sd = 42;
    int n = 100;
    parlay::sequence<int> data(n);
    int num_workers = parlay::num_workers();

    // preventing work-stealing so the input generated is deterministic
    parlay::parallel_for(0, num_workers, [&](size_t k) {
        std::mt19937 gen(sd + k);
        std::uniform_int_distribution<> dist(0, range);

        for (int i = n * k / num_workers; i < n * (k+ 1) / num_workers; i++){ 
            data[i] = dist(gen);
        }
    });


    //parlay::sequence<int> data = {5, 3, 9, 1, 7, 4, 6, 2, 8, 0};
    //auto slice = parlay::make_slice(data);

    SampleSort<int, ParlaySampleSortBaseCase, ParlaySampleSortPivot, ParlaySampleSortPartition, ParlaySampleSortRec> sorter;
    sorter.sort(absl::Span<int>(data.data(), data.size()));

    auto sorted = is_sorted(data);
    std::cout << sorted << std::endl;

    //for (int x : data) std::cout << x << " ";
    //std::cout << std::endl;
}