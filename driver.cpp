#include "parallel_samplesort.h"
#include "ParlaySampleSortBasecase.h"
#include "ParlaySampleSortRec.h"


int main() {
    size_t range = 100000;
    size_t sd = 42;
    int n = 1000000000;
    parlay::sequence<int> data(n);
    int num_workers = parlay::num_workers();

    parlay::parallel_for(0, num_workers, [&](size_t k) {
        std::mt19937 gen(sd + k);
        std::uniform_int_distribution<> dist(0, range);

        for (int i = n * k / num_workers; i < n * (k+ 1) / num_workers; i++){ 
            data[i] = dist(gen);
        }
    });

    /*for(int i = 0; i < 10; i++) {
        std::cout << data[i] << std::endl;
    }*/


    //parlay::sequence<int> data = {5, 3, 9, 1, 7, 4, 6, 2, 8, 0};
    //auto slice = parlay::make_slice(data);

    SampleSort<int, ParlaySampleSortBaseCase, ParlaySampleSortPivot, ParlaySampleSortPartition, ParlaySampleSortRec> sorter;
    sorter.sort(absl::Span<int>(data.data(), data.size()));

    auto sorted = is_sorted(data);
    std::cout << "is sorted: " << sorted << std::endl;

    /*for(int i = 0; i < 10; i++) {
        std::cout << data[i] << std::endl;
    }*/
}