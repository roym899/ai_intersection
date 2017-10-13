#include "csp_solver.h"

#include <stack>
#include <iostream>
#include <limits>



void CSP_Solver::add_range_bin_constraint(int bin, Range range) {
    fixed_ranges.push_back(range);
    fixed_bins.push_back(bin);
}

void CSP_Solver::add_range_bin_sequence(double min_start, 
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

// returns a vector of arrays. these arrays have the following structure:
// { range_idx_1, bin_idx_1, range_idx_2, bin_idx_2}
std::vector< std::array<int, 5> > CSP_Solver::find_conflicts(const std::vector< std::vector<Range> >& range_sequences) {
    std::vector< std::array<int, 5> > conflicts;
    std::array<int, 5> conflict;

    for(int i=0; i<bin_sequences.size(); ++i) {
        for(int j=0; j<bin_sequences[i].size(); ++j) {
            for(int m=0; m<fixed_bins.size(); ++m) { // check for conflicts between movable range and fixed ranges
                if(bin_sequences[i][j] == fixed_bins[m] && overlap(range_sequences[i][j], fixed_ranges[m])) {
                    // indicate a conflict between fixed constraints with fifth parameter
                    conflict[0] = i;
                    conflict[1] = j;
                    conflict[2] = m;
                    conflict[4] = 1;
                    conflicts.push_back(conflict);
                }
            }   
        }
        for(int j=i+1; j<bin_sequences.size(); ++j) {
            for(int k=0; k<bin_sequences[i].size(); ++k) {
                for(int l=0; l<bin_sequences[j].size(); ++l) { // check for conflicts between movable ranges
                    if(bin_sequences[i][k] == bin_sequences[j][l] && overlap(range_sequences[i][k], range_sequences[j][l])) {
                        conflict[0] = i;
                        conflict[1] = k;
                        conflict[2] = j;
                        conflict[3] = l;
                        conflict[4] = 0;
                        conflicts.push_back(conflict);
                    }
                }
            }
        }
    }

    return conflicts;
}

void CSP_Solver::solve(std::vector< std::vector<double> >& solutions,
                       int depth) {
    // find the conflicts for the given iteration

    // check order constraint
    for(int i=0; i<bin_sequences.size(); ++i) {
        for(int j=i+1; j<bin_sequences.size(); ++j) {
            if(*bin_sequences[i].begin() == *bin_sequences[j].begin() && range_sequences[j].begin()->get_start() < range_sequences[i].begin()->get_start()) {
                return;
            }
        }
    }

    std::vector< std::array<int, 5> > conflicts = find_conflicts(range_sequences);
    std::stack< std::array<int, 5> > shift_stack;

    // if here is no conflict, end the recursion at this point and add the state to the solutions
    if(conflicts.size() == 0) {
        // no conflicts left -> stop at this node
        std::vector<double> start_times;
        for(int i=0; i<range_sequences.size(); ++i) {
            start_times.push_back(range_sequences[i].begin()->get_start());
        }
        solutions.push_back(start_times);
        return;
    }

    // go deeper into search tree if max depth not reched yet
    if(depth > 0) {
        // add conflicts in right order to the search stack
        for(const auto& conflict : conflicts ) {
            // two ways to potentially solve a conflict: shift first back or shift second one back
            std::array<int, 5> new_shift_first;
            new_shift_first[0] = conflict[0];
            new_shift_first[1] = conflict[1];
            new_shift_first[2] = conflict[2];
            new_shift_first[3] = conflict[3];
            new_shift_first[4] = conflict[4];
            std::array<int, 5> new_shift_second;
            new_shift_second[0] = conflict[2];
            new_shift_second[1] = conflict[3];
            new_shift_second[2] = conflict[0];
            new_shift_second[3] = conflict[1];
            new_shift_second[4] = conflict[4];

            if (conflict[4] == 1) { // conflict with fixed constraint, only one way to solve
                shift_stack.push(new_shift_first);
            }
            else if(conflict[1] == 0 && conflict[3] == 0) {// starting bin has conflict with another starting bin, there should be no switching in order!
                // only one solution viable here
                if( conflict[0] < conflict[2])
                    shift_stack.push(new_shift_second);
                else
                    shift_stack.push(new_shift_first);
            }
            else {
                // start with good solution, e.g. shift the one with earlier end time back
                if( (range_sequences[conflict[0]].end()-1)->get_end() <  (range_sequences[conflict[2]].end()-1)->get_end()) {
                    shift_stack.push(new_shift_second);
                    shift_stack.push(new_shift_first); // putting first shift on the stack second means processing this shift first
                } 
                else {
                    shift_stack.push(new_shift_first);
                    shift_stack.push(new_shift_second);
                }
            }
        }

        // as long as stack not empty continue the search
        while(shift_stack.size() != 0) {
            std::array<int, 5> shift_array = shift_stack.top();
            double time_shift = get_shift(shift_array);

            // stop if shift behind the max_start is needed
            if ( range_sequences[shift_array[0]].begin()->get_start() + time_shift > max_starts[shift_array[0]]) {
                shift_stack.pop();
                continue;
            }

            // apply the shift operation
            for(auto & range : range_sequences[shift_array[0]]) {
                range.shift(time_shift);
            }

            solve(solutions, depth-1);

            // undo the shift operation            
            for(auto & range : range_sequences[shift_array[0]]) {
                range.shift(-time_shift);
            }
            shift_stack.pop();
        }
    }

 }

std::vector<double> CSP_Solver::solve() {
    std::vector< std::vector<double> > solutions;
    const int max_depth = 10;

    for(int depth=0; depth < max_depth; ++depth) {
        std::cerr << "Analyzing depth " << depth << "...\n";
        solve(solutions, depth);
        if(solutions.size() > 0)
            break;
    }


    // loop through all solutions and search for the best
    std::vector<double> best_solution;
    double best_finish_time = std::numeric_limits<double>::max();
    int counter = 0;
    double longest_end_time = 0;
    double current_end_time = 0;

    double range_start = 0;
    double range_end = 0;
    double range_difference = 0;

    std::vector<Range> current_ranges;
    // takes the solution with the minimal time at which the last car leaves the intersection
    // takes the the arrival time from the last car and adds the time it spends on the intersection center
    for(const std::vector<double>& solution: solutions){
        counter = 0;
        longest_end_time = 0;
        for(const double& arrive_time: solution){
            current_ranges = range_sequences[counter];
            range_start = current_ranges[0].get_start();
            range_end = current_ranges.back().get_end();
            range_difference = range_end - range_start;
            current_end_time = arrive_time + range_difference;
            if(current_end_time > longest_end_time){
                longest_end_time = current_end_time;
            }
            counter++;
        }
        
        if(longest_end_time < best_finish_time){
            best_solution = solution;
            best_finish_time = longest_end_time;
        }
        
    }

    std::cerr << "Final solution (chosen from " << solutions.size() << "): ";

    for(double time : best_solution) {
        std::cerr << time << " ";
    }
    std::cerr << std::endl;

    return best_solution;
}


double CSP_Solver::get_shift(const std::array<int, 5>& shift_array) const {
    if (shift_array[4] == 1) // shift variable array against fixed constraint range
        return fixed_ranges[shift_array[2]].get_end() - range_sequences[shift_array[0]][shift_array[1]].get_start();
    else
        return range_sequences[shift_array[2]][shift_array[3]].get_end() - range_sequences[shift_array[0]][shift_array[1]].get_start();

}

// checks if two ranges overlap or not
bool overlap(const Range &lhs, const Range &rhs) {
    if(lhs.end <= rhs.start || lhs.start >=rhs.end)
        return false;
    else
        return true;
}
