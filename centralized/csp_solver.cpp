#include "csp_solver.h"

void add_range_bin_sequence(double min_start, 
                            double max_start, 
                            std::vector<int> bin_sequence, 
                            std::vector<double> length_sequence) {
    min_starts.push_back(min_start);
    max_starts.push_back(max_start);
    bin_sequences.push_back(bin_sequence);
    range_sequences.push_back(std::vector<Range>());
    for(double length : length_sequence) {
        (range_sequences.end()-1)->push_back(Range(min_start, min_start+length));
        min_start += length;
    }
}

std::vector<int> find_conflicts() {
    
}

std::vector<double> solve() {

}