#include "csp_solver.h"

#include <stack>
#include <iostream>

// TODO:
// handling for: same lane no passing allowed
// constant constraints for cars already on intersection
// handling for max time constraint

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
std::vector< std::array<int, 4> > CSP_Solver::find_conflicts(const std::vector< std::vector<Range> >& range_sequences) {
    std::vector< std::array<int, 4> > conflicts;
    std::array<int, 4> conflict;

    std::cerr << "Searching for conflicts in " << bin_sequences.size() << "sequences" << std::endl;

    for(int i=0; i<bin_sequences.size(); ++i) {
        for(int j=i+1; j<bin_sequences.size(); ++j) {
            for(int k=0; k<bin_sequences[i].size(); ++k) {
                for(int l=0; l<bin_sequences[j].size(); ++l) {
                    if(bin_sequences[i][k] == bin_sequences[j][l] && overlap(range_sequences[i][k], range_sequences[j][l])) {
                        conflict[0] = i;
                        conflict[1] = k;
                        conflict[2] = j;
                        conflict[3] = l;
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
    std::vector< std::array<int, 4> > conflicts = find_conflicts(range_sequences);
    std::stack< std::array<int, 4> > shift_stack;

    std::cerr << conflicts.size() << " conflicts found\n";

    // if here is no conflict, end the recursion at this point and add the state to the solutions
    if(conflicts.size() == 0) {
        std::cerr << "No conflict found...\n";
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
            std::array<int, 4> new_shift_first;
            new_shift_first[0] = conflict[0];
            new_shift_first[1] = conflict[1];
            new_shift_first[2] = conflict[2];
            new_shift_first[3] = conflict[3];
            std::array<int, 4> new_shift_second;
            new_shift_second[0] = conflict[2];
            new_shift_second[1] = conflict[3];
            new_shift_second[2] = conflict[0];
            new_shift_second[3] = conflict[1];

            if(conflict[1] == 0 && conflict[3] == 0) {// starting bin has conflict with another starting bin, there should be no switching in order!
                // start with good solution, e.g. shift the one with earlier end time back
                if( conflict[0] < conflict[2])
                    shift_stack.push(new_shift_second);
                else
                    shift_stack.push(new_shift_first);
            }
            else {
                // start with good solution, e.g. shift the one with earlier end time back
                if( (range_sequences[conflict[0]].end()-1)->get_end() <  (range_sequences[conflict[3]].end()-1)->get_end()) {
                    shift_stack.push(new_shift_second);
                    shift_stack.push(new_shift_first); // putting first shift on the stack second means processing this shift first
                } 
                else {
                    shift_stack.push(new_shift_first);
                    shift_stack.push(new_shift_second);
                }
            }

            // TODO handle special case of same first bin (no passing on same lane)

        }

        // as long as stack not empty continue the search
        while(shift_stack.size() != 0) {
            std::array<int, 4> shift_array = shift_stack.top();
            double time_shift = get_shift(shift_array);
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

    std::cerr << "Final solution: ";

    for(double time : solutions[0]) {
        std::cerr << time << " ";
    }
    std::cerr << std::endl;

    // TODO: return the best, not the first solution
    return solutions[0];
}


double CSP_Solver::get_shift(const std::array<int, 4>& shift_array) const {
    return range_sequences[shift_array[2]][shift_array[3]].get_end() - range_sequences[shift_array[0]][shift_array[1]].get_start();
}

// checks if two ranges overlap or not
bool overlap(const Range &lhs, const Range &rhs) {
    if(lhs.end <= rhs.start || lhs.start >=rhs.end)
        return false;
    else
        return true;
}